/*
 * Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/ 
 *  
 *  
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met:
 * 
 * 	* Redistributions of source code must retain the above copyright 
 * 	  notice, this list of conditions and the following disclaimer.
 * 
 * 	* Redistributions in binary form must reproduce the above copyright
 * 	  notice, this list of conditions and the following disclaimer in the 
 * 	  documentation and/or other materials provided with the   
 * 	  distribution.
 * 
 * 	* Neither the name of Texas Instruments Incorporated nor the names of
 * 	  its contributors may be used to endorse or promote products derived
 * 	  from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


extern "C" {
#include <stdint.h>
#include <stdio.h>
#include <pru_cfg.h>
#include <pru_intc.h>
#include <rsc_types.h>
#include <pru_rpmsg.h>
#include "resource_table_0.h"
}

#include "common.h"
#include "adc.h"
#include "hal.h"
#include "scheduled_output_action.h"
#include "timer.h"
#include "util.h"
#include "msg_definition.h"
#include "light_barrier.h"
#include "controller.h"
#include "version.h"
#include "motor.h"
#include "piston.h"
#include "hw.h"
#include "queue.h"
#include "compressor.h"
#include "command_interface.h"
#include "color_detect.h"
#include "object_pool.h"
#include "brick_eject_command.h"

volatile register uint32_t __R30;
volatile register uint32_t __R31;


/* Host-0 Interrupt sets bit 30 in register R31 */
#define HOST_INT			((uint32_t) 1 << 30)


/* The PRU-ICSS system events used for RPMsg are defined in the Linux device tree
 * PRU0 uses system event 16 (To ARM) and 17 (From ARM)
 * PRU1 uses system event 18 (To ARM) and 19 (From ARM)
 */
#define TO_ARM_HOST			16
#define FROM_ARM_HOST		17

/*
 * Using the name 'rpmsg-pru' will probe the rpmsg_pru driver found
 * at linux-x.y.z/drivers/rpmsg/rpmsg_pru.c
 */
#define CHAN_NAME					"rpmsg-pru"
#define CHAN_DESC					"Channel 30"
#define CHAN_PORT					30

/* 
 * Used to make sure the Linux drivers are ready for RPMsg communication
 * Found at linux-x.y.z/include/uapi/linux/virtio_config.h
 */
#define VIRTIO_CONFIG_S_DRIVER_OK	4

uint8_t payload[RPMSG_BUF_SIZE];

uint8_t adc_values[200];
uint8_t adc_value_index = 0;


static const uint32_t ADC_LIMIT_TOLERANCE = 60;
static const uint32_t ADC_NO_OBJECT_LIMIT = 0x4be;
static const uint32_t ADC_BLUE_OBJECT_LIMIT = 0x49c;
static const uint32_t ADC_RED_OBJECT_LIMIT = 0x32a;
static const uint32_t ADC_WHITE_OBJECT_LIMIT = 0x2ef;

enum Mode { MODE_NORMAL, MODE_DIAGNOSTIC };

enum SortOrder { BLUE_RED_WHITE };


bool is_controller_started;
bool rpmsg_connected;
uint32_t pulsecounter_last_change;
bool is_conveyor_running;
struct pru_rpmsg_transport transport;

// A message will need to be sent to the PRU before the PRU will know the destination address.
// https://e2e.ti.com/support/arm/sitara_arm/f/791/p/543704/1983379
uint16_t src;   // from where the message was received
uint16_t dst;   // where the message is sent to
uint16_t len;
uint32_t now;
uint32_t last_all_inputs_value;

uint32_t adc_last_measurement;
uint32_t adc_min_value;
uint32_t lightbarrier1_last_change;
uint32_t lightbarrier2_last_change;
uint32_t lightbarriers3_to_5_last_change;

Motor motor(MOTOR_MASK);
//Compressor compressor(MOTOR_MASK)
Piston piston[] = { Piston(VALVE1_MASK), Piston(VALVE2_MASK), Piston(VALVE3_MASK) };
LightBarrier lightBarrier[] = { LightBarrier(LIGHTBARRIER1_MASK), LightBarrier(LIGHTBARRIER2_MASK), LightBarrier(LIGHTBARRIERS3_TO_5_MASK) };


Hw hw(motor,
      //compressor
        piston[0],
        piston[1],
        piston[2],
        lightBarrier[0],
        lightBarrier[1],
        lightBarrier[2]);



