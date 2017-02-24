#include "controller.h"
#include "controller_state_normal.h"
#include "controller_state_diagnostic.h"
#include "motor.h"
#include "version.h"
#include "msg_definition.h"
#include "piston.h"
#include "light_barrier.h"
#include "hw.h"
#include "pru_rpmsg.h"
#include "rpmsg_tx_interface.h"

#ifdef ADC_LOGGING
extern uint8_t adc_values[200];
#endif

Controller::Controller(Hw &hw, RpMsgTxInterface *rpmsg, ControllerStateDiagnostic &state_diagnostic, ControllerStateNormal &state_normal):
                    state_diagnostic(state_diagnostic),
                    state_normal(state_normal),
                    hw(hw),
                    rpmsg(rpmsg)
{
    pState = &state_normal;
    rpmsg->registerReceiver(this);

}



void Controller::setState(ControllerState* pNewState)
{
    pState->onExit();
    this->pState = pNewState;
    pState->onEntry();
}

void Controller::processCmd(uint8_t cmd)
{
    //handle commands independent of current state
    switch(cmd)
    {
        case CMD_GET_ALL_INFO:
            handleGetAllInfo();
            break;

        case CMD_RESET:
            setState(&state_normal);
            break;

    }

    // handle state dependent commands
    pState->processCmd(*this, cmd);
}



void Controller::handleGetAllInfo()
{
    char buffer[2];
    buffer[0] = VERSION_MAJOR;
    buffer[1] = VERSION_MINOR;
    rpmsg->post_msg(buffer, 2);

    if (hw.motor->isRunning()) {
      rpmsg->post_info(INFO_MOTOR_START);
    }
    else {
      rpmsg->post_info(INFO_MOTOR_STOP);
    }
    if (hw.piston0->isPushed()) {
      rpmsg->post_info(INFO_VALVE_1_ON);
    }
    else {
      rpmsg->post_info(INFO_VALVE_1_OFF);
    }
    if (hw.piston1->isPushed()) {
      rpmsg->post_info(INFO_VALVE_2_ON);
    }
    else {
      rpmsg->post_info(INFO_VALVE_2_OFF);
    }
    if (hw.piston2->isPushed()) {
      rpmsg->post_info(INFO_VALVE_3_ON);
    }
    else {
      rpmsg->post_info(INFO_VALVE_3_OFF);
    }
    if (pState == &state_normal) {
      rpmsg->post_info(INFO_MODE_NORMAL);
    }
    else {
      rpmsg->post_info(INFO_MODE_DIAGNOSTIC);
    }
    if (state_normal.getState() == &state_normal.state_started) {
      rpmsg->post_info(INFO_CTRL_START);
    }
    else {
      rpmsg->post_info(INFO_CTRL_STOP);
    }


    if(hw.lightBarrier0->isInterrupted()) {
      rpmsg->post_info(INFO_LIGHT_BARRIER_1_DARK);
    }
    else {
      rpmsg->post_info(INFO_LIGHT_BARRIER_1_BRIGHT);
    }
    if (hw.lightBarrier1->isInterrupted()) {
      rpmsg->post_info(INFO_LIGHT_BARRIER_2_DARK);
    }
    else {
      rpmsg->post_info(INFO_LIGHT_BARRIER_2_BRIGHT);
    }
    if (hw.lightBarrierEmergencyStop->isInterrupted()) {
      rpmsg->post_info(INFO_EMERGENCY_STOP_ON);
    }
    else {
      rpmsg->post_info(INFO_EMERGENCY_STOP_OFF);
    }

#ifdef ADC_LOGGING
    rpmsg->post_msg( (const char*) adc_values, 200);
#endif
}

void Controller::doIt()
{
    pState->doIt();
}

void Controller::processesMessage(uint8_t *msg, uint16_t size){
    processCmd((*msg));
}


