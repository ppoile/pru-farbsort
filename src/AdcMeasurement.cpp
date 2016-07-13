/*
 * (C) Copyright 2016
 * Urs Fässler, bbv Software Services, http://bbv.ch
 *
 * SPDX-License-Identifier:	GPL-3.0+
 */

#include "AdcMeasurement.h"

AdcMeasurement::AdcMeasurement(Adc &_adc) :
  adc(_adc)
{
}

uint16_t AdcMeasurement::read()
{
  clearFifo();
  waitForIdle();
  adc.start();
  waitForSample();
  const uint16_t value = popValue();
  adc.stop();
  return value;
}

void AdcMeasurement::clearFifo()
{
  while (adc.sampleCount() > 0) {
    adc.popSample();
  }
}

void AdcMeasurement::waitForIdle()
{
  while (!adc.isIdle()) {
  }
}

void AdcMeasurement::waitForSample()
{
  while (adc.sampleCount() < 1) {
    }
}

uint16_t AdcMeasurement::popValue()
{
  return adc.popSample() & 0xffff;
}

