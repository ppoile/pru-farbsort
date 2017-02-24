#ifndef GPO_H
#define GPO_H

#include "gpio.h"

class Gpo : public Gpio
{
public:
    Gpo(uint32_t mask);

    void set();
    void clear();
    bool getStatus();
};

#endif // GPO_H
