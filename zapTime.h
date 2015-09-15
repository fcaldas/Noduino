

/*
 * Callback function called by timer interruption every 50ms
 * to check if zap has started / finished
 * 
 * The data structure will be stored in TimeInterruption::data void pointer
 */

#define lightSensorPin A4

struct dataZap{
  bool isZapping;
  unsigned long nFramesLow;
  unsigned int nFramesHigh;
};

void zapTCallback(){
  dataZap *data = (dataZap *) TimeInterruption::data;
  int value = analogRead(lightSensorPin);
  Serial.println(value);
  //will search for a high luminosity level
  if(data->isZapping){
      if(value < 20){
        data->nFramesLow++;
        data->nFramesHigh = 0;
      }else{
        data->nFramesHigh++;
        Serial.println("Cnt High");
      }
  }else{
      if(value < 20){
        data->nFramesLow++;
      }else{
        data->nFramesLow = 0;
      }
      if(data->nFramesLow > 3){
        data->isZapping = true;
        Serial.println("set ZAP");
      }
  }
  //finished zapping!
  if(data->nFramesHigh > 3){
    TimeInterruption::removeInterruption();
    TimeInterruption::nTimes = data->nFramesLow;
    
  }
}

