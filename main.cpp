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
#include "command_interface.h"
#include "color_detect.h"
#include "brick_eject_command.h"
#include "controller_state_diagnostic.h"
#include "controller_state_normal.h"

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
Piston piston[] = { Piston(VALVE1_MASK), Piston(VALVE2_MASK), Piston(VALVE3_MASK) };
LightBarrier lightBarrier[] = { LightBarrier(LIGHTBARRIER1_MASK), LightBarrier(LIGHTBARRIER2_MASK), LightBarrier(LIGHTBARRIERS3_TO_5_MASK) };
Adc adc;

Hw hw(motor,
      //compressor
        piston[0],
        piston[1],
        piston[2],
        lightBarrier[0],
        lightBarrier[1],
        lightBarrier[2],
        adc);



Timer timer;

Queue<Color,COLOR_QUEUE_SIZE> colorQueue;
ColorDetect colorDetect(hw, timer, colorQueue);
ObjectPool<BrickEjectCommand, 5> ejectCommandPool;

ControllerStateDiagnostic stateDiagnostic(hw, timer);
ControllerStateNormalStateStarted state_started(hw, timer, colorQueue, colorDetect, ejectCommandPool);
ControllerStateNormalStateStopped state_stopped(hw, timer);
ControllerStateNormal stateNormal(hw, timer, state_started, state_stopped);

Controller ctrl(hw, stateDiagnostic, stateNormal);


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




void main() {
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

    timer.start();
    timer.schedule(&colorDetect,100);


    while (1) {
        timer.poll();
        ctrl.doIt();
        checkArmToPruMsg();
    }
}
