/*
 * (C) Copyright 2016
 * Urs Fässler, bbv Software Services, http://bbv.ch
 *
 * SPDX-License-Identifier:	GPL-3.0+
 */

#pragma once

#include <stdint.h>

class Adc
{
public:
  virtual ~Adc(){}

  virtual bool isIdle() const = 0;
  virtual void start() = 0;
  virtual void stop() = 0;

  virtual uint32_t sampleCount() const = 0;
  virtual uint32_t popSample() = 0;

};
