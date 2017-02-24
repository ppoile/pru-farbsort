#ifndef CONTROLLERSTATENORMALSTATESTARTED_H
#define CONTROLLERSTATENORMALSTATESTARTED_H

#include "controller_state_normal_state.h"
#include "object_pool.h"
#include "brick_eject_command.h"
#include "queue.h"
#include "brick_eject_command_done_interface.h"

struct Hw;
class ColorDetect;
class TimerInterface;
class RpMsgTxInterface;

class ControllerStateNormalStateStarted : public ControllerStateNormalState, BrickEjectCommandDoneInterface
{
public:
    ControllerStateNormalStateStarted(Hw &hw,
                                      TimerInterface *timer,
                                      RpMsgTxInterface *rpmsg,
                                      Queue<Color,COLOR_QUEUE_SIZE> &colorQueue,
                                      CommandInterface *colorDetect,
                                      ObjectPool<BrickEjectCommand, 5> &ejectCommandPool);

    void processCmd(uint8_t cmd);
    void onEntry();
    void onExit();
    void doIt();

private:
    ObjectPool<BrickEjectCommand, 5> &ejectCommandPool;
    bool lb2BrickUnhandled;
    Queue<Color,COLOR_QUEUE_SIZE> &colorQueue;
    CommandInterface *colorDetect;

    void brickEjectCommandDone(BrickEjectCommand *command);
};

#endif // CONTROLLERSTATENORMALSTATESTARTED_H
