#include "util.h"

void appendNumber(char *buffer, uint32_t value)
{
  char *p = buffer;
  for (int nibble_index = 7; nibble_index >= 0; --nibble_index) {
    uint32_t nibble = (value >> (nibble_index * 4)) & 0xF;
    char ch;
    if (nibble <= 9) {
      ch = '0' + nibble;
    }
    else {
      ch = 'a' + nibble - 10;
    }
    *p++ = ch;
  }
}
