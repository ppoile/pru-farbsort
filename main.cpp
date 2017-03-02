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
}

#include "common.h"
#include "hw.h"
#include "adc.h"
#include "scheduled_output_action.h"
#include "timer.h"
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
#include "controller_state_normal_started.h"
#include "controller_state_normal_stopped.h"
#include "rpmsg.h"
#include "rpmsg_rx_interface.h"
#include "rpmsg_tx_interface.h"
#include "command_interface.h"
#include "conveyor_belt_observer.h"
#include "pulse_counter.h"

#ifdef ADC_LOGGING
uint8_t adc_values[200];
uint8_t adc_value_index = 0;
#endif


// init communication to host
RpMsgTrx rpmsg;

// init hw components
Motor motor(Gpio::MOTOR_MASK);

Piston piston[] = { Piston(1, Gpio::VALVE1_MASK, &rpmsg),
                    Piston(2, Gpio::VALVE2_MASK, &rpmsg),
                    Piston(3, Gpio::VALVE3_MASK, &rpmsg) };

LightBarrier lightBarrier[] = { LightBarrier(1, Gpio::LIGHTBARRIER1_MASK, &rpmsg),
                                LightBarrier(2, Gpio::LIGHTBARRIER2_MASK, &rpmsg),
                                LightBarrier(3, Gpio::LIGHTBARRIERS3_TO_5_MASK, &rpmsg) };
Adc adc;
PulseCounter pulseCounter(Gpio::PULSECOUNTER_MASK);

Hw hw(  &motor,
        &piston[0],
        &piston[1],
        &piston[2],
        &lightBarrier[0],
        &lightBarrier[1],
        &lightBarrier[2],
        &adc);

Timer timer;

Queue<Color,COLOR_QUEUE_SIZE> colorQueue;
ColorDetect colorDetect(hw, &timer, colorQueue, &rpmsg);
ObjectPool<BrickEjectCommand, 5> ejectCommandPool;

ConveyorBeltObserver conveyorBeltObserver(&pulseCounter, &timer, &rpmsg);

// init controller states
ControllerStateDiagnostic stateDiagnostic(hw, &timer, &rpmsg);
ControllerStateNormalStarted state_started(hw, &timer, &rpmsg, colorQueue, ejectCommandPool);
ControllerStateNormalStopped state_stopped(hw, &timer, &rpmsg);

// init controller
Controller ctrl(hw, &rpmsg, stateDiagnostic, state_stopped, state_started);


void main() {

#ifdef ADC_LOGGING
    for(int i = 0; i < 200; i++)
    {
      adc_values[i] = 0xaa;
    }
#endif

    /* Allow OCP master port access by the PRU so the PRU can read external memories */
    CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

    rpmsg.start(); 
    timer.start();

    // calling the execute() will start the infinite calling of theses commands
    colorDetect.execute();
    conveyorBeltObserver.execute();

    while (1) {
        lightBarrier[0].poll();
        lightBarrier[1].poll();
        timer.poll();
        pulseCounter.poll();
        ctrl.doIt();
        rpmsg.processMessages();
    }
}
