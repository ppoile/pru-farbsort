#ifndef CONTROLLERSTATENORMALSTARTED_H
#define CONTROLLERSTATENORMALSTARTED_H

#include "object_pool.h"
#include "brick_eject_command.h"
#include "queue.h"
#include "brick_eject_command_done_interface.h"
#include "controller_state.h"

struct Hw;
class ColorDetect;
class TimerInterface;
class RpMsgTxInterface;

class ControllerStateNormalStarted : public ControllerState, public BrickEjectCommandDoneInterface
{
public:
    ControllerStateNormalStarted(Hw &hw,
                                      TimerInterface *timer,
                                      RpMsgTxInterface *rpmsg,
                                      Queue<Color,COLOR_QUEUE_SIZE> &colorQueue,
                                      CommandInterface *colorDetect,
                                      ObjectPool<BrickEjectCommand, 5> &ejectCommandPool);

    void processCmd(Controller &controller, uint8_t cmd);
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

#endif // CONTROLLERSTATENORMALSTARTED_H
