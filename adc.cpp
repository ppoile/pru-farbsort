#include "adc.h"

namespace {
  volatile uint32_t *const AdcBaseAddr = (uint32_t*)0x44E0D000;

  const uint32_t CTRL = 0x40;
  const uint32_t ADC_CLKDIV = 0x4c;
  const uint32_t STEPCONFIG1 = 0x64;
  //const uint32_t STEPDELAY1 = 0x68;
  const uint32_t FIFO1THRESHOLD = 0xf4;
  const uint32_t FIFO1COUNT = 0xf0;
  const uint32_t FIFO1DATA = 0x200;
  const uint32_t ADCSTAT = 0x44;
  const uint32_t STEPENABLE = 0x54;
  const uint32_t FSM_BUSY = (1<<5);

  uint32_t register_read(uint32_t offset)
  {
    return AdcBaseAddr[offset / 4];
  }

  void register_write(uint32_t offset, uint32_t value)
  {
    AdcBaseAddr[offset / 4] = value;
  }
}

    int32_t adc_state =0 ;

Adc::Adc()
{
  register_write(ADC_CLKDIV, 8-1);      // ACD clock will be divided by
  register_write(CTRL, 6);  // step config registers writable, store the channel id tag in the adc fifo

  register_write(STEPCONFIG1, 0x04000008);  // fifo1, channel 1, 4 sample average, SW enabled, one-shot
  //register_write(STEPDELAY1, 0x0f000098);   // sample delay 0xf clock cycles, open delay 0x98
  register_write(FIFO1THRESHOLD, 20-1);     // fifo threshold level 20

  register_write(CTRL, 7);  // turn on TSC_ADC_SS.
}

uint16_t Adc::read()
{
  while (register_read(FIFO1COUNT) > 0) {
    (void)register_read(FIFO1DATA);
  }
  while (register_read(ADCSTAT) & FSM_BUSY) {
  }

  register_write(STEPENABLE, 0x2);

  while (register_read(FIFO1COUNT) < 1) {
  }

  const uint16_t value = register_read(FIFO1DATA) & 0x0FFF;
  register_write(STEPENABLE, 0x0);

  return value;
}
