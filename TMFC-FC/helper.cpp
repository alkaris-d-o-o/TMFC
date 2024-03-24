#include "helper.h"

// int armed = 0;

/*
 *  Functions used in Helper Class
 */
int Helper::armingState(long ch1, long ch3){
  if(ch3 < 1050 && ch1 < 1050 ) this->armed = 1;
  if(this->armed == 1 && ch3 < 1050 && ch1 > 1450) this->armed = 2;
  if(this->armed == 2 && ch3 < 1050 && ch1 > 1950) this->armed = 0;

  return this->armed;
}