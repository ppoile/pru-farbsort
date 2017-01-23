#include "piston.h"
#include "msg_definition.h"

int16_t post_info(char info);

Piston::Piston(uint8_t mask): Gpo(mask)
{

}

void Piston::push()
{
    set();
}

void Piston::pull()
{
    clear();
    //@todo message shall depend on instance
    post_info(INFO_VALVE_1_OFF);
}
