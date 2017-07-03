#include "conveyor_belt_observer.h"
#include "timer_interface.h"
#include "rpmsg_tx_interface.h"
#include "msg_definition.h"
#include "pulse_counter_interface.h"

ConveyorBeltObserver::ConveyorBeltObserver(PulseCounterInterface *pulseCounter, TimerInterface *timer, RpMsgTxInterface *rpmsg)
    :pulseCounter(pulseCounter), timer(timer), rpmsg(rpmsg), bIsRunning(false)
{

}

// check if in observation interval slopes on pulse counter occured to deduce running conveyor belt
void ConveyorBeltObserver::execute()
{
    int slopeCount = pulseCounter->reset();

    if((slopeCount) && (!bIsRunning))
    {
        bIsRunning = true;
        rpmsg->post_info(INFO_CONVEYOR_RUNNING);
    }
    else if ((!slopeCount) && (bIsRunning))
    {
        bIsRunning = false;
        rpmsg->post_info(INFO_CONVEYOR_STOPPED);
    }

    timer->registerCommand(this, 20); // 200 ms, value in units of 10ms
}

bool ConveyorBeltObserver::isRunning()
{
    return bIsRunning;
}

