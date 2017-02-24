#ifndef CONTROLLERSTATENORMALSTATE_H
#define CONTROLLERSTATENORMALSTATE_H

#include <stdint.h>


struct Hw;
class TimerInterface;
class RpMsgTxInterface;

class ControllerStateNormalState
{
public:
    ControllerStateNormalState(Hw &hw, TimerInterface *timer, RpMsgTxInterface *rpmsg);

    virtual void processCmd(uint8_t cmd) = 0;
    virtual void onEntry(){};
    virtual void onExit(){};
    virtual void doIt(){};

protected:
    class ControllerStateNormal *pSuperState;
    Hw &hw;
    TimerInterface *timer;
    RpMsgTxInterface *rpmsg;
};

#endif // CONTROLLERSTATENORMALSTATE_H
