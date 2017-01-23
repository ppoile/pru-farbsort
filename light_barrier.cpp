#include <stdint.h>
#include <pru_cfg.h>

#include "light_barrier.h"

volatile register uint32_t __R31;

LightBarrier::LightBarrier(int mask) : Gpi(mask)
{

}

bool LightBarrier::isInterrupted(void)
{
    return __R31 & mask;
}

