#include "controller_state_normal.h"
#include "controller.h"
#include "msg_definition.h"
#include "motor.h"
#include "controller_state_diagnostic.h"
#include <stdint.h>
#include "controller_state_normal_state_stopped.h"
#include "hw.h"

int16_t post_info(char info);

bool ControllerStateNormal::lb2BrickUnhandled = true;

ControllerStateNormal::ControllerStateNormal(Hw &hw, Timer &timer,
                                             ControllerStateNormalStateStarted &state_started,
                                             ControllerStateNormalStateStopped &state_stopped):
                                                ControllerState(hw, timer),
                                                state_started(state_started),
                                                state_stopped(state_stopped)

{
    pState = &state_stopped;
}

void ControllerStateNormal::processCmd(Controller &controller, uint8_t cmd)
{
    pState->processCmd(cmd);

    switch(cmd)
    {
        case CMD_MODE_DIAGNOSTIC:
            hw.motor.stop();
            post_info(INFO_CTRL_STOP);
            controller.setState(&controller.state_diagnostic);
            break;
    }
}


void ControllerStateNormal::setState(ControllerStateNormalState *pNewState, Hw &hw)
{
    pState = pNewState;
    pState->onEntry();
}

ControllerStateNormalState* ControllerStateNormal::getState()
{
    return pState;
}

void ControllerStateNormal::onEntry()
{
    setState(&state_stopped, hw);
}

void ControllerStateNormal::doIt()
{
    pState->doIt();
}
