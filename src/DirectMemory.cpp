/*
 * (C) Copyright 2016
 * Urs Fässler, bbv Software Services, http://bbv.ch
 *
 * SPDX-License-Identifier:	GPL-3.0+
 */

#include "DirectMemory.h"

DirectMemory::DirectMemory(uint32_t *_base) :
  base(_base)
{
}

uint32_t DirectMemory::read(uint32_t offset) const
{
  return base[offset/4];
}

void DirectMemory::write(uint32_t offset, uint32_t value)
{
  base[offset/4] = value;
}
