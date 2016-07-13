/*
 * (C) Copyright 2016
 * Urs Fässler, bbv Software Services, http://bbv.ch
 *
 * SPDX-License-Identifier:	GPL-3.0+
 */

#include "Up.h"

#include <Led.h>

namespace lightsequence
{

Up::Up(Led &_led) :
  led(_led),
  count(0)
{
}

void Up::tick()
{
  led.setLeds(1<<count);
  count = (count + 1) % 4;
}

}

