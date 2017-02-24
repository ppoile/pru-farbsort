#ifndef CONTROLLERSTATENORMALSTATESTOPPED_H
#define CONTROLLERSTATENORMALSTATESTOPPED_H

#include "controller_state_normal_state.h"

struct Hw;
class TimerInterface;
class RpMsgTxInterface;

class ControllerStateNormalStateStopped : public ControllerStateNormalState
{
public:
    ControllerStateNormalStateStopped(Hw &hw, TimerInterface *timer, RpMsgTxInterface *rpmsg);
    void processCmd(uint8_t cmd);
    void onEntry();
};

#endif // CONTROLLERSTATENORMALSTATESTOPPED_H
