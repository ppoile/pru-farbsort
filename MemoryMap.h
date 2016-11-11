/*
 * (C) Copyright 2016
 * Urs Fässler, bbv Software Services, http://bbv.ch
 *
 * SPDX-License-Identifier:	GPL-3.0+
 */

#pragma once

#include <stdint.h>

class MemoryMap
{
public:
  virtual ~MemoryMap(){}

  virtual void write(uint32_t offset, uint32_t value) = 0;
  virtual uint32_t read(uint32_t offset) const = 0;

};

