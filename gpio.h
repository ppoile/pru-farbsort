#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>


class Gpio
{
public:
    Gpio(uint32_t mask);

protected:
     const int mask;
};

#endif // GPIO_H
