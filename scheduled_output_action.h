#include <stdint.h>


struct ScheduledOutputAction
{
  uint32_t timestamp;
  uint32_t bitmask;
  bool value;

  ScheduledOutputAction();
  ScheduledOutputAction(uint32_t timestamp, uint32_t bitmask, bool value);
};
