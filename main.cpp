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

#include "scheduled_output_action.h"
#include "timer.h"

#include "DirectMemory.h"
#include "TiAdc.h"
#include "AdcMeasurement.h"


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

static const uint32_t PULSECOUNTER_MASK = 0x8000;
static const uint32_t LIGHTBARRIER1_MASK = 0x4000;
static const uint32_t LIGHTBARRIER2_MASK = 0x10000;
static const uint32_t LIGHTBARRIERS3_TO_5_MASK = 0x4;

static const uint32_t ADC_LIMIT_TOLERANCE = 60;
static const uint32_t ADC_NO_OBJECT_LIMIT = 0x4be;
static const uint32_t ADC_BLUE_OBJECT_LIMIT = 0x49a;
static const uint32_t ADC_RED_OBJECT_LIMIT = 0x343;
static const uint32_t ADC_WHITE_OBJECT_LIMIT = 0x307;

enum Mode { STOPPED, RUNNING, DIAGNOSTIC };

Mode mode;
bool rpmsg_connected;
uint32_t pulsecounter_last_change;
bool conveyor_running;
struct pru_rpmsg_transport transport;
uint16_t src, dst, len;
uint32_t now;
uint32_t last_all_inputs_value;
static uint32_t * const AdcBaseAddr = (uint32_t*)0x44E0D000;
DirectMemory adcMemory(AdcBaseAddr);
TiAdc adc(adcMemory);
AdcMeasurement measurement(adc);
uint32_t adc_last_measurement;
uint32_t adc_min_value;
uint32_t lightbarrier1_last_change;
uint32_t lightbarrier2_last_change;
uint32_t lightbarrier3_to_5_last_change;


int16_t post_event(void *event, uint16_t length)
{
  if (!rpmsg_connected) {
    return -4; //RPMSG_NOT_CONNECTED
  }
  return pru_rpmsg_send(&transport, dst, src, event, length);
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
  char buffer[] = "pusherX=Y (now=0xXXXXXXXX)\n";
  if (next_action.bitmask == VALVE1_MASK) {
    buffer[6] = '1';
  }
  else if (next_action.bitmask == VALVE2_MASK) {
    buffer[6] = '2';
  }
  else if (next_action.bitmask == VALVE3_MASK) {
    buffer[6] = '3';
  }
  if (next_action.value) {
    buffer[8] = '1';
  }
  else {
    buffer[8] = '0';
  }
  appendNumber(&buffer[17], now);
  post_event((void*)buffer, 27);
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
  uint16_t value = measurement.read();
  adc_last_measurement = now;
  if (value < adc_min_value) {
    adc_min_value = value;
  }
  char buffer[] = "ADC=0xXXXXXXXX (now=0xXXXXXXXX)\n";
  appendNumber(&buffer[6], (uint32_t)value);
  appendNumber(&buffer[22], (uint32_t)now);
  post_event((void*)buffer, 32);
  if (now < next_action + 22) {
    return;
  }
  adc_actions.pop_front();
  char *color;
  uint32_t color_length;
  if (adc_min_value >= ADC_NO_OBJECT_LIMIT) {
    color = "?";
    color_length = 1;
  }
  else if ((adc_min_value <= ADC_BLUE_OBJECT_LIMIT + ADC_LIMIT_TOLERANCE) &&
           (adc_min_value >= ADC_BLUE_OBJECT_LIMIT)) {
    color = "blue";
    color_length = 4;
  }
  else if ((adc_min_value <= ADC_RED_OBJECT_LIMIT + ADC_LIMIT_TOLERANCE) &&
           (adc_min_value >= ADC_RED_OBJECT_LIMIT)) {
    color = "red";
    color_length = 3;
  }
  else if ((adc_min_value <= ADC_WHITE_OBJECT_LIMIT + ADC_LIMIT_TOLERANCE) &&
           (adc_min_value >= ADC_WHITE_OBJECT_LIMIT)) {
    color = "white";
    color_length = 5;
  }
  else {
    color = "?";
    color_length = 1;
  }
  strcpy(buffer, "color=");
  strcpy(&buffer[6], color);
  buffer[6 + color_length] = '\n';
  buffer[6 + color_length + 1] = '\0';
  post_event((void*)buffer, 6 + color_length + 1);
  adc_min_value = 0xFFFF;
}

