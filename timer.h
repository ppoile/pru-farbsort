#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include "timer_interface.h"

#define TIMER_MAX_SCHEDULES 10

class CommandInterface;

class Timer : public TimerInterface
{

private :
    typedef struct
    {
        CommandInterface* command; // command to execute when ticks reach 0
        int ticks;
    } TimerRegistration;

    TimerRegistration registration[TIMER_MAX_SCHEDULES];


public:
    Timer();
    void registerCommand(CommandInterface *command, int ticks);
    void unregisterCommand(CommandInterface *command);
    void poll();
    void start();
    bool elapsed();
    void stop();
    uint32_t get_ticks();
    int lastTimer;


};

#endif // TIMER_H


