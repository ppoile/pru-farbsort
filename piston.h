#ifndef PISTON_H
#define PISTON_H

#include "gpo.h"
#include "piston_interface.h"

class Piston : public PistonInterface, public Gpo
{
public:
    Piston(uint32_t mask);

    void push();
    void pull();
    bool isPushed();
};

#endif // PISTON_H
