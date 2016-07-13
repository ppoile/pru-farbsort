/*
 * (C) Copyright 2016
 * Urs Fässler, bbv Software Services, http://bbv.ch
 *
 * SPDX-License-Identifier:	GPL-3.0+
 */

#include "DirectMemory.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

TEST(MemoryMap_Test, returns_value_form_base_plus_offset)
{
  uint32_t memory[0x100];
  DirectMemory testee{memory};
  memset(memory, 0, sizeof(memory));
  memory[0] = 42;
  memory[20/4] = 57;
  memory[100/4] = 23;

  ASSERT_EQ(42, testee.read(0));
  ASSERT_EQ(57, testee.read(20));
  ASSERT_EQ(23, testee.read(100));
}

TEST(MemoryMap_Test, writes_to_base_plus_offset)
{
  uint32_t memory[0x100];
  DirectMemory testee{memory};
  memset(memory, 0, sizeof(memory));

  testee.write(0, 42);
  testee.write(20, 57);
  testee.write(100, 23);

  ASSERT_EQ(42, memory[0]);
  ASSERT_EQ(57, memory[20/4]);
  ASSERT_EQ(23, memory[100/4]);
}
