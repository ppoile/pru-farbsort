#include "controller_state_normal_started.h"
#include "controller.h"
#include "motor.h"
#include "msg_definition.h"
#include "controller_state_normal_stopped.h"
#include "hw.h"
#include "light_barrier.h"
#include "rpmsg_tx_interface.h"
#include "controller_state.h"
#include "timer_interface.h"

int16_t post_info(char info);


ControllerStateNormalStarted::ControllerStateNormalStarted(Hw &hw,
                                                                     TimerInterface *timer,
                                                                     RpMsgTxInterface *rpmsg,
                                                                     Queue<Color,COLOR_QUEUE_SIZE> &colorQueue,
                                                                     CommandInterface *colorDetect,
                                                                     ObjectPool<BrickEjectCommand, 5> &ejectCommandPool)
    :ControllerState(hw, timer, rpmsg),
      ejectCommandPool(ejectCommandPool),
      lb2BrickUnhandled(false),
      colorQueue(colorQueue),
      colorDetect(colorDetect)
{

}

void ControllerStateNormalStarted::onEntry()
{
    colorQueue.clear();
    hw.motor->start();
    timer->registerCommand(colorDetect,5);
}

void ControllerStateNormalStarted::onExit()
{
    timer->unregisterCommand(colorDetect);
}

void ControllerStateNormalStarted::processCmd(Controller &controller, uint8_t cmd)
{

    switch(cmd)
    {
        case CMD_STOP:
            controller.setState(&controller.state_normal_stopped);
            break;

        case CMD_MODE_DIAGNOSTIC:
            controller.setState(&controller.state_diagnostic);
            break;

    }
}

void ControllerStateNormalStarted::brickEjectCommandDone(BrickEjectCommand *command)
{
    ejectCommandPool.free(command);
}


void ControllerStateNormalStarted::doIt()
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
