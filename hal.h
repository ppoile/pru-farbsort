#pragma once

#include <stdint.h>


// outputs
extern const uint32_t MOTOR_MASK;
extern const uint32_t VALVE1_MASK;
extern const uint32_t VALVE2_MASK;
extern const uint32_t VALVE3_MASK;

// inputs
extern const uint32_t PULSECOUNTER_MASK;
extern const uint32_t LIGHTBARRIER1_MASK;
extern const uint32_t LIGHTBARRIER2_MASK;
extern const uint32_t LIGHTBARRIERS3_TO_5_MASK;



uint32_t get_all_inputs();
