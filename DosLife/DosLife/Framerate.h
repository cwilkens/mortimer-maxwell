#ifndef FRAMERATE_H
#define FRAMERATE_H

#include <inttypes.h>

struct timer_type		 					// Create A Structure For The Timer Information
{
  int64_t       frequency;					// Timer Frequency
  float         resolution;					// Timer Resolution
  unsigned long mm_timer_start;				// Multimedia Timer Start Value
  unsigned long mm_timer_elapsed;			// Multimedia Timer Elapsed Time
  bool			performance_timer;			// Using The Performance Timer?
  int64_t       performance_timer_start;	// Performance Timer Start Value
  int64_t       performance_timer_elapsed;	// Performance Timer Elapsed Time
};

void TimerInit();
float TimerGetTime();

#endif /* FRAMERATE_H */
