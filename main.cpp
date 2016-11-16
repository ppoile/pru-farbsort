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

#include <list>
#include <string>

#include <stdint.h>
#include <stdio.h>
#include <string.h>

extern "C" {
#include <pru_cfg.h>
#include <pru_intc.h>
#include <rsc_types.h>
#include <sys_mailbox.h>
#include <pru_virtqueue.h>
#include <pru_rpmsg.h>
#include "resource_table_0.h"
}

#include "adc.h"
#include "scheduled_output_action.h"
#include "timer.h"


volatile register uint32_t __R30;
volatile register uint32_t __R31;

void appendNumber(char *buffer, uint32_t value)
{
  char *p = buffer;
  for (int nibble_index = 7; nibble_index >= 0; --nibble_index) {
    uint32_t nibble = (value >> (nibble_index * 4)) & 0xF;
    char ch;
    if (nibble <= 9) {
      ch = '0' + nibble;
    }
    else {
      ch = 'a' + nibble - 10;
    }
    *p++ = ch;
  }
}

/* PRU0 is mailbox module user 1 */
#define MB_USER						1
/* Mbox0 - mail_u1_irq (mailbox interrupt for PRU0) is Int Number 60 */
#define MB_INT_NUMBER				60

/* Host-0 Interrupt sets bit 30 in register R31 */
#define HOST_INT					0x40000000

/* The mailboxes used for RPMsg are defined in the Linux device tree
 * PRU0 uses mailboxes 2 (From ARM) and 3 (To ARM)
 * PRU1 uses mailboxes 4 (From ARM) and 5 (To ARM)
 */
#define MB_TO_ARM_HOST				3
#define MB_FROM_ARM_HOST			2

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

static const uint32_t MOTOR_MASK = 0x8000;
static const uint32_t VALVE1_MASK = 0x4000;
static const uint32_t VALVE2_MASK = 0x80;
static const uint32_t VALVE3_MASK = 0x20;
static const char motor_stop[] = "motor=stop\n";
static const char motor_start[] = "motor=start\n";
static const char valve1_off[] = "valve1=off\n";
static const char valve2_off[] = "valve2=off\n";
static const char valve3_off[] = "valve3=off\n";

static const uint32_t PULSECOUNTER_MASK = 0x8000;
static const uint32_t LIGHTBARRIER1_MASK = 0x4000;
static const uint32_t LIGHTBARRIER2_MASK = 0x10000;
static const uint32_t LIGHTBARRIERS3_TO_5_MASK = 0x4;
static const char lightbarrier1_on[] = "lightbarrier1=on\n";
static const char lightbarrier1_off[] = "lightbarrier1=off\n";
static const char lightbarrier2_on[] = "lightbarrier2=on\n";
static const char lightbarrier2_off[] = "lightbarrier2=off\n";
static const char emergency_stop_on[] = "emergency-stop=on\n";
static const char emergency_stop_off[] = "emergency-stop=off\n";

static const uint32_t ADC_LIMIT_TOLERANCE = 60;
static const uint32_t ADC_NO_OBJECT_LIMIT = 0x4be;
static const uint32_t ADC_BLUE_OBJECT_LIMIT = 0x49a;
static const uint32_t ADC_RED_OBJECT_LIMIT = 0x343;
static const uint32_t ADC_WHITE_OBJECT_LIMIT = 0x307;

enum Mode { MODE_NORMAL, MODE_DIAGNOSTIC };
static const char mode_normal[] = "mode=normal\n";
static const char controller_stopped[] = "controller=stopped\n";
static const char controller_started[] = "controller=started\n";
static const char conveyor_running[] = "conveyor=running\n";
static const char conveyor_stopped[] = "conveyor=stopped\n";

enum Color { BLUE, RED, WHITE, UNKNOWN };
enum SortOrder { BLUE_RED_WHITE };

static const char sort_order_blue_red_white[] = "sort-order=blue-red-white\n";

Mode mode;
SortOrder sort_order;
bool is_controller_started;
bool rpmsg_connected;
uint32_t pulsecounter_last_change;
bool is_conveyor_running;
struct pru_rpmsg_transport transport;
uint16_t src, dst, len;
uint32_t now;
uint32_t last_all_inputs_value;

