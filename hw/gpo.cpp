#include "gpo.h"
#include <pru_cfg.h>

volatile register uint32_t __R30;

Gpo::Gpo(uint32_t mask) : Gpio(mask)
{

}

void Gpo::set()
{
    __R30 |= mask;
}

void Gpo::clear()
{
    __R30 &= ~mask;
}

bool Gpo::getStatus()
{
    return __R30 & mask;
}

