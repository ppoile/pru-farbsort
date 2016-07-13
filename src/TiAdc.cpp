/*
 * (C) Copyright 2016
 * Urs Fässler, bbv Software Services, http://bbv.ch
 *
 * SPDX-License-Identifier:	GPL-3.0+
 */

#include "TiAdc.h"

const uint32_t TiAdc::CTRL            = 0x40;
const uint32_t TiAdc::ADC_CLKDIV      = 0x4c;
const uint32_t TiAdc::STEPCONFIG5     = 0x84;
const uint32_t TiAdc::STEPDELAY5      = 0x88;
const uint32_t TiAdc::STEPCONFIG1     = 0x64;
const uint32_t TiAdc::STEPDELAY1      = 0x68;
const uint32_t TiAdc::FIFO1THRESHOLD  = 0xf4;
const uint32_t TiAdc::FIFO1COUNT      = 0xf0;
const uint32_t TiAdc::FIFO1DATA       = 0x200;
const uint32_t TiAdc::ADCSTAT         = 0x44;
const uint32_t TiAdc::STEPENABLE      = 0x54;

TiAdc::TiAdc(MemoryMap &_reg) :
  reg(_reg)
{
  reg.write(ADC_CLKDIV,         8-1);
  reg.write(CTRL,                 6);
  reg.write(CTRL,                 7);

  reg.write(STEPCONFIG1, 0x04280008);
  reg.write(STEPDELAY1,  0x0f000098);

  reg.write(FIFO1THRESHOLD, 20-1);
}

void TiAdc::start()
{
  reg.write(STEPENABLE, 0x2);
}

void TiAdc::stop()
{
  reg.write(STEPENABLE, 0x0);
}

uint32_t TiAdc::sampleCount() const
{
  return reg.read(FIFO1COUNT);
}

uint32_t TiAdc::popSample()
{
  return reg.read(FIFO1DATA);
}

bool TiAdc::isIdle() const
{
  return (reg.read(ADCSTAT) & (1<<5)) == 0;
}
