/*
 * (C) Copyright 2016
 * Urs Fässler, bbv Software Services, http://bbv.ch
 *
 * SPDX-License-Identifier:	GPL-3.0+
 */

#include "Trace.h"

Trace::Trace(volatile char *_address, size_t _size) :
  address(_address),
  size(_size),
  index(0)
{
}

void Trace::log(const char *value)
{
  for (const char *itr = value; *itr != '\0'; itr++) {
    puts(*itr);
  }
}

void Trace::puts(char value)
{
  address[index] = value;
  index++;
  if (index >= size) {
    index = 0;
  }
  address[index] = '\0';
}

char Trace::hexNibble(uint8_t value) const
{
  if (value > 0xf) {
    return '?';
  }
  if (value >= 0xa) {
    return value - 0xa + 'a';
  }
  return value + '0';
}
