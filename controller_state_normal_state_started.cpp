#include "controller_state_normal_state_started.h"
#include "controller_state_normal.h"
#include "controller.h"
#include "motor.h"
#include "msg_definition.h"
#include "controller_state_normal_state_stopped.h"
#include "hw.h"
#include "light_barrier.h"

int16_t post_info(char info);


ControllerStateNormalStateStarted::ControllerStateNormalStateStarted(Hw &hw,
                                                                     Timer &timer,
                                                                     Queue<Color,COLOR_QUEUE_SIZE> &colorQueue,
                                                                     ColorDetect &colorDetect,
                                                                     ObjectPool<BrickEjectCommand, 5> &ejectCommandPool)
    :ControllerStateNormalState(hw, timer),
      colorQueue(colorQueue),
      colorDetect(colorDetect),
      ejectCommandPool(ejectCommandPool),
      lb2BrickUnhandled(false)
{

}

void ControllerStateNormalStateStarted::onEntry()
{
    hw.motor.start();
}

void ControllerStateNormalStateStarted::processCmd(uint8_t cmd)
{

    switch(cmd)
    {
        case CMD_STOP:
            pSuperState->setState(&pSuperState->state_stopped, hw);

    }
}

void ControllerStateNormalStateStarted::brickEjectCommandDone(BrickEjectCommand *command)
{
    ejectCommandPool.free(command);
}


void ControllerStateNormalStateStarted::doIt()
{
    if(hw.lightBarrier1.isInterrupted()) // brick after color detection?
    {
        if(lb2BrickUnhandled)   // brick unhandled?
        {
            post_info(0xaa);
            Color color = colorQueue.pull(); // get detected color

            post_info(color);

            BrickEjectCommand *command = ejectCommandPool.getObject();
            if(command)
            {

                command->ejectColor(hw, timer, color, this);
                lb2BrickUnhandled = false;
            }
            else
            {
                // @todo error handling, command pool exhausted
            }
        }
    }
    else // no brick
    {
        lb2BrickUnhandled = true;
    }
}
