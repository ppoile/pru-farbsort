#include "scheduled_output_action.h"

ScheduledOutputAction::ScheduledOutputAction()
  : timestamp((uint32_t)-1)
  , bitmask(1)
  , value(0)
{}

ScheduledOutputAction::ScheduledOutputAction(uint32_t timestamp, uint32_t bitmask, bool value)
  : timestamp(timestamp)
  , bitmask(bitmask)
  , value(value)
{}
