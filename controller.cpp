#include "controller.h"
#include "controller_state_normal.h"
#include "controller_state_diagnostic.h"
#include "motor.h"
#include "hal.h"
#include "version.h"
#include "msg_definition.h"
#include "piston.h"
#include "light_barrier.h"
#include "hw.h"
#include "pru_rpmsg.h"

extern bool rpmsg_connected;
extern int16_t post_info(char info);
extern int16_t post_event(char const *event, uint16_t length);
extern uint8_t adc_values[200];


Controller::Controller(Hw &hw, ControllerStateDiagnostic &state_diagnostic, ControllerStateNormal &state_normal):
                       hw(hw),
                       state_diagnostic(state_diagnostic),
                       state_normal(state_normal)
{
    pState = &state_normal;

}



void Controller::setState(ControllerState* pState)
{
    this->pState = pState;
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
    post_event(buffer, 2);

    if (hw.motor.getStatus()) {
      post_info(INFO_MOTOR_START);
    }
    else {
      post_info(INFO_MOTOR_STOP);
    }
    if (hw.piston0.getStatus()) {
      post_info(INFO_VALVE_1_ON);
    }
    else {
      post_info(INFO_VALVE_1_OFF);
    }
    if (hw.piston1.getStatus()) {
      post_info(INFO_VALVE_2_ON);
    }
    else {
      post_info(INFO_VALVE_2_OFF);
    }
    if (hw.piston2.getStatus()) {
      post_info(INFO_VALVE_3_ON);
    }
    else {
      post_info(INFO_VALVE_3_OFF);
    }
    if (pState == &state_normal) {
      post_info(INFO_MODE_NORMAL);
    }
    else {
      post_info(INFO_MODE_DIAGNOSTIC);
    }
    if (state_normal.getState() == &state_normal.state_started) {
      post_info(INFO_CTRL_START);
    }
    else {
      post_info(INFO_CTRL_STOP);
    }

    /*
    if (verbose) {
      post_info(INFO_VERBOSE_ON);
    }
    else {
      post_info(INFO_VERBOSE_OFF);
    }
    if (is_conveyor_running) {
      post_info(INFO_CONVEYER_RUNNING);
    }
    else {
      post_info(INFO_CONVEYER_STOPPED);
    }*/

    if(hw.lightBarrier0.isInterrupted()) {
      post_info(INFO_LIGHT_BARRIER_1_DARK);
    }
    else {
      post_info(INFO_LIGHT_BARRIER_1_BRIGHT);
    }
    if (hw.lightBarrier1.isInterrupted()) {
      post_info(INFO_LIGHT_BARRIER_2_DARK);
    }
    else {
      post_info(INFO_LIGHT_BARRIER_2_BRIGHT);
    }
    if (hw.lightBarrierEmergencyStop.isInterrupted()) {
      post_info(INFO_EMERGENCY_STOP_ON);
    }
    else {
      post_info(INFO_EMERGENCY_STOP_OFF);
    }

    post_event( (const char*) adc_values, 200);
}

void Controller::doIt()
{
    pState->doIt();
}


