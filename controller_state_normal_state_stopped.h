#ifndef CONTROLLERSTATENORMALSTATESTOPPED_H
#define CONTROLLERSTATENORMALSTATESTOPPED_H

#include "controller_state_normal_state.h"

struct Hw;

class ControllerStateNormalStateStopped : public ControllerStateNormalState
{
public:
    ControllerStateNormalStateStopped();
    void processCmd(Hw &hw, uint8_t cmd);
    void onEntry(Hw &hw);
};

#endif // CONTROLLERSTATENORMALSTATESTOPPED_H
