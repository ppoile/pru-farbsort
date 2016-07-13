/*
 * (C) Copyright 2016
 * Urs Fässler, bbv Software Services, http://bbv.ch
 *
 * SPDX-License-Identifier:	GPL-3.0+
 */

#ifndef PRULED_H
#define PRULED_H

#include "Led.h"
#include "precpp11.h"

#include <stdint.h>

class PruLed :
    public Led
{
public:
  PruLed();

  void setLeds(uint32_t value) override;

};

#endif
