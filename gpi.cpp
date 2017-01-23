#include "gpi.h"
#include <stdint.h>

volatile register uint32_t __R31;

Gpi::Gpi(uint32_t mask) : Gpio(mask)
{

}

bool Gpi::getStatus()
{
    return __R31 & mask;
}

