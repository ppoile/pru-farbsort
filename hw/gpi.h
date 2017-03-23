#ifndef GPI_H
#define GPI_H

#include "gpio.h"

class Gpi : public Gpio
{
public:
    Gpi(uint32_t mask);
    bool getStatus();
};

#endif // GPI_H
