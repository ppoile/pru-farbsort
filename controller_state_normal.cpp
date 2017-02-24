#include "controller_state_normal.h"
#include "controller.h"
#include "msg_definition.h"
#include "motor.h"
#include "controller_state_diagnostic.h"
#include <stdint.h>
#include "controller_state_normal_state_stopped.h"
#include "hw.h"
#include "rpmsg_tx_interface.h"


bool ControllerStateNormal::lb2BrickUnhandled = true;

ControllerStateNormal::ControllerStateNormal(Hw &hw, TimerInterface *timer, RpMsgTxInterface *rpmsg,
                                             ControllerStateNormalStateStarted &state_started,
                                             ControllerStateNormalStateStopped &state_stopped):
                                                ControllerState(hw, timer, rpmsg),
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
            controller.setState(&controller.state_diagnostic);
            break;
    }
}


void ControllerStateNormal::setState(ControllerStateNormalState *pNewState)
{
    pState->onExit();
    pState = pNewState;
    pState->onEntry();
}

ControllerStateNormalState* ControllerStateNormal::getState()
{
    return pState;
}

void ControllerStateNormal::onEntry()
{
    setState(&state_stopped);
}

void ControllerStateNormal::onExit()
{
    rpmsg->post_info(INFO_CTRL_STOP);
    setState(&state_stopped);
}

void ControllerStateNormal::doIt()
{
    pState->doIt();
}
