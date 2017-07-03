#ifndef CONVEYOR_BELT_OBSERVER_H
#define CONVEYOR_BELT_OBSERVER_H

#include "command_interface.h"

class RpMsgTxInterface;
class TimerInterface;
class PulseCounterInterface;

class ConveyorBeltObserver : public CommandInterface
{
public:
    ConveyorBeltObserver(PulseCounterInterface *pulseCounter, TimerInterface *timer, RpMsgTxInterface *rpmsg);

    void execute();

    bool isRunning();

private:
    PulseCounterInterface *pulseCounter;
    TimerInterface *timer;
    RpMsgTxInterface *rpmsg;

    bool bIsRunning;
};

#endif // CONVEYOR_BELT_OBSERVER_H
