#ifndef CONTROLLERSTATENORMAL_H
#define CONTROLLERSTATENORMAL_H

#include "controller_state.h"
#include "controller_state_normal_state.h"
#include "controller_state_normal_state_started.h"
#include "controller_state_normal_state_stopped.h"
#include "timer.h"

struct Hw;


class ControllerStateNormal: public ControllerState
{
public:
    ControllerStateNormal(Hw &hw, Timer &timer,
                          ControllerStateNormalStateStarted &state_started,
                          ControllerStateNormalStateStopped &state_stopped);

        void setState(ControllerStateNormalState *pNewState, Hw &hw);
    ControllerStateNormalState*getState();

    void processCmd(Controller &controller, uint8_t cmd);
    void onEntry();
    void doIt();

    ControllerStateNormalStateStarted &state_started;
    ControllerStateNormalStateStopped &state_stopped;

private:
    ControllerStateNormalState* pState;
    static bool lb2BrickUnhandled;
};

#endif // CONTROLLERSTATENORMAL_H
