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
//#include "resource_table_0.h"
}

#include "common.h"
#include "hw.h"
#include "adc.h"
#include "scheduled_output_action.h"
#include "timer.h"
#include "util.h"
#include "msg_definition.h"
#include "light_barrier.h"
#include "controller.h"
#include "version.h"
#include "motor.h"
#include "piston.h"
#include "queue.h"
#include "command_interface.h"
#include "color_detect.h"
#include "brick_eject_command.h"
#include "controller_state_diagnostic.h"
#include "controller_state_normal.h"
#include "rpmsg.h"
#include "rpmsg_rx_interface.h"
#include "rpmsg_tx_interface.h"
#include "command_interface.h"


volatile register uint32_t __R30;
volatile register uint32_t __R31;




uint8_t adc_values[200];
uint8_t adc_value_index = 0;


enum Mode { MODE_NORMAL, MODE_DIAGNOSTIC };

enum SortOrder { BLUE_RED_WHITE };


bool is_controller_started;

uint32_t pulsecounter_last_change;
bool is_conveyor_running;


uint32_t now;
uint32_t last_all_inputs_value;

uint32_t adc_last_measurement;
uint32_t adc_min_value;
uint32_t lightbarrier1_last_change;
uint32_t lightbarrier2_last_change;
uint32_t lightbarriers3_to_5_last_change;


Motor motor(Gpio::MOTOR_MASK);
Piston piston[] = { Piston(Gpio::VALVE1_MASK), Piston(Gpio::VALVE2_MASK), Piston(Gpio::VALVE3_MASK) };
LightBarrier lightBarrier[] = { LightBarrier(Gpio::LIGHTBARRIER1_MASK), LightBarrier(Gpio::LIGHTBARRIER2_MASK), LightBarrier(Gpio::LIGHTBARRIERS3_TO_5_MASK) };
Adc adc;

Hw hw(&motor,
      //compressor
        &piston[0],
        &piston[1],
        &piston[2],
        &lightBarrier[0],
        &lightBarrier[1],
        &lightBarrier[2],
        &adc);



Timer timer;

Queue<Color,COLOR_QUEUE_SIZE> colorQueue;
ColorDetect colorDetect(hw, &timer, colorQueue);
ObjectPool<BrickEjectCommand, 5> ejectCommandPool;


RpMsgTrx rpmsg;

ControllerStateDiagnostic stateDiagnostic(hw, &timer, &rpmsg);
ControllerStateNormalStateStarted state_started(hw, &timer, &rpmsg, colorQueue, &colorDetect, ejectCommandPool);
ControllerStateNormalStateStopped state_stopped(hw, &timer, &rpmsg);
ControllerStateNormal stateNormal(hw, &timer, &rpmsg, state_started, state_stopped);
Controller ctrl(hw, &rpmsg, stateDiagnostic, stateNormal);


void main() {

    /* Allow OCP master port access by the PRU so the PRU can read external memories */
    CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;


    rpmsg.start();

    for(int i = 0; i < 200; i++)
    {
      adc_values[i] = 0xaa;
    }

    timer.start();

    while (1) {
        timer.poll();
        ctrl.doIt();
        rpmsg.processMessages();
    }
}
