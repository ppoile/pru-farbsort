#ifndef CONTROLLERSTATENORMALSTATESTARTED_H
#define CONTROLLERSTATENORMALSTATESTARTED_H

#include "controller_state_normal_state.h"

struct Hw;

class ControllerStateNormalStateStarted : public ControllerStateNormalState
{
public:
    ControllerStateNormalStateStarted();

    void processCmd(Hw &hw, uint8_t cmd);
    void onEntry(Hw &hw);
};

#endif // CONTROLLERSTATENORMALSTATESTARTED_H
