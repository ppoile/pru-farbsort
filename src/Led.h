#ifndef LED_H
#define LED_H

#include <stdint.h>

class Led
{
public:
  virtual ~Led(){}

  virtual void setLeds(uint32_t) = 0;

};

#endif
