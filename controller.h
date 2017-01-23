#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "controller_state.h"
#include "controller_state_normal.h"
#include "controller_state_diagnostic.h"


class Hw;

class Controller
{
public:
    Controller(Hw &hw);


    void processCmd(uint8_t cmd);
    void setState(ControllerState* pState);

    ControllerStateDiagnostic state_diagnostic;
    ControllerStateNormal   state_normal;


private:
    Hw &hw;
    ControllerState *pState;

    void handleGetAllInfo();

};

#endif // CONTROLLER_H
