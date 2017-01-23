#ifndef CONTROLLERSTATE_H
#define CONTROLLERSTATE_H

#include <stdint.h>

struct Hw;
class Controller;

class ControllerState
{
public:
    ControllerState();

    virtual void processCmd(Hw &hw, Controller &controller, uint8_t cmd) = 0;
    virtual void onEntry(Hw &hw){};
};

#endif // CONTROLLERSTATE_H
