#ifndef BRICKEJECTCOMMAND_H
#define BRICKEJECTCOMMAND_H

#include "command_interface.h"
#include "common.h"

class Timer;
class Hw;
class Piston;

class BrickEjectCommand;

typedef void(*CallbackFunc)(BrickEjectCommand *);

class BrickEjectCommand : public CommandInterface
{
public:
    BrickEjectCommand();

    void ejectColor(Hw &hw, Timer &timer, Color color, CallbackFunc callback);
    void execute();

private:
    static Piston *piston;
    static Timer *timer;
    static CallbackFunc callback;

    typedef enum{ Piston_Push, Piston_Pull} PistonCommand;
    PistonCommand command;
};

#endif // BRICKEJECTCOMMAND_H
