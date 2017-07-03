#ifndef CONTROLLERSTATENORMALSTATESTOPPED_H
#define CONTROLLERSTATENORMALSTATESTOPPED_H

#include "controller_state.h"

struct Hw;
class TimerInterface;
class RpMsgTxInterface;

class ControllerStateNormalStopped : public ControllerState
{
public:
    ControllerStateNormalStopped(Hw &hw, TimerInterface *timer, RpMsgTxInterface *rpmsg);
    void processCmd(Controller &controller, uint8_t cmd);
    void onEntry();
};

#endif // CONTROLLERSTATENORMALSTATESTOPPED_H
