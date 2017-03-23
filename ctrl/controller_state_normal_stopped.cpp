#include "controller_state_normal_stopped.h"
#include "msg_definition.h"
#include "controller_state_normal_started.h"
#include "motor.h"
#include "controller.h"
#include "hw.h"
#include "piston.h"

int16_t post_info(char info);

ControllerStateNormalStopped::ControllerStateNormalStopped(Hw &hw, TimerInterface *timer, RpMsgTxInterface *rpmsg)
    : ControllerState(hw, timer, rpmsg)
{

}

void ControllerStateNormalStopped::onEntry()
{
    hw.motor->stop();
    hw.piston0->pull();
    hw.piston1->pull();
    hw.piston2->pull();
}

void ControllerStateNormalStopped::processCmd(Controller &controller, uint8_t cmd)
{

    switch(cmd)
    {
        case CMD_START:
            controller.setState(&controller.state_normal_started);
            break;

        case CMD_MODE_DIAGNOSTIC:
            controller.setState(&controller.state_diagnostic);
            break;


    }
}

