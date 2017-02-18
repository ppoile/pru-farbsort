#ifndef CONTROLLERSTATE_H
#define CONTROLLERSTATE_H

#include <stdint.h>

struct Hw;
class Controller;
class Timer;

class ControllerState
{
public:
    ControllerState(Hw &hw, Timer &timer);

    virtual void processCmd(Controller &controller, uint8_t cmd) = 0;
    virtual void onEntry(){};
    virtual void doIt(){};

protected:
    Hw &hw;
    Timer &timer;
};

#endif // CONTROLLERSTATE_H
