/*
 * (C) Copyright 2016
 * Urs Fässler, bbv Software Services, http://bbv.ch
 *
 * SPDX-License-Identifier:	GPL-3.0+
 */

#ifndef TIADC_H
#define TIADC_H

#include "Adc.h"
#include "MemoryMap.h"
#include "precpp11.h"

#include <stdint.h>

class TiAdc :
    public Adc
{
public:
  static const uint32_t CTRL;
  static const uint32_t ADC_CLKDIV;
  static const uint32_t STEPCONFIG1;
  static const uint32_t STEPDELAY1;
  static const uint32_t STEPCONFIG5;
  static const uint32_t STEPDELAY5;
  static const uint32_t FIFO1THRESHOLD;
  static const uint32_t FIFO1COUNT;
  static const uint32_t FIFO1DATA;
  static const uint32_t ADCSTAT;
  static const uint32_t STEPENABLE;

  TiAdc(MemoryMap &reg);

  bool isIdle() const override;
  void start() override;
  void stop() override;

  uint32_t sampleCount() const override;
  uint32_t popSample() override;

private:
  MemoryMap &reg;

};

#endif
