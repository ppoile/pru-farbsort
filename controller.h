#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "controller_state.h"
#include "controller_state_normal.h"
#include "controller_state_diagnostic.h"
#include "rpmsg_rx_interface.h"


class Hw;
class RpMsgTxInterface;

class Controller : public RpMsgRxInterface
{
public:
    Controller(Hw &hw, RpMsgTxInterface *rpmsg, ControllerStateDiagnostic &state_diagnostic, ControllerStateNormal &state_normal);


    void processCmd(uint8_t cmd);
    void doIt();
    void setState(ControllerState* pState);
    void processesMessage(uint8_t *msg, uint16_t size);

    ControllerStateDiagnostic   &state_diagnostic;
    ControllerStateNormal       &state_normal;


private:
    Hw &hw;
    RpMsgTxInterface *rpmsg;
    ControllerState *pState;


    void handleGetAllInfo();

};

#endif // CONTROLLER_H
