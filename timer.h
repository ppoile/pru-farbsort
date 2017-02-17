#include <stdint.h>



#define TIMER_MAX_SCHEDULES 10

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


public:
    Timer();
    void schedule(CommandInterface *command, int ticks);
    void poll();
    void start();
    bool elapsed();
    void stop();
    uint32_t get_ticks();
    int lastTimer;


};
