#include "hal.h"


volatile register uint32_t __R31;


// outputs
const uint32_t MOTOR_MASK = 0x8000;
const uint32_t VALVE1_MASK = 0x4000;
const uint32_t VALVE2_MASK = 0x80;
const uint32_t VALVE3_MASK = 0x20;

// inputs
const uint32_t PULSECOUNTER_MASK = 0x8000;
const uint32_t LIGHTBARRIER1_MASK = 0x4000;
const uint32_t LIGHTBARRIER2_MASK = 0x10000;
const uint32_t LIGHTBARRIERS3_TO_5_MASK = 0x4;


uint32_t get_all_inputs()
{
  return __R31 & ~LIGHTBARRIERS3_TO_5_MASK;
}
