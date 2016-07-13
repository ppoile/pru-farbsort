/*
 * (C) Copyright 2016
 * Urs Fässler, bbv Software Services, http://bbv.ch
 *
 * SPDX-License-Identifier:	GPL-3.0+
 */

#ifndef ADC_MOCK_H
#define ADC_MOCK_H

#include "Adc.h"

#include <gmock/gmock.h>

class AdcMock :
    public Adc
{
public:

  MOCK_CONST_METHOD0(isIdle, bool());
  MOCK_METHOD0(start, void());
  MOCK_METHOD0(stop, void());
  MOCK_CONST_METHOD0(sampleCount, uint32_t());
  MOCK_METHOD0(popSample, uint32_t());

};

#endif
