#include <stdint.h>

#include "light_barrier.h"

LightBarrier::LightBarrier(int mask) : Gpi(mask)
{

}

bool LightBarrier::isInterrupted(void)
{
    return getStatus();
}

