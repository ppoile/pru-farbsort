#ifndef MOTOR_H
#define MOTOR_H

#include "gpo.h"

class Motor: public Gpo
{
public:
    Motor(int mask);

    void start();
    void stop();

};

#endif // MOTOR_H
