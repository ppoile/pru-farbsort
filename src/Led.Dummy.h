/*
 * (C) Copyright 2016
 * Urs Fässler, bbv Software Services, http://bbv.ch
 *
 * SPDX-License-Identifier:	GPL-3.0+
 */

#ifndef LED_DUMMY_H
#define LED_DUMMY_H

#include "Led.h"

#include <stdint.h>

class LedDummy :
    public Led
{
public:
  LedDummy(uint32_t defaultValue);

  void setLeds(uint32_t value) override;

  uint32_t leds;

};

#endif