uint32_t adc_last_measurement;
uint32_t adc_min_value;
uint32_t lightbarrier1_last_change;
uint32_t lightbarrier2_last_change;
uint32_t lightbarriers3_to_5_last_change;

std::list<Color> detected_colors;


int16_t post_event(char const *event, uint16_t length)
{
  if (!rpmsg_connected) {
    return -4; //RPMSG_NOT_CONNECTED
  }
  return pru_rpmsg_send(&transport, dst, src, (void*)event, length);
}

std::list<ScheduledOutputAction> pusher_actions;

void schedule_pusher_action(uint32_t timestamp, uint32_t bitmask, bool value)
{
  ScheduledOutputAction action(timestamp, bitmask, value);
  pusher_actions.push_back(action);
}

void check_scheduled_pusher_actions()
{
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
    strcpy(&buffer[length], "on");
    length += 2;
  }
  else {
    strcpy(&buffer[length], "off");
    length += 3;
  }
  buffer[length] = '\n';
  length += 1;
  post_event(buffer, length);
  if (next_action.value) {
    __R30 |= next_action.bitmask;
  }
  else {
    __R30 &= ~next_action.bitmask;
  }
  pusher_actions.pop_front();
}

std::list<uint32_t> adc_actions;

void schedule_adc_action(uint32_t timestamp)
{
  adc_actions.push_back(timestamp);
}

void check_scheduled_adc_actions()
{
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
  strcpy(&buffer[6], color_string);
  buffer[6 + color_length] = '\n';
  post_event(buffer, 6 + color_length + 1);
  if (is_controller_started && (color != UNKNOWN)) {
    detected_colors.push_back(color);
  }
  adc_min_value = 0xFFFF;
}

void on_input_change(uint32_t mask, int value, int last_value)
{
  if (mask == LIGHTBARRIERS3_TO_5_MASK) {
    if (now - lightbarriers3_to_5_last_change < 5) {
      return;
    }
    lightbarriers3_to_5_last_change = now;
    if (value) {
      post_event(emergency_stop_on, 18);
      if (is_controller_started) {
        __R30 &= ~MOTOR_MASK;
        post_event(motor_stop, 11);
        is_controller_started = false;
        post_event(controller_stopped, 19);
      }
    }
    else {
      post_event(emergency_stop_off, 19);
    }
  }
  if (mask == PULSECOUNTER_MASK) {
    pulsecounter_last_change = timer_get_ticks();
    if (!is_conveyor_running) {
      is_conveyor_running = true;
      post_event(conveyor_running, 17);
    }
  }
  if (mask == LIGHTBARRIER1_MASK) {
    if (now - lightbarrier1_last_change < 5) {
      return;
    }
    lightbarrier1_last_change = now;
    if (value) {
      post_event(lightbarrier1_on, 17);
    }
    else {
      post_event(lightbarrier1_off, 18);
      schedule_adc_action(now + 111);
    }
  }
  if (mask == LIGHTBARRIER2_MASK) {
    if (now - lightbarrier2_last_change < 5) {
      return;
    }
    lightbarrier2_last_change = now;
    if (value) {
      post_event(lightbarrier2_on, 17);
      if (is_controller_started) {
        if (detected_colors.size() == 0) {
          post_event("debug: No colored object detected. Letting it pass...\n", 54);
          return;
        }
        Color color = detected_colors.front();
        detected_colors.pop_front();
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
            schedule_pusher_action(now + 274, VALVE3_MASK, true);
            schedule_pusher_action(now + 304, VALVE3_MASK, false);
            break;
          case UNKNOWN:
          default:
            break;
        }
      }
    }
    else {
      post_event(lightbarrier2_off, 18);
    }
  }
}

void process_input(uint32_t all_inputs_value, uint32_t mask)
{
  bool value = !(!(all_inputs_value & mask));
  bool last_value = !(!(last_all_inputs_value & mask));
  if (value != last_value) {
    on_input_change(mask, value, last_value);
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
      post_event(conveyor_stopped, 17);
    }
  }

  last_all_inputs_value = all_inputs_value;
}

bool get_last_input(uint32_t mask)
{
  return !(!(last_all_inputs_value & mask));
}

