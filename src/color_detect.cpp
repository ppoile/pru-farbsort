#include <stdint.h>
#include "color_detect.h"
#include "adc.h"
#include "timer.h"
#include "hw.h"
#include "rpmsg_tx_interface.h"
#include "msg_definition.h"

#ifdef ADC_LOGGING
extern uint8_t adc_values[200];
extern uint8_t adc_value_index;
#endif

namespace {
    uint16_t const adcRefBlack  = 1228;
    uint16_t const adcRefBlue   = 1187;
    uint16_t const adcRefRed    = 815;
    uint16_t const adcRefWhite  = 768;

    uint16_t const thresholdBlack  =  (adcRefBlack + adcRefBlue) / 2;
    uint16_t const thresholdBlue   =  (adcRefBlue + adcRefRed) / 2;
    uint16_t const thresholdRed    =  (adcRefRed + adcRefWhite) / 2;
}

ColorDetect::ColorDetect(Hw &hw, TimerInterface *timer, Queue<Color, COLOR_QUEUE_SIZE> &colorQueue, RpMsgTxInterface *rpmsg):
    adcOld(adcRefBlack), diffOld(0), lastColor(BLACK), hw(hw), timer(timer),  colorQueue(colorQueue), rpmsg(rpmsg)
{

}


void ColorDetect::execute()
{
    int16_t adc = hw.adc->read();
    int16_t diff = (adc - adcOld);
    int16_t diffDiff = diff - diffOld;

#ifdef ADC_LOGGING
    // temporary fill the adc values into buffer to query from linux
    adc_values[adc_value_index++] = adc & 0xff;
    adc_values[adc_value_index++] = (adc >> 8) & 0xff;
    if(adc_value_index >= 200)
    {
        adc_value_index = 0;
    }
#endif

    if(((diff > 0) && (diffOld <= 0)) && (diffDiff > 0))// minimum shortly missed?
    {
         evalColor(adcOld, true);
    }
    else if(((diff < 0) && (diffOld >=0)) && (diffDiff < 0)) // maximum shortly missed
    {
        evalColor(adcOld, false);
    }

    adcOld = adc;
    diffOld = diff;

    // register this function for periodic call
    timer->registerCommand(this,5);
}

void ColorDetect::evalColor(uint16_t adc, bool minimum)
{
    Color color;

    if(minimum)
    {
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
    }
    else
    {
        if(adc > thresholdBlack)
        {
            color = BLACK;
        }
        else
        {
            color = lastColor;
        }
    }

    if(color != lastColor) // color changed?
    {
        lastColor = color;
        if(color != BLACK)
        {
            colorQueue.push(color);
            rpmsg->post_info((INFO_COLOR_BLUE - 1) + color);
        }
    }

}



