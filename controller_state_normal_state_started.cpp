#include "controller_state_normal_state_started.h"
#include "controller_state_normal.h"
#include "controller.h"
#include "motor.h"
#include "msg_definition.h"
#include "controller_state_normal_state_stopped.h"
#include "hw.h"
#include "light_barrier.h"
#include "rpmsg_tx_interface.h"

int16_t post_info(char info);


ControllerStateNormalStateStarted::ControllerStateNormalStateStarted(Hw &hw,
                                                                     TimerInterface *timer,
                                                                     RpMsgTxInterface *rpmsg,
                                                                     Queue<Color,COLOR_QUEUE_SIZE> &colorQueue,
                                                                     CommandInterface *colorDetect,
                                                                     ObjectPool<BrickEjectCommand, 5> &ejectCommandPool)
    :ControllerStateNormalState(hw, timer, rpmsg),
      ejectCommandPool(ejectCommandPool),
      lb2BrickUnhandled(false),
      colorQueue(colorQueue),
      colorDetect(colorDetect)
{

}

void ControllerStateNormalStateStarted::onEntry()
{
    colorQueue.clear();
    hw.motor->start();
    timer->registerCommand(colorDetect,5);
}

void ControllerStateNormalStateStarted::onExit()
{
    hw.motor->stop();
    timer->unregisterCommand(colorDetect);
    colorQueue.clear();
}

void ControllerStateNormalStateStarted::processCmd(uint8_t cmd)
{

    switch(cmd)
    {
        case CMD_STOP:
            pSuperState->setState(&pSuperState->state_stopped);

    }
}

void ControllerStateNormalStateStarted::brickEjectCommandDone(BrickEjectCommand *command)
{
    ejectCommandPool.free(command);
}


void ControllerStateNormalStateStarted::doIt()
{
    if(hw.lightBarrier1->isInterrupted()) // brick after color detection?
    {
        if(lb2BrickUnhandled)   // brick unhandled?
        {
            rpmsg->post_info(0xaa);
            Color color = colorQueue.pull(); // get detected color

            rpmsg->post_info(color);

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
