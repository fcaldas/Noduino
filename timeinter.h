/*
 * Time interruptions using Timer1.h library to measure input
 * User to measure zapping/turn on time.
 */


class TimeInterruption{

public:
  static unsigned long nTimes;
  static unsigned long period;
  static bool finished;
  /*
   * Initialize timer with period in uS.
   */
  static void init(unsigned long period){
    Timer1.initialize(period);
    TimeInterruption::period = period;
  }

  static void callback(){
    TimeInterruption::nTimes++;
    //define stop condition!
    if(digitalRead(6) == 1){
      Timer1.detachInterrupt();
      TimeInterruption::finished = true;
    }
  }

  static void startCount(){
    TimeInterruption::nTimes = 0;
    TimeInterruption::finished = false;
    Timer1.attachInterrupt(callback, TimeInterruption::period);
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