void main() {
  mode = MODE_NORMAL;
  sort_order = BLUE_RED_WHITE;
  is_controller_started = false;
  rpmsg_connected = false;
  pulsecounter_last_change = 0;
  is_conveyor_running = false;
  src=0xFFFF;
  dst=0xFFFF;
  len=0xFFFF;
  now = 0;
  last_all_inputs_value = __R31;
  __R30 = 0;
  adc_last_measurement = 0;
  adc_min_value = 0xFFFF;
  lightbarrier1_last_change = 0;
  lightbarrier2_last_change = 0;
  lightbarriers3_to_5_last_change = 0;

  volatile uint8_t *status;

  /* allow OCP master port access by the PRU so the PRU can read external memories */
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  /* clear the status of event MB_INT_NUMBER (the mailbox event) and enable the mailbox event */
  CT_INTC.SICR_bit.STS_CLR_IDX = MB_INT_NUMBER;
  CT_MBX.IRQ[MB_USER].ENABLE_SET |= 1 << (MB_FROM_ARM_HOST * 2);

  /* Make sure the Linux drivers are ready for RPMsg communication */
  status = &resourceTable.rpmsg_vdev.status;
  while (!(*status & VIRTIO_CONFIG_S_DRIVER_OK));

  /* Initialize pru_virtqueue corresponding to vring0 (PRU to ARM Host direction) */
  pru_virtqueue_init(&transport.virtqueue0, &resourceTable.rpmsg_vring0, &CT_MBX.MESSAGE[MB_TO_ARM_HOST], &CT_MBX.MESSAGE[MB_FROM_ARM_HOST]);

  /* Initialize pru_virtqueue corresponding to vring1 (ARM Host to PRU direction) */
  pru_virtqueue_init(&transport.virtqueue1, &resourceTable.rpmsg_vring1, &CT_MBX.MESSAGE[MB_TO_ARM_HOST], &CT_MBX.MESSAGE[MB_FROM_ARM_HOST]);

  /* Create the RPMsg channel between the PRU and ARM user space using the transport structure. */
  while (pru_rpmsg_channel(RPMSG_NS_CREATE, &transport, CHAN_NAME, CHAN_DESC, CHAN_PORT) != PRU_RPMSG_SUCCESS);

  timer_start();

  while (1) {
    timer_poll();
    now = timer_get_ticks();

    /* Check bit 30 of register R31 to see if the mailbox interrupt has occurred */
    if (__R31 & HOST_INT) {
      /* Clear the mailbox interrupt */
      CT_MBX.IRQ[MB_USER].STATUS_CLR |= 1 << (MB_FROM_ARM_HOST * 2);

      /* Clear the event status, event MB_INT_NUMBER corresponds to the mailbox interrupt */
      CT_INTC.SICR_bit.STS_CLR_IDX = MB_INT_NUMBER;

      /* Use a while loop to read all of the current messages in the mailbox */
      while (CT_MBX.MSGSTATUS_bit[MB_FROM_ARM_HOST].NBOFMSG > 0) {
        /* Check to see if the message corresponds to a receive event for the PRU */
        if (CT_MBX.MESSAGE[MB_FROM_ARM_HOST] == 1) {
          /* Receive the message */
          if (pru_rpmsg_receive(&transport, &src, &dst, payload, &len) == PRU_RPMSG_SUCCESS) {
            uint32_t stripped_len = len;
            rpmsg_connected = true;
            bool skip_echo = false;
            int rc;
            if (len < RPMSG_BUF_SIZE) {
              uint32_t pos = len;
              payload[pos] = '\0';
              if (pos > 0 ) {
                pos -= 1;
                if (payload[pos] == '\r') {
                  stripped_len = len - 1;
                }
              }
            }
            rc = strncmp((char*)payload, "connect", stripped_len);
            if (rc == 0) {
              post_event(motor_stop, 11);
              post_event(valve1_off, 11);
              post_event(valve2_off, 11);
              post_event(valve3_off, 11);
              post_event(mode_normal, 12);
              post_event(sort_order_blue_red_white, 26);
              post_event(controller_stopped, 19);
              if (is_conveyor_running) {
                post_event(conveyor_running, 17);
              }
              else {
                post_event(conveyor_stopped, 17);
              }
              if (get_last_input(LIGHTBARRIER1_MASK)) {
                post_event(lightbarrier1_on, 17);
              }
              else {
                post_event(lightbarrier1_off, 18);
              }
              if (get_last_input(LIGHTBARRIER2_MASK)) {
                post_event(lightbarrier2_on, 17);
              }
              else {
                post_event(lightbarrier2_off, 18);
              }
              if (get_last_input(LIGHTBARRIERS3_TO_5_MASK)) {
                post_event(emergency_stop_on, 18);
              }
              else {
                post_event(emergency_stop_off, 19);
              }
            }
            rc = strncmp((char*)payload, "disconnect", stripped_len);
            if (rc == 0) {
              __R30 &= ~MOTOR_MASK;
              __R30 &= ~VALVE1_MASK;
              __R30 &= ~VALVE2_MASK;
              __R30 &= ~VALVE3_MASK;
              mode = MODE_NORMAL;
              is_controller_started = false;
              rpmsg_connected = false;
            }
            if (mode == MODE_DIAGNOSTIC) {
              rc = strncmp((char*)payload, "mode=normal", stripped_len);
              if (rc == 0) {
                __R30 &= ~MOTOR_MASK;
                post_event(motor_stop, 11);
                __R30 &= ~VALVE1_MASK;
                post_event(valve1_off, 11);
                __R30 &= ~VALVE2_MASK;
                post_event(valve2_off, 11);
                __R30 &= ~VALVE3_MASK;
                post_event(valve3_off, 11);
                is_controller_started = false;
                post_event(controller_stopped, 19);
                mode = MODE_NORMAL;
              }
            }
            if (mode == MODE_NORMAL) {
              rc = strncmp((char*)payload, "mode=diagnostic", stripped_len);
              if (rc == 0) {
                if (is_controller_started) {
                  __R30 &= ~MOTOR_MASK;
                  post_event(motor_stop, 11);
                  is_controller_started = false;
                  post_event(controller_stopped, 19);
                }
                mode = MODE_DIAGNOSTIC;
              }
            }
            if (mode == MODE_NORMAL && !is_controller_started) {
              rc = strncmp((char*)payload, "start", stripped_len);
              if (rc == 0) {
                if (get_last_input(LIGHTBARRIERS3_TO_5_MASK)) {
                  skip_echo = true;
                  post_event("log: 'emergency-stop=on' prevented start\n", 41);
                }
                else {
                  __R30 |= MOTOR_MASK;
                  post_event(motor_start, 12);
                  is_controller_started = true;
                  post_event(controller_started, 19);
                }
              }
            }
            if (is_controller_started) {
              rc = strncmp((char*)payload, "stop", stripped_len);
              if (rc == 0) {
                __R30 &= ~MOTOR_MASK;
                post_event(motor_stop, 11);
                is_controller_started = false;
                post_event(controller_stopped, 19);
              }
            }
            if (mode == MODE_DIAGNOSTIC) {
              rc = strncmp((char*)payload, "motor=start", stripped_len);
              if (rc == 0) {
                __R30 |= MOTOR_MASK;
              }
              rc = strncmp((char*)payload, "motor=stop", stripped_len);
              if (rc == 0) {
                __R30 &= ~MOTOR_MASK;
              }
              rc = strncmp((char*)payload, "valve1=on", stripped_len);
              if (rc == 0) {
                __R30 |= VALVE1_MASK;
              }
              rc = strncmp((char*)payload, "valve1=off", stripped_len);
              if (rc == 0) {
                __R30 &= ~VALVE1_MASK;
              }
              rc = strncmp((char*)payload, "valve2=on", stripped_len);
              if (rc == 0) {
                __R30 |= VALVE2_MASK;
              }
              rc = strncmp((char*)payload, "valve2=off", stripped_len);
              if (rc == 0) {
                __R30 &= ~VALVE2_MASK;
              }
              rc = strncmp((char*)payload, "valve3=on", stripped_len);
              if (rc == 0) {
                __R30 |= VALVE3_MASK;
              }
              rc = strncmp((char*)payload, "valve3=off", stripped_len);
              if (rc == 0) {
                __R30 &= ~VALVE3_MASK;
              }
            }

            if (!skip_echo) {
              /* Echo the message back to the same address from which we just received */
              pru_rpmsg_send(&transport, dst, src, payload, len);
            }
          }
        }
      }
    }
    process_inputs(__R31);
    check_scheduled_pusher_actions();
    check_scheduled_adc_actions();
  }
}