Controller ctrl(hw);
Timer timer;
ColorDetect colorDetect(timer);

Queue<Color,3> detected_colors;
ObjectPool<BrickEjectCommand, 3> ejectCommandPool;

bool verbose;

int16_t post_event(char const *event, uint16_t length)
{
  if (!rpmsg_connected) {
    return -4; //RPMSG_NOT_CONNECTED
  }
  return pru_rpmsg_send(&transport, dst, src, (void*)event, length);
}

int16_t post_info(char info)
{
  if (!rpmsg_connected) {
    return -4; //RPMSG_NOT_CONNECTED
  }
  return pru_rpmsg_send(&transport, dst, src, (void*)&info, 1);
}

/*
 * @todo implement again
std::list<ScheduledOutputAction> pusher_actions;
*/
void schedule_pusher_action(uint32_t timestamp, uint32_t bitmask, bool value)
{
  ScheduledOutputAction action(timestamp, bitmask, value);
  // @todo implement again
  //pusher_actions.push_back(action);
}

void check_scheduled_pusher_actions()
{
  return;
  /*
   * @todo reimplement
  if (pusher_actions.size() == 0) {
    return;
  }
  ScheduledOutputAction &next_action = pusher_actions.front();
  if (next_action.timestamp > now) {
    return;
  }
  char buffer[29] = "valve";
  uint32_t length = 5;
  buffer[length] = 'X';
  if (next_action.bitmask == VALVE1_MASK) {
    buffer[length] = '1';
  }
  else if (next_action.bitmask == VALVE2_MASK) {
    buffer[length] = '2';
  }
  else if (next_action.bitmask == VALVE3_MASK) {
    buffer[length] = '3';
  }
  length += 1;
  buffer[length] = '=';
  length += 1;
  if (next_action.value) {
    //strcpy(&buffer[length], "on");
    length += 2;
  }
  else {
    //strcpy(&buffer[length], "off");
    length += 3;
  }
  buffer[length] = '\n';
  length += 1;
  //post_event(buffer, length);
  if (next_action.value) {
    __R30 |= next_action.bitmask;
  }
  else {
    __R30 &= ~next_action.bitmask;
  }
  pusher_actions.pop_front();
  */
}

// @reimplement
//std::list<uint32_t> adc_actions;

void schedule_adc_action(uint32_t timestamp)
{
  // @todo reimplement
    //adc_actions.push_back(timestamp);
}

