#ifndef COLORDETECT_H
#define COLORDETECT_H

#include "command_interface.h"
#include "common.h"
#include "queue.h"

class Timer;
class Hw;

class ColorDetect : public CommandInterface
{
public:
    ColorDetect(Hw &hw, Timer &timer, Queue<Color, COLOR_QUEUE_SIZE> &colorQueue);

    void execute();

private:
    int16_t adcOld;
    int16_t diffOld;
    Color lastColor;

    void evalColor(uint16_t adc);
    Timer &timer;
    Hw &hw;
    Queue<Color, COLOR_QUEUE_SIZE> &colorQueue;
};

#endif // COLORDETECT_H
