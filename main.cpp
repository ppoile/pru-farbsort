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


volatile register uint32_t __R30;
volatile register uint32_t __R31;

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
//static const uint32_t LIGHTBARRIERS3_TO_5_MASK = 0x4;

struct pru_rpmsg_transport transport;
uint16_t src=0xFFFF, dst=0xFFFF, len=0xFFFF;


void on_input_change(uint32_t mask, int value, int last_value)
{
  if (src == 0xFFFF) {
    return;
  }

  if (mask == PULSECOUNTER_MASK) {
    if (value) {
      static const char pulsecounter_on[] = "pulsecounter=on\r";
      pru_rpmsg_send(&transport, dst, src, (void*)pulsecounter_on, 16);
    }
    else {
      static const char pulsecounter_off[] = "pulsecounter=off\r";
      pru_rpmsg_send(&transport, dst, src, (void*)pulsecounter_off, 17);
    }
  }
  if (mask == LIGHTBARRIER1_MASK) {
    if (value) {
      static const char lightbarrier1_on[] = "lightbarrier1=on\r";
      pru_rpmsg_send(&transport, dst, src, (void*)lightbarrier1_on, 17);
    }
    else {
      static const char lightbarrier1_off[] = "lightbarrier1=off\r";
      pru_rpmsg_send(&transport, dst, src, (void*)lightbarrier1_off, 18);
    }
  }
  if (mask == LIGHTBARRIER2_MASK) {
    if (value) {
      static const char lightbarrier2_on[] = "lightbarrier2=on\r";
      pru_rpmsg_send(&transport, dst, src, (void*)lightbarrier2_on, 17);
    }
    else {
      static const char lightbarrier2_off[] = "lightbarrier2=off\r";
      pru_rpmsg_send(&transport, dst, src, (void*)lightbarrier2_off, 18);
    }
  }
}

static uint32_t last_all_inputs_value;

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
  get_input(all_inputs_value, PULSECOUNTER_MASK);
  get_input(all_inputs_value, LIGHTBARRIER1_MASK);
  get_input(all_inputs_value, LIGHTBARRIER2_MASK);

  last_all_inputs_value = all_inputs_value;
}


void main() {
	volatile uint8_t *status;

        last_all_inputs_value = ~__R31;
	__R30 = 0;

	/* allow OCP master port access by the PRU so the PRU can read external memories */
	CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

        //timer_start();

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
	while(pru_rpmsg_channel(RPMSG_NS_CREATE, &transport, CHAN_NAME, CHAN_DESC, CHAN_PORT) != PRU_RPMSG_SUCCESS);
	while(1){
		/* Check bit 30 of register R31 to see if the mailbox interrupt has occurred */
		if(__R31 & HOST_INT){
			/* Clear the mailbox interrupt */
			CT_MBX.IRQ[MB_USER].STATUS_CLR |= 1 << (MB_FROM_ARM_HOST * 2);
			/* Clear the event status, event MB_INT_NUMBER corresponds to the mailbox interrupt */
			CT_INTC.SICR_bit.STS_CLR_IDX = MB_INT_NUMBER;
			/* Use a while loop to read all of the current messages in the mailbox */
			while(CT_MBX.MSGSTATUS_bit[MB_FROM_ARM_HOST].NBOFMSG > 0){
				/* Check to see if the message corresponds to a receive event for the PRU */
				if(CT_MBX.MESSAGE[MB_FROM_ARM_HOST] == 1){
					/* Receive the message */
					if(pru_rpmsg_receive(&transport, &src, &dst, payload, &len) == PRU_RPMSG_SUCCESS){
						int rc;
						if(len < RPMSG_BUF_SIZE){
							payload[len] = '\0';
						}
						rc = strncmp((char*)payload, "motor=start\r", len);
						if(rc == 0){
							__R30 |= MOTOR_MASK;
						}
						rc = strncmp((char*)payload, "motor=stop\r", len);
						if(rc == 0){
							__R30 &= ~MOTOR_MASK;
						}
						rc = strncmp((char*)payload, "valve1=on\r", len);
						if(rc == 0){
							__R30 |= VALVE1_MASK;
						}
						rc = strncmp((char*)payload, "valve1=off\r", len);
						if(rc == 0){
							__R30 &= ~VALVE1_MASK;
						}
						rc = strncmp((char*)payload, "valve2=on\r", len);
						if(rc == 0){
							__R30 |= VALVE2_MASK;
						}
						rc = strncmp((char*)payload, "valve2=off\r", len);
						if(rc == 0){
							__R30 &= ~VALVE2_MASK;
						}
						rc = strncmp((char*)payload, "valve3=on\r", len);
						if(rc == 0){
							__R30 |= VALVE3_MASK;
						}
						rc = strncmp((char*)payload, "valve3=off\r", len);
						if(rc == 0){
							__R30 &= ~VALVE3_MASK;
						}

						/* Echo the message back to the same address from which we just received */
						pru_rpmsg_send(&transport, dst, src, payload, len);
					}
				}
			}
		}
		process_inputs(__R31);
	}
}
