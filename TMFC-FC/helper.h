#include <Wire.h>
#ifndef helper_h
#define helper_h


class Helper{
  protected: 
    /*
     *  This variable has 3 states:
     *    0 - Disarmed
     *    1 - Mid position, wait for state to be changed
     *    2 - Armed
     */
    int armed = 0;
  public:
    int armingState(long ch1, long ch3);
    bool isArmed(){ return this->armed == 2; }

    
};

#endif