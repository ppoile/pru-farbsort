/*
 * (C) Copyright 2016
 * Urs Fässler, bbv Software Services, http://bbv.ch
 *
 * SPDX-License-Identifier:	GPL-3.0+
 */

#include "Led.Dummy.h"

LedDummy::LedDummy(uint32_t defaultValue) :
  leds(defaultValue)
{
}

void LedDummy::setLeds(uint32_t value)
{
  leds = value;
}
