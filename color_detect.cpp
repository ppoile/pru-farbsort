#include <stdint.h>
#include "color_detect.h"
#include "adc.h"
#include "timer.h"
#include "hw.h"

extern uint8_t adc_values[200];
extern uint8_t adc_value_index;

extern int16_t post_event(char const *event, uint16_t length);

namespace {
    uint16_t const adcRefBlack  = 1228;
    uint16_t const adcRefBlue   = 1187;
    uint16_t const adcRefRed    = 815;
    uint16_t const adcRefWhite  = 768;

    uint16_t const thresholdBlack  =  (adcRefBlack + adcRefBlue) / 2;
    uint16_t const thresholdBlue   =  (adcRefBlue + adcRefRed) / 2;
    uint16_t const thresholdRed    =  (adcRefRed + adcRefWhite) / 2;
}

ColorDetect::ColorDetect(Hw &hw, Timer &timer, Queue<Color, COLOR_QUEUE_SIZE> &colorQueue):
    hw(hw), timer(timer), adcOld(0), diffOld(0), lastColor(BLACK), colorQueue(colorQueue)
{

}


void ColorDetect::execute()
{
    int16_t adc = hw.adc.read();
    int16_t diff = (adc - adcOld);
    int16_t diffDiff = diff - diffOld;

    // temporary fill the adc values into buffer to query from linux
    adc_values[adc_value_index++] = adc & 0xff;
    adc_values[adc_value_index++] = (adc >> 8) & 0xff;
    if(adc_value_index >= 200)
    {
        adc_value_index = 0;
    }

    if(((diff > 0) && (diffOld <= 0)) && (diffDiff > 0))// minimum shortly missed?
    {
         evalColor(adcOld);
    }
    else if(((diff < 0) && (diffOld >=0)) && (diffDiff < 0)) // maximum shortly missed
    {
        evalColor(adcOld);
    }

    adcOld = adc;
    diffOld = diff;

    // register this function for periodic call
    timer.schedule(this,5);
}

void ColorDetect::evalColor(uint16_t adc)
{
    Color color;

    if(adc > thresholdBlack)
    {
        color = BLACK;
    }
    else if (adc > thresholdBlue)
    {
        color = BLUE;
    }
    else if (adc > thresholdRed)
    {
        color = RED;
    }
    else
    {
        color = WHITE;
    }

    if(color != lastColor) // color changed?
    {
        post_event((char*)&color,1);
        lastColor = color;
        if(color != BLACK)
        {
            colorQueue.push(color);
        }
    }

}



