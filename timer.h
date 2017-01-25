#include <stdint.h>

void timer_start();
void timer_poll();
bool timer_elapsed();
void timer_stop();
uint32_t timer_get_ticks();

#define TIMER_MAX_SCHEDULES 5

class CommandInterface;

class Timer
{

private :
    typedef struct
    {
        CommandInterface* command; // command to execute when ticks reach 0
        int ticks;
    } TimerRegistration;

    TimerRegistration registration[TIMER_MAX_SCHEDULES];
    int lastTimer;

public:
    Timer();
    void schedule(CommandInterface *command, int ticks);
    void poll();


};