void on_input_change(uint32_t mask, int value, int last_value)
{
  if (mask == LIGHTBARRIERS3_TO_5_MASK) {
    if (now - lightbarrier3_to_5_last_change < 5) {
      return;
    }
    lightbarrier3_to_5_last_change = now;
    if (mode==RUNNING) {
      if (value) {
        __R30 &= ~MOTOR_MASK;
        mode=STOPPED;
        static const char emergency_motor_stop[] = "mode=stopped (emergency-stop)\n";
        post_event((void*)emergency_motor_stop, 28);
      }
    }
    else {
      if (value) {
        static const char emergency_stop_on[] = "emergency-stop=on\n";
        post_event((void*)emergency_stop_on, 18);
      }
      else {
        static const char emergency_stop_off[] = "emergency-stop=off\n";
        post_event((void*)emergency_stop_off, 19);
      }
    }
  }
  if (mask == PULSECOUNTER_MASK) {
    pulsecounter_last_change = timer_get_ticks();
    if (!conveyor_running) {
      conveyor_running = true;
      static const char conveyor_running[] = "conveyor=running\n";
      post_event((void*)conveyor_running, 17);
    }
  }
  if (mask == LIGHTBARRIER1_MASK) {
    if (now - lightbarrier1_last_change < 5) {
      return;
    }
    lightbarrier1_last_change = now;
    if (value) {
      char buffer[] = "lightbarrier1=on (now=0xXXXXXXXX)\n";
      appendNumber(&buffer[24], now);
      post_event((void*)buffer, 34);
    }
    else {
      char buffer[] = "lightbarrier1=off (now=0xXXXXXXXX)\n";
      appendNumber(&buffer[25], now);
      post_event((void*)buffer, 35);
      schedule_adc_action(now + 111);
    }
  }
  if (mask == LIGHTBARRIER2_MASK) {
    if (now - lightbarrier2_last_change < 5) {
      return;
    }
    lightbarrier2_last_change = now;
    if (value) {
      char buffer[] = "lightbarrier2=on (now=0xXXXXXXXX)\n";
      appendNumber(&buffer[24], now);
      post_event((void*)buffer, 34);
      if (mode == RUNNING) {
        schedule_pusher_action(now + 69, VALVE1_MASK, true);
        schedule_pusher_action(now + 99, VALVE1_MASK, false);
        schedule_pusher_action(now + 166, VALVE2_MASK, true);
        schedule_pusher_action(now + 196, VALVE2_MASK, false);
        schedule_pusher_action(now + 274, VALVE3_MASK, true);
        schedule_pusher_action(now + 304, VALVE3_MASK, false);
      }
    }
    else {
      char buffer[] = "lightbarrier2=off (now=0xXXXXXXXX)\n";
      appendNumber(&buffer[25], now);
      post_event((void*)buffer, 35);
    }
  }
}

uint32_t get_input(uint32_t all_inputs_value, uint32_t mask)
{
  bool value = !(!(all_inputs_value & mask));
  bool last_value = !(!(last_all_inputs_value & mask));
  if (value != last_value) {
    on_input_change(mask, value, last_value);
  }
  return value;
}

void process_inputs(uint32_t all_inputs_value)
{
  get_input(all_inputs_value, LIGHTBARRIERS3_TO_5_MASK);
  get_input(all_inputs_value, PULSECOUNTER_MASK);
  get_input(all_inputs_value, LIGHTBARRIER1_MASK);
  get_input(all_inputs_value, LIGHTBARRIER2_MASK);

  if (conveyor_running) {
    int32_t ticks_since_last_change = now - pulsecounter_last_change;
    if (ticks_since_last_change > 100) {
      conveyor_running = false;
      static const char conveyor_stopped[] = "conveyor=stopped\n";
      post_event((void*)conveyor_stopped, 17);
    }
  }

  last_all_inputs_value = all_inputs_value;
}

void main() {
  mode = DIAGNOSTIC;
  rpmsg_connected = false;
  pulsecounter_last_change = 0;
  conveyor_running = false;
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
  lightbarrier3_to_5_last_change = 0;

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
            rpmsg_connected = true;
            int rc;
            if (len < RPMSG_BUF_SIZE) {
              payload[len] = '\0';
            }
            rc = strncmp((char*)payload, "connect\r", len);
            if (rc == 0) {
            }
            rc = strncmp((char*)payload, "disconnect\r", len);
            if (rc == 0) {
              __R30 &= ~MOTOR_MASK;
              __R30 &= ~VALVE1_MASK;
              __R30 &= ~VALVE2_MASK;
              __R30 &= ~VALVE3_MASK;
              rpmsg_connected = false;
            }
            rc = strncmp((char*)payload, "mode=stopped\r", len);
            if (rc == 0) {
              mode = STOPPED;
            }
            rc = strncmp((char*)payload, "mode=diagnostic\r", len);
            if (rc == 0) {
              mode = DIAGNOSTIC;
            }
            if (mode == STOPPED) {
              rc = strncmp((char*)payload, "start\r", len);
              if (rc == 0) {
                __R30 |= MOTOR_MASK;
                mode = RUNNING;
              }
            }
            if (mode == RUNNING) {
              rc = strncmp((char*)payload, "stop\r", len);
              if (rc == 0) {
                __R30 &= ~MOTOR_MASK;
                mode = STOPPED;
              }
            }
            if (mode == DIAGNOSTIC) {
              rc = strncmp((char*)payload, "motor=start\r", len);
              if (rc == 0) {
                __R30 |= MOTOR_MASK;
              }
              rc = strncmp((char*)payload, "motor=stop\r", len);
              if (rc == 0) {
                __R30 &= ~MOTOR_MASK;
              }
              rc = strncmp((char*)payload, "valve1=on\r", len);
              if (rc == 0) {
                __R30 |= VALVE1_MASK;
              }
              rc = strncmp((char*)payload, "valve1=off\r", len);
              if (rc == 0) {
                __R30 &= ~VALVE1_MASK;
              }
              rc = strncmp((char*)payload, "valve2=on\r", len);
              if (rc == 0) {
                __R30 |= VALVE2_MASK;
              }
              rc = strncmp((char*)payload, "valve2=off\r", len);
              if (rc == 0) {
                __R30 &= ~VALVE2_MASK;
              }
              rc = strncmp((char*)payload, "valve3=on\r", len);
              if (rc == 0) {
                __R30 |= VALVE3_MASK;
              }
              rc = strncmp((char*)payload, "valve3=off\r", len);
              if (rc == 0) {
                __R30 &= ~VALVE3_MASK;
              }
            }

            /* Echo the message back to the same address from which we just received */
            pru_rpmsg_send(&transport, dst, src, payload, len);
          }
        }
      }
    }
    process_inputs(__R31);
    check_scheduled_pusher_actions();
    check_scheduled_adc_actions();
  }
}
