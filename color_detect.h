#ifndef COLORDETECT_H
#define COLORDETECT_H

#include "command_interface.h"
#include "common.h"
#include "queue.h"

class Timer;
class Hw;
class TimerInterface;
class RpMsgTxInterface;

class ColorDetect : public CommandInterface
{
public:
    ColorDetect(Hw &hw, TimerInterface *timer, Queue<Color, COLOR_QUEUE_SIZE> &colorQueue, RpMsgTxInterface *rpmsg);

    void execute();

private:
    int16_t adcOld;
    int16_t diffOld;
    Color lastColor;

    Hw &hw;
    TimerInterface *timer;
    Queue<Color, COLOR_QUEUE_SIZE> &colorQueue;
    RpMsgTxInterface *rpmsg;

    void evalColor(uint16_t adc, bool minimum);
};

#endif // COLORDETECT_H
