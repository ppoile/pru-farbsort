/*
 * (C) Copyright 2016
 * Urs Fässler, bbv Software Services, http://bbv.ch
 *
 * SPDX-License-Identifier:	GPL-3.0+
 */

#pragma once

#include "MemoryMap.h"
#include "precpp11.h"

#include <stdint.h>

class DirectMemory :
		public MemoryMap
{
public:
	DirectMemory(uint32_t *base);

  uint32_t read(uint32_t offset) const override;
  void write(uint32_t offset, uint32_t value) override;

private:
	uint32_t * const base;

};
