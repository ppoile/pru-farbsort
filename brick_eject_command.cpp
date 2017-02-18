#include "brick_eject_command.h"
#include "hw.h"
#include "timer.h"
#include "piston.h"

namespace
{
    const uint16_t PUSH_TIME = 30;  // 300 ms, in units of 10 ms

    const uint16_t delay[] = {55, 150, 240};
}

Piston *BrickEjectCommand::piston;
Timer *BrickEjectCommand::timer;
BrickEjectCommandDoneInterface *BrickEjectCommand::callback;

BrickEjectCommand::BrickEjectCommand()
{

}

void BrickEjectCommand::ejectColor(Hw &hw, Timer &timer, Color color, BrickEjectCommandDoneInterface *callback)
{
    command = Piston_Push;
    uint16_t ticks = 0;

    this->timer = &timer;
    this->callback = callback;

    // decide which piston to use
    switch(color)
    {
        case BLUE:
            piston = &hw.piston0;
            ticks = delay[0];
            break;

        case RED:
            piston = &hw.piston1;
            ticks = delay[1];
            break;

        case WHITE:
            piston = &hw.piston2;
            ticks = delay[2];
            break;

        default:
            break;

    }

    if(ticks)
    {
        timer.schedule(this, ticks);
    }
    else
    {
        callback->brickEjectCommandDone(this);
    }

}

void BrickEjectCommand::execute()
{
    if(command == Piston_Push) // push
    {
        piston->push();
        command = Piston_Pull;
        timer->schedule(this, PUSH_TIME); // prepare pull
    }
    else // pull
    {
        piston->pull();
        callback->brickEjectCommandDone(this);

    }
}

