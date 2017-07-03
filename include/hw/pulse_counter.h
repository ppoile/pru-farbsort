#ifndef PULSE_COUNTER_H
#define PULSE_COUNTER_H

#include "gpi.h"
#include "pulse_counter_interface.h"

class PulseCounter : public Gpi, public PulseCounterInterface
{
public:
    PulseCounter(int mask);
    void poll();

    int reset();

private:
    int slopeCount;
    bool bOldState;
};

#endif // PULSE_COUNTER_H
