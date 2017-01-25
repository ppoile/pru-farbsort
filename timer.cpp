#include "timer.h"

#include <stdint.h>
#include <pru_iep.h>
#include <pru_intc.h>
#include "command_interface.h"

volatile register uint32_t __R31;


namespace {
  const uint32_t COUNTER_INCREMENTS_PER_SECOND_AT_200MHZ = 200e6;
  const uint32_t TICKS_PER_SECOND = 100;

  uint32_t ticks = 0;
}

void timer_start()
{
  /* Disable counter */
  CT_IEP.TMR_GLB_CFG_bit.CNT_EN = 0;

  /* Reset Count register */
  CT_IEP.TMR_CNT = 0xFFFFFFFF;

  /* Clear overflow status register */
  CT_IEP.TMR_GLB_STS_bit.CNT_OVF = 1;

  /* Clear compare status */
  CT_IEP.TMR_CMP_STS_bit.CMP_HIT = 0xFF;

  /* Set compare value */
  CT_IEP.TMR_CMP0 = COUNTER_INCREMENTS_PER_SECOND_AT_200MHZ / TICKS_PER_SECOND;

  /* Enable CMP0 and reset on event */
  CT_IEP.TMR_CMP_CFG_bit.CMP0_RST_CNT_EN = 1;
  CT_IEP.TMR_CMP_CFG_bit.CMP_EN = 1;

  /* Configure default increment */
  CT_IEP.TMR_GLB_CFG_bit.DEFAULT_INC = 1;

  /* Disable compensation */
  CT_IEP.TMR_COMPEN_bit.COMPEN_CNT = 0;

  /* Clear the status of all interrupts */
  CT_INTC.SECR0 = 0xFFFFFFFF;
  CT_INTC.SECR1 = 0xFFFFFFFF;

  /* Enable counter */
  CT_IEP.TMR_GLB_CFG_bit.CNT_EN = 1;
}

void timer_poll()
{
  if (timer_elapsed()) {
    ticks += 1;
    CT_IEP.TMR_CMP_STS_bit.CMP_HIT = 0x01;
  }
}

bool timer_elapsed()
{
  return !(!(CT_IEP.TMR_CMP_STS_bit.CMP_HIT & 0x01));
}

void timer_stop()
{
  /* Disable counter */
  CT_IEP.TMR_GLB_CFG_bit.CNT_EN = 0;

  /* Disable Compare0 */
  CT_IEP.TMR_CMP_CFG = 0x0;

  /* Clear Compare status */
  CT_IEP.TMR_CMP_STS = 0xFF;

  /* Clear the status of the interrupt */
  CT_INTC.SECR0 = (1 << 7);
}

uint32_t timer_get_ticks()
{
  return ticks;
}


Timer::Timer()
{
    for (int i = 0; i < TIMER_MAX_SCHEDULES; i++)
    {
        registration[i].ticks = 0; // mark as free
    }
}


void Timer::schedule(CommandInterface *command, int ticks)
{
    for (int i = 0; i < TIMER_MAX_SCHEDULES; i++)
    {
        if(registration[i].ticks == 0)
        {
            registration[i].command = command;
            registration[i].ticks = ticks;
            break;
        }
        else
        {
            // @todo error handling, what to do when too many registrations
        }
    }

}

void Timer::poll()
{
    int currentTimer = ticks;
    int diff = currentTimer - lastTimer;
    for (int i = 0; i < TIMER_MAX_SCHEDULES; i++)
    {
        if(registration[i].ticks > diff)
        {
            registration[i].ticks -= diff;
        }
        else if (registration[i].ticks > 0)
        {
            registration[i].ticks = 0;
            registration[i].command->execute();
        }
    }
    lastTimer = currentTimer;
}


