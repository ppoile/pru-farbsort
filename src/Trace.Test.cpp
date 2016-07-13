/*
 * (C) Copyright 2016
 * Urs Fässler, bbv Software Services, http://bbv.ch
 *
 * SPDX-License-Identifier:	GPL-3.0+
 */

#include "Trace.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

TEST(MemoryMap_Test, write_a_string_to_the_beginning_of_the_memory)
{
  char memory[0x10];
  Trace testee(memory, sizeof(memory));

  testee.log("hello");

  ASSERT_EQ("hello", std::string{memory});
}

TEST(MemoryMap_Test, writing_a_string_adds_the_terminator)
{
  char memory[0x10];
  Trace testee(memory, sizeof(memory));
  memset(memory, 0xff, sizeof(memory));

  testee.log("x");

  ASSERT_EQ('\0', memory[1]);
}

TEST(MemoryMap_Test, write_string_wraps_around)
{
  char memory[0x8];
  Trace testee(memory, sizeof(memory));

  testee.log("hello world");

  ASSERT_EQ("rld", std::string{memory});
}

TEST(MemoryMap_Test, starts_writing_at_the_end_of_the_last_write)
{
  char memory[0x10];
  Trace testee(memory, sizeof(memory));

  testee.log("hello ");
  testee.log("world");

  ASSERT_EQ("hello world", std::string{memory});
}

TEST(MemoryMap_Test, write_8_bit_unsigned_int)
{
  char memory[0x10];
  Trace testee(memory, sizeof(memory));
  const uint8_t value = 0x42;

  testee.hex(value);

  ASSERT_EQ("42", std::string{memory});
}

TEST(MemoryMap_Test, write_8_bit_signed_int)
{
  char memory[0x10];
  Trace testee(memory, sizeof(memory));
  const int8_t value = -128;

  testee.hex(value);

  ASSERT_EQ("80", std::string{memory});
}

TEST(MemoryMap_Test, write_16_bit_unsigned_int)
{
  char memory[0x10];
  Trace testee(memory, sizeof(memory));
  const uint16_t value = 0xcafe;

  testee.hex(value);

  ASSERT_EQ("cafe", std::string{memory});
}

TEST(MemoryMap_Test, write_16_bit_signed_int)
{
  char memory[0x10];
  Trace testee(memory, sizeof(memory));
  const uint16_t value = -1;

  testee.hex(value);

  ASSERT_EQ("ffff", std::string{memory});
}

TEST(MemoryMap_Test, write_32_bit_unsigned_int)
{
  char memory[0x10];
  Trace testee(memory, sizeof(memory));
  const uint32_t value = 0xdeadbeef;

  testee.hex(value);

  ASSERT_EQ("deadbeef", std::string{memory});
}

TEST(MemoryMap_Test, writes_leading_zeros_for_hex_output)
{
  char memory[0x10];
  Trace testee(memory, sizeof(memory));
  const uint32_t value = 0;

  testee.hex(value);

  ASSERT_EQ("00000000", std::string{memory});
}
