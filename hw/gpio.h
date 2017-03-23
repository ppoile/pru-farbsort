#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>


class Gpio
{
public:
    Gpio(uint32_t mask);

    // outputs
    static const uint32_t MOTOR_MASK;
    static const uint32_t VALVE1_MASK;
    static const uint32_t VALVE2_MASK;
    static const uint32_t VALVE3_MASK;

    // inputs
    static const uint32_t PULSECOUNTER_MASK;
    static const uint32_t LIGHTBARRIER1_MASK;
    static const uint32_t LIGHTBARRIER2_MASK;
    static const uint32_t LIGHTBARRIERS3_TO_5_MASK;

protected:
     const int mask;
};

#endif // GPIO_H