void check_scheduled_adc_actions()
{
  return;
  /*
  if (adc_actions.size() == 0) {
    return;
  }
  uint32_t &next_action = adc_actions.front();
  if (now < next_action - 22) {
    return;
  }
  if (now == adc_last_measurement) {
    return;
  }
  uint16_t value = adc_read();
  adc_last_measurement = now;
  if (value < adc_min_value) {
    adc_min_value = value;
  }
  if (now < next_action + 22) {
    return;
  }
  adc_actions.pop_front();
  if (verbose) {
    char buffer[3];//= "log: adc_min_value=0xXXXXXXXX\n";
    buffer[0] = INFO_COLOR_DETECT;
    buffer[1] = adc_min_value & 0xff;
    buffer[2] = (adc_min_value >> 8 ) & 0xff;
    //appendNumber(&buffer[21], adc_min_value);
    post_event(buffer, 3);
  }
  Color color;
  char *color_string;
  uint32_t color_length;
  if (adc_min_value >= ADC_NO_OBJECT_LIMIT) {
    color = UNKNOWN;
    color_string = "?";
    color_length = 1;
  }
  else if ((adc_min_value <= ADC_BLUE_OBJECT_LIMIT + ADC_LIMIT_TOLERANCE) &&
           (adc_min_value >= ADC_BLUE_OBJECT_LIMIT)) {
    color = BLUE;
    color_string = "blue";
    color_length = 4;
  }
  else if ((adc_min_value <= ADC_RED_OBJECT_LIMIT + ADC_LIMIT_TOLERANCE) &&
           (adc_min_value >= ADC_RED_OBJECT_LIMIT)) {
    color = RED;
    color_string = "red";
    color_length = 3;
  }
  else if ((adc_min_value <= ADC_WHITE_OBJECT_LIMIT + ADC_LIMIT_TOLERANCE) &&
           (adc_min_value >= ADC_WHITE_OBJECT_LIMIT)) {
    color = WHITE;
    color_string = "white";
    color_length = 5;
  }
  else {
    color = UNKNOWN;
    color_string = "?";
    color_length = 1;
  }
  char buffer[12] = "color=";
  //strcpy(&buffer[6], color_string);
  buffer[6 + color_length] = '\n';
  //post_event(buffer, 6 + color_length + 1);
  if (is_controller_started && (color != UNKNOWN)) {
    detected_colors.push_back(color);
  }
  adc_min_value = 0xFFFF;
  */
}
/*
void on_input_change(uint32_t mask, bool value)
{
  if (mask == LIGHTBARRIERS3_TO_5_MASK) {
    if (now - lightbarriers3_to_5_last_change < 5) {
      return;
    }
    lightbarriers3_to_5_last_change = now;
    if (value) {
      post_info(INFO_EMERGENCY_STOP_ON);
      if (is_controller_started) {
        __R30 &= ~MOTOR_MASK;
        post_info(INFO_MOTOR_STOP);
        is_controller_started = false;
        post_info(INFO_CTRL_STOP);
      }
    }
    else {
      post_info(INFO_EMERGENCY_STOP_OFF);
    }
  }
  else if (mask == PULSECOUNTER_MASK) {
    pulsecounter_last_change = timer.get_ticks();
    if (!is_conveyor_running) {
      is_conveyor_running = true;
      post_info(INFO_CONVEYER_RUNNING);
    }
  }
  else if (mask == LIGHTBARRIER1_MASK) {
    if (now - lightbarrier1_last_change < 5) {
      return;
    }
    lightbarrier1_last_change = now;
    if (value) {
      post_info(INFO_LIGHT_BARRIER_1_ON);
    }
    else {
      post_info(INFO_LIGHT_BARRIER_1_OFF);
      schedule_adc_action(now + 111);
    }
    if (verbose) {
      char buffer[] = "now=0xXXXXXXXX\n";
      appendNumber(&buffer[6], now);
      //post_event(buffer, 15);
    }
  }
  else if (mask == LIGHTBARRIER2_MASK) {
    if (now - lightbarrier2_last_change < 5) {
      return;
    }
    lightbarrier2_last_change = now;
    if (value) {
      post_info(INFO_LIGHT_BARRIER_2_ON);
      if (is_controller_started) {
        if (detected_colors.isEmpty()) {
          //post_event("debug: No colored object detected. Letting it pass...\n", 54);
        }
        else {
          Color color = detected_colors.pull();
          switch (color) {
            case BLUE:
              schedule_pusher_action(now + 69, VALVE1_MASK, true);
              schedule_pusher_action(now + 99, VALVE1_MASK, false);
              break;
            case RED:
              schedule_pusher_action(now + 166, VALVE2_MASK, true);
              schedule_pusher_action(now + 196, VALVE2_MASK, false);
              break;
            case WHITE:
              schedule_pusher_action(now + 264, VALVE3_MASK, true);
              schedule_pusher_action(now + 294, VALVE3_MASK, false);
              break;
            case UNKNOWN:
            default:
              break;
          }
        }
      }
    }
    else {
      post_info(INFO_LIGHT_BARRIER_2_OFF);
    }
    if (verbose) {
      char buffer[] = "now=0xXXXXXXXX\n";
      appendNumber(&buffer[6], now);
      //post_event(buffer, 15);
    }
  }

  if (value) {
    last_all_inputs_value |= mask;
  }
  else {
    last_all_inputs_value &= ~mask;
  }
}

void process_input(uint32_t all_inputs_value, uint32_t mask)
{
  bool value = all_inputs_value & mask;
  bool last_value = last_all_inputs_value & mask;
  if (value != last_value) {
    on_input_change(mask, value);
  }
}

void process_inputs(uint32_t all_inputs_value)
{
  process_input(all_inputs_value, LIGHTBARRIERS3_TO_5_MASK);
  process_input(all_inputs_value, PULSECOUNTER_MASK);
  process_input(all_inputs_value, LIGHTBARRIER1_MASK);
  process_input(all_inputs_value, LIGHTBARRIER2_MASK);

  if (is_conveyor_running) {
    int32_t ticks_since_last_change = now - pulsecounter_last_change;
    if (ticks_since_last_change > 100) {
      is_conveyor_running = false;
      post_info(INFO_CONVEYER_STOPPED);
    }
  }
}
*/
bool get_last_input(uint32_t mask)
{
  return last_all_inputs_value & mask;
}


