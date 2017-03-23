#ifndef CONTROLLERSTATE_H
#define CONTROLLERSTATE_H

#include <stdint.h>

struct Hw;
class Controller;
class TimerInterface;
class RpMsgTxInterface;

class ControllerState
{
public:
    ControllerState(Hw &hw, TimerInterface *timer, RpMsgTxInterface *rpmsg)
        :hw(hw), timer(timer), rpmsg(rpmsg)
    {

    }

    virtual void processCmd(Controller &controller, uint8_t cmd) = 0;
    virtual void onEntry(){};
    virtual void doIt(){};

protected:
    Hw &hw;
    TimerInterface *timer;
    RpMsgTxInterface *rpmsg;
};

#endif // CONTROLLERSTATE_H
