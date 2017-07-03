#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include "adc_interface.h"

class Adc: public AdcInterface
{
    public:
        Adc();
        uint16_t read();

};

#endif

