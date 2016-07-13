/*
 * (C) Copyright 2016
 * Urs Fässler, bbv Software Services, http://bbv.ch
 *
 * SPDX-License-Identifier:	GPL-3.0+
 */

#ifndef ADCMEASUREMENT_H
#define ADCMEASUREMENT_H

#include "Adc.h"

#include <stdint.h>

class AdcMeasurement
{
public:
  AdcMeasurement(Adc &adc);

  uint16_t read();

private:
  Adc &adc;

  void clearFifo();
  void waitForIdle();
  void waitForSample();
  uint16_t popValue();

};

#endif
