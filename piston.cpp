#include "piston.h"
#include "msg_definition.h"


Piston::Piston(uint32_t mask): Gpo(mask)
{

}

void Piston::push()
{
    set();
}

void Piston::pull()
{
    clear();
}

bool Piston::isPushed()
{
    return getStatus();
}
