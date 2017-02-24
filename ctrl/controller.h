#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "controller_state.h"
#include "controller_state_normal_started.h"
#include "controller_state_normal_stopped.h"
#include "controller_state_diagnostic.h"
#include "rpmsg_rx_interface.h"


class Hw;
class RpMsgTxInterface;


class Controller : public RpMsgRxInterface
{
public:
    Controller(Hw &hw, RpMsgTxInterface *rpmsg, ControllerStateDiagnostic &state_diagnostic, ControllerStateNormalStopped &state_normal_stopped, ControllerStateNormalStarted &state_normal_startedb);


    void processCmd(uint8_t cmd);
    void doIt();
    void setState(ControllerState* pState);
    void processesMessage(uint8_t *msg, uint16_t size);

    ControllerStateDiagnostic       &state_diagnostic;
    ControllerStateNormalStopped    &state_normal_stopped;
    ControllerStateNormalStarted    &state_normal_started;


private:
    Hw &hw;
    RpMsgTxInterface *rpmsg;
    ControllerState *pState;


    void handleGetAllInfo();

};

#endif // CONTROLLER_H
