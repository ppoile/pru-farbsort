/*
 * (C) Copyright 2016
 * Urs Fässler, bbv Software Services, http://bbv.ch
 *
 * SPDX-License-Identifier:	GPL-3.0+
 */

#pragma once

#include "MemoryMap.h"

#include <gmock/gmock.h>

class MemoryMapMock :
    public MemoryMap
{
public:
  MOCK_METHOD2(write, void(uint32_t, uint32_t));
  MOCK_CONST_METHOD1(read, uint32_t(uint32_t));

};
