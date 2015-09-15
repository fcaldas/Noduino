/*
 * Time interruptions using Timer1.h library to measure input
 * User to measure zapping/turn on time.
 * 
 * How to use:
 * 
 *    Call TimeInterruption::init(period_usec)
 *    Put any data you need in TimeInterruption::data void pointer
 *    call startCount(& function_to_callback())
 *    this function will be called everytime the counter > period
 *    
 *    When your periodic function finished you need to:
 *          Timer1.detachInterrupt();
 *          TimeInterruption::finished = true;
 *          
 *    You can also wait for a period function to finish with TimeInterruption::wait();
 */


class TimeInterruption{

public:
  static unsigned long nTimes;
  static unsigned long period;
  static bool finished;
  static void *data;
  /*
   * Initialize timer with period in uS.
   */
  static void init(unsigned long period){
    Timer1.initialize(period);
    TimeInterruption::period = period;
  }

  static void startCount(void (*callback)()){
    TimeInterruption::nTimes = 0;
    TimeInterruption::finished = false;
    Timer1.attachInterrupt(callback, TimeInterruption::period);
  }

  static void removeInterruption(){
      Timer1.detachInterrupt();
      TimeInterruption::finished = true;    
  }
  
  static void wait(){
    while(TimeInterruption::finished == false) {
      delay(10);
    }
  }

  static unsigned long getMs(char *c){
    sprintf(c, "%d", TimeInterruption::period/1000*TimeInterruption::nTimes);
  }
};

unsigned long TimeInterruption::nTimes;
unsigned long TimeInterruption::period;
bool TimeInterruption::finished;
void * TimeInterruption::data;
