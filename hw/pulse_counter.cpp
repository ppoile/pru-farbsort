#include <stdint.h>
#include "pulse_counter.h"
#include "gpi.h"

PulseCounter::PulseCounter(int mask) : Gpi(mask), slopeCount(0), bOldState(false)
{

}

int PulseCounter::reset()
{
    int value = slopeCount;
    slopeCount = 0;
    return value;
}

void PulseCounter::poll()
{
    bool newState = getStatus();

    if(newState != bOldState)
    {
        bOldState = newState;
        slopeCount++;
    }
}
