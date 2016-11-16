#include "adc.h"

namespace {
  uint32_t *const AdcBaseAddr = (uint32_t*)0x44E0D000;

  const uint32_t CTRL = 0x40;
  const uint32_t ADC_CLKDIV = 0x4c;
  const uint32_t STEPCONFIG1 = 0x64;
  const uint32_t STEPDELAY1 = 0x68;
  const uint32_t FIFO1THRESHOLD = 0xf4;
  const uint32_t FIFO1COUNT = 0xf0;
  const uint32_t FIFO1DATA = 0x200;
  const uint32_t ADCSTAT = 0x44;
  const uint32_t STEPENABLE = 0x54;

  uint32_t register_read(uint32_t offset)
  {
    return AdcBaseAddr[offset / 4];
  }

  void register_write(uint32_t offset, uint32_t value)
  {
    AdcBaseAddr[offset / 4] = value;
  }
}

void adc_init()
{
  register_write(ADC_CLKDIV, 8-1);
  register_write(CTRL, 6);
  register_write(CTRL, 7);
  register_write(STEPCONFIG1, 0x04000008);
  register_write(STEPDELAY1, 0x0f000098);
  register_write(FIFO1THRESHOLD, 20-1);
}

uint16_t adc_read()
{
  while (register_read(FIFO1COUNT) > 0) {
    (void)register_read(FIFO1DATA);
  }
  while (register_read(ADCSTAT) & (1<<5)) {
  }
  register_write(STEPENABLE, 0x2);
  while (register_read(FIFO1COUNT) < 1) {
  }
  const uint16_t value = register_read(FIFO1DATA) & 0xFFFF;
  register_write(STEPENABLE, 0x0);
  return value;
}
