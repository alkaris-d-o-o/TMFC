#include "helper.h"

/*
 *  This variable has 3 states:
 *    0 - Disarmed
 *    1 - Mid position, wait for state to be changed
 *    2 - Armed
 */
int armed = 0;

/*
 *  Functions used in Helper Class
 */
int Helper::armingState(long ch1, long ch3){
  if(ch3 < 1050 && ch1 < 1050 ) armed = 1;
  if(armed == 1 && ch3 < 1050 && ch1 > 1450) armed = 2;
  if(armed == 2 && ch3 < 1050 && ch1 > 1950) armed = 0;

  return armed;
}