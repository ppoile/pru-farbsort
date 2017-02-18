#ifndef CONTROLLERSTATEDIAGNOSTIC_H
#define CONTROLLERSTATEDIAGNOSTIC_H

#include "controller_state.h"

struct Hw;
class Controller;

class ControllerStateDiagnostic : public ControllerState
{
public:
    ControllerStateDiagnostic(Hw &hw, Timer &timer);

    void processCmd(Controller &controller, uint8_t cmd);
};

#endif // CONTROLLERSTATEDIAGNOSTIC_H
