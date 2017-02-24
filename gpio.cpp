#include "gpio.h"


const uint32_t Gpio::MOTOR_MASK = 0x8000;
const uint32_t Gpio::VALVE1_MASK = 0x4000;
const uint32_t Gpio::VALVE2_MASK = 0x80;
const uint32_t Gpio::VALVE3_MASK = 0x20;

// inputs
const uint32_t Gpio::PULSECOUNTER_MASK = 0x8000;
const uint32_t Gpio::LIGHTBARRIER1_MASK = 0x4000;
const uint32_t Gpio::LIGHTBARRIER2_MASK = 0x10000;
const uint32_t Gpio::LIGHTBARRIERS3_TO_5_MASK = 0x4;

Gpio::Gpio(uint32_t mask) : mask(mask)
{

}