static void checkArmToPruMsg()
{
    /* Check bit 30 of register R31 to see if the mailbox interrupt has occurred */
    if (__R31 & HOST_INT)
    {
        /* Clear the event status */
        CT_INTC.SICR_bit.STS_CLR_IDX = FROM_ARM_HOST;


        /* Use a while loop to read all of the current messages in the mailbox */
        while (pru_rpmsg_receive(&transport, &src, &dst, payload, &len) == PRU_RPMSG_SUCCESS)
        {
            rpmsg_connected = true;
            bool skip_echo = false;

            ctrl.processCmd((payload[0]));

            if (!verbose) {
              if(payload[0] == CMD_VERBOSE_ON) {
                verbose = true;
              }
            }
            if (verbose) {
              if (payload[0] == CMD_VERBOSE_OFF) {
                verbose = false;
              }
            }

            if (!skip_echo) {
              /* Echo the message back to the same address from which we just received */
              pru_rpmsg_send(&transport, dst, src, payload, len);
            }
        }
    }
}

class MessageAction: public CommandInterface
{
    void execute()
    {


        uint16_t value = adc_read();

        adc_values[adc_value_index++] = value & 0xff;
        adc_values[adc_value_index++] = (value >> 8) & 0xff;
        if(adc_value_index >= 200)
        {
            adc_value_index = 0;
        }

        timer.schedule(this,5);
    }
};

MessageAction messageAction;

void main() {
    bool unhandled = true;

/*  is_controller_started = false;
  rpmsg_connected = false;
  pulsecounter_last_change = 0;
  is_conveyor_running = false;
  src=0xFFFF;
  dst=0xFFFF;
  len=0xFFFF;
  now = 0;
  last_all_inputs_value = get_all_inputs();
  __R30 = 0;
  adc_last_measurement = 0;
  adc_min_value = 0xFFFF;
  lightbarrier1_last_change = 0;
  lightbarrier2_last_change = 0;
  lightbarriers3_to_5_last_change = 0;
  verbose = false;

  volatile uint8_t *status;
*/
    /* allow OCP master port access by the PRU so the PRU can read external memories */
//  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  //struct pru_rpmsg_transport transport;
  uint16_t src, dst, len;
  volatile uint8_t *status;

  /* Allow OCP master port access by the PRU so the PRU can read external memories */
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  /* Clear the status of the PRU-ICSS system event that the ARM will use to 'kick' us */
  CT_INTC.SICR_bit.STS_CLR_IDX = FROM_ARM_HOST;

  /* Make sure the Linux drivers are ready for RPMsg communication */
  status = &resourceTable.rpmsg_vdev.status;
  while (!(*status & VIRTIO_CONFIG_S_DRIVER_OK));

  /* Initialize the RPMsg transport structure */
  pru_rpmsg_init(&transport, &resourceTable.rpmsg_vring0, &resourceTable.rpmsg_vring1, TO_ARM_HOST, FROM_ARM_HOST);

  /* Create the RPMsg channel between the PRU and ARM user space using the transport structure. */
  while (pru_rpmsg_channel(RPMSG_NS_CREATE, &transport, CHAN_NAME, CHAN_DESC, CHAN_PORT) != PRU_RPMSG_SUCCESS);

  for(int i = 0; i < 200; i++)
  {
      adc_values[i] = 0xaa;
  }

  adc_init();
  timer.start();
  //timer.schedule(&messageAction,100);
  timer.schedule(&colorDetect,100);


  while (1) {
    timer.poll();
    //now = timer_get_ticks();
    //timer.poll();


    if(lightBarrier[1].isInterrupted()) // brick after color detection?
    {
        if(unhandled)   // brick unhandled?
        {
            BrickEjectCommand *command = ejectCommandPool.getObject();
            if(command)
            {

                command->ejectColor(hw, timer, RED);
                unhandled = false;
            }
            else
            {
                // @todo error handling, command pool exhausted
            }
        }
    }
    else // no brick
    {
        unhandled = false;
    }


    checkArmToPruMsg();
    //process_inputs(get_all_inputs());
    //check_scheduled_pusher_actions();
    //check_scheduled_adc_actions();
  }
}
