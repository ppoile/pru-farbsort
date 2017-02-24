#include "controller_state_normal_state_stopped.h"
#include "controller_state_normal_state.h"
#include "controller_state_normal.h"
#include "msg_definition.h"
#include "controller_state_normal_state_started.h"
#include "motor.h"
#include "controller.h"
#include "hw.h"
#include "piston.h"

int16_t post_info(char info);

ControllerStateNormalStateStopped::ControllerStateNormalStateStopped(Hw &hw, TimerInterface *timer, RpMsgTxInterface *rpmsg)
    : ControllerStateNormalState(hw, timer, rpmsg)
{

}

void ControllerStateNormalStateStopped::onEntry()
{
    hw.motor->stop();
    hw.piston0->pull();
    hw.piston1->pull();
    hw.piston2->pull();
}

void ControllerStateNormalStateStopped::processCmd(uint8_t cmd)
{

    switch(cmd)
    {
        case CMD_START:
            pSuperState->setState(&pSuperState->state_started);
            break;


    }
}

