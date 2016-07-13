/*
 * (C) Copyright 2016
 * Urs Fässler, bbv Software Services, http://bbv.ch
 *
 * SPDX-License-Identifier:	GPL-3.0+
 */

#include "PruLed.h"

#include <pru_cfg.h>

volatile register uint32_t __R30;

PruLed::PruLed()
{
  __R30 = 0;
}

void PruLed::setLeds(uint32_t value)
{
  __R30 = value;
}
