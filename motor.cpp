#include <stdint.h>
#include "motor.h"
#include "hal.h"
#include "msg_definition.h"


int16_t post_info(char info);

Motor::Motor(int mask) : Gpo(mask)
{

}

void Motor::stop()
{
    clear();
    post_info(INFO_MOTOR_STOP);
}

void Motor::start()
{
    set();
    post_info(INFO_MOTOR_START);
}



