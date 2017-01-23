#ifndef CONTROLLERSTATENORMALSTATE_H
#define CONTROLLERSTATENORMALSTATE_H

#include <stdint.h>

struct Hw;

class ControllerStateNormalState
{
public:
    ControllerStateNormalState();

    virtual void processCmd(Hw &hw, uint8_t cmd) = 0;
    virtual void onEntry(Hw &hw){};

protected:
    class ControllerStateNormal *pSuperState;
};

#endif // CONTROLLERSTATENORMALSTATE_H
