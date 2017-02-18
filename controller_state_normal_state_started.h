#ifndef CONTROLLERSTATENORMALSTATESTARTED_H
#define CONTROLLERSTATENORMALSTATESTARTED_H

#include "controller_state_normal_state.h"
#include "object_pool.h"
#include "brick_eject_command.h"
#include "queue.h"
#include "timer.h"
#include "brick_eject_command_done_interface.h"

struct Hw;
class ColorDetect;

class ControllerStateNormalStateStarted : public ControllerStateNormalState, BrickEjectCommandDoneInterface
{
public:
    ControllerStateNormalStateStarted(Hw &hw,
                                      Timer &timer,
                                      Queue<Color,COLOR_QUEUE_SIZE> &colorQueue,
                                      ColorDetect &colorDetect,
                                      ObjectPool<BrickEjectCommand, 5> &ejectCommandPool);

    void processCmd(uint8_t cmd);
    void onEntry();
    void doIt();

private:
    ObjectPool<BrickEjectCommand, 5> &ejectCommandPool;
    void brickEjectCommandDone(BrickEjectCommand *command);
    bool lb2BrickUnhandled;
    Queue<Color,COLOR_QUEUE_SIZE> &colorQueue;
    ColorDetect &colorDetect;
};

#endif // CONTROLLERSTATENORMALSTATESTARTED_H
