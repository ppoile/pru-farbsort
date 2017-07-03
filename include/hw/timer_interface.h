#ifndef TIMERINTERFACE
#define TIMERINTERFACE

class CommandInterface;

class TimerInterface
{
public:

    virtual void registerCommand(CommandInterface *command, int ticks) = 0;
    virtual void unregisterCommand(CommandInterface *command) = 0;
};

#endif // TIMERINTERFACE

