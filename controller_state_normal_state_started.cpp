#include "controller_state_normal_state_started.h"
#include "controller_state_normal.h"
#include "controller.h"
#include "motor.h"
#include "msg_definition.h"
#include "controller_state_normal_state_stopped.h"
#include "hw.h"

int16_t post_info(char info);

ControllerStateNormalStateStarted::ControllerStateNormalStateStarted()
{

}

void ControllerStateNormalStateStarted::onEntry(Hw &hw)
{
    hw.motor.start();
}

void ControllerStateNormalStateStarted::processCmd(Hw &hw, uint8_t cmd)
{

    switch(cmd)
    {
        case CMD_STOP:
            pSuperState->setState(&pSuperState->state_stopped, hw);

    }
}
