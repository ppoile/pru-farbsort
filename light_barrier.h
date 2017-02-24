#ifndef LIGHTBARRIER_H
#define LIGHTBARRIER_H

#include "gpi.h"
#include "light_barrier_interface.h"

class LightBarrier : public Gpi, public LightBarrierInterface
{
private:


public:
    LightBarrier(int mask);
    bool isInterrupted(void);

};

#endif // LIGHTBARRIER_H
