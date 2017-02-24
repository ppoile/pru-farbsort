#include <stdint.h>
#include "controller_state_diagnostic.h"
#include "controller.h"
#include "controller_state_normal.h"
#include "motor.h"
#include "msg_definition.h"
#include "piston.h"
#include "hw.h"
#include "timer_interface.h"
#include "rpmsg_tx_interface.h"


ControllerStateDiagnostic::ControllerStateDiagnostic(Hw &hw, TimerInterface *timer, RpMsgTxInterface *rpmsg):ControllerState(hw, timer, rpmsg)
{
}


void ControllerStateDiagnostic::processCmd(Controller &controller, uint8_t cmd)
{

    switch(cmd)
    {
        case CMD_MODE_NORMAL:
            hw.motor->stop();
            hw.piston0->pull();
            hw.piston1->pull();
            hw.piston2->pull();

            rpmsg->post_info(INFO_CTRL_STOP);

            controller.setState(&controller.state_normal);
            break;

        case CMD_MOTOR_START:
            hw.motor->start();
            break;

        case CMD_MOTOR_STOP:
            hw.motor->stop();
            break;

        case CMD_VALVE_1_ON:
            hw.piston0->push();
            break;

        case CMD_VALVE_1_OFF:
            hw.piston0->pull();
            break;

        case CMD_VALVE_2_ON:
            hw.piston1->push();
            break;

        case CMD_VALVE_2_OFF:
            hw.piston1->pull();
            break;

        case CMD_VALVE_3_ON:
            hw.piston2->push();
            break;

        case CMD_VALVE_3_OFF:
            hw.piston2->pull();
            break;

    }

}
