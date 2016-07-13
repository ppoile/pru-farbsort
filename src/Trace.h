/*
 * (C) Copyright 2016
 * Urs Fässler, bbv Software Services, http://bbv.ch
 *
 * SPDX-License-Identifier:	GPL-3.0+
 */

#ifndef TRACE_H
#define TRACE_H

#include <stdint.h>
#include <stddef.h>

class Trace
{
public:
  Trace(volatile char *_address, size_t _size);

  template<typename T>
  void hex(T value)
  {
    const uint8_t nibbleCount = 2*sizeof(T);
    for (int i = nibbleCount; i > 0; i--) {
      puts(hexNibble((value >> 4*(i-1)) & 0xf));
    }
  }
  void log(const char *value);

private:
  volatile char * const address;
  const size_t size;
  size_t index;

  void puts(char value);
  char hexNibble(uint8_t value) const;

};

#endif
