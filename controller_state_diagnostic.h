#ifndef CONTROLLERSTATEDIAGNOSTIC_H
#define CONTROLLERSTATEDIAGNOSTIC_H

#include "controller_state.h"

struct Hw;
class Controller;

class ControllerStateDiagnostic : public ControllerState
{
public:
    ControllerStateDiagnostic();

    void processCmd(Hw &hw, Controller &controller, uint8_t cmd);
};

#endif // CONTROLLERSTATEDIAGNOSTIC_H
