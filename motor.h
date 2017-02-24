#ifndef MOTOR_H
#define MOTOR_H

#include "gpo.h"
#include "motor_interface.h"

class Motor: public MotorInterface, public Gpo
{
public:
    Motor(int mask);

    void start();
    void stop();
    bool isRunning();

};

#endif // MOTOR_H
