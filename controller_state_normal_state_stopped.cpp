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

ControllerStateNormalStateStopped::ControllerStateNormalStateStopped()
{

}

void ControllerStateNormalStateStopped::onEntry(Hw &hw)
{
    hw.motor.stop();
    hw.piston0.pull();
    hw.piston1.pull();
    hw.piston2.pull();
}

void ControllerStateNormalStateStopped::processCmd(Hw &hw, uint8_t cmd)
{

    switch(cmd)
    {
        case CMD_START:
            pSuperState->setState(&pSuperState->state_started, hw);
            break;


    }
}

