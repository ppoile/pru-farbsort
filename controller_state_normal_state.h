#ifndef CONTROLLERSTATENORMALSTATE_H
#define CONTROLLERSTATENORMALSTATE_H

#include <stdint.h>
#include "timer.h"

struct Hw;

class ControllerStateNormalState
{
public:
    ControllerStateNormalState(Hw &hw, Timer &timer);

    virtual void processCmd(uint8_t cmd) = 0;
    virtual void onEntry(){};
    virtual void doIt(){};

protected:
    class ControllerStateNormal *pSuperState;
    Hw &hw;
    Timer &timer;
};

#endif // CONTROLLERSTATENORMALSTATE_H
