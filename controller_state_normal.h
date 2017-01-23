#ifndef CONTROLLERSTATENORMAL_H
#define CONTROLLERSTATENORMAL_H

#include "controller_state.h"
#include "controller_state_normal_state.h"
#include "controller_state_normal_state_started.h"
#include "controller_state_normal_state_stopped.h"

struct Hw;

class ControllerStateNormal: public ControllerState
{
public:
    ControllerStateNormal();

    void processCmd(Hw &hw, Controller &controller, uint8_t cmd);
    void setState(ControllerStateNormalState *pNewState, Hw &hw);
    ControllerStateNormalState*getState();
    void onEntry(Hw &hw);

    ControllerStateNormalStateStarted state_started;
    ControllerStateNormalStateStopped state_stopped;

private:
    ControllerStateNormalState* pState;
};

#endif // CONTROLLERSTATENORMAL_H
