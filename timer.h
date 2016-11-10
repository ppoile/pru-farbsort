#include <stdint.h>

void timer_start();
void timer_poll();
bool timer_elapsed();
void timer_stop();
uint32_t get_ticks();
