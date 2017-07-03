#ifndef BRICKEJECTCOMMAND_H
#define BRICKEJECTCOMMAND_H

#include "command_interface.h"
#include "common.h"
#include "controller_state_normal_started.h"
#include "brick_eject_command_done_interface.h"

class TimerInterface;
class Hw;
class PistonInterface;

class BrickEjectCommand;


class BrickEjectCommand : public CommandInterface
{
public:
    BrickEjectCommand();

    void ejectColor(Hw &hw, TimerInterface *timer, Color color, BrickEjectCommandDoneInterface *callback);
    void execute();

private:
    PistonInterface *piston;
    static TimerInterface *timer;
    static BrickEjectCommandDoneInterface *callback;

    typedef enum{ Piston_Push, Piston_Pull} PistonCommand;
    PistonCommand command;
};

#endif // BRICKEJECTCOMMAND_H
