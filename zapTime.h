enum IMG_MODE {
  BLACK,
  LIVE,
  FREEZE
};

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
  TimeInterruption::nTimes++;
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
    //TimeInterruption::nTimes = data->nFramesLow;
    
  }
}


void getLiveStatus(EthernetClient *client, char args[]){
  unsigned int dt = JSONParser::getInt(args, "\"dt\"");
  //this will be used as a circular buffer with size = 20
  int data[10];
  char str[10];
  unsigned int nloop = dt / 10;
  float sum = 0;

  float derivMoyenne = 0;
  int maxDeriv = 0;
  Serial.println("Call live status");
  
  vector<IMG_MODE> imgMode;
  vector<unsigned int> finalMode;
  vector<unsigned int> deltas;
  vector<float> averages, derivates;
  //collect data and classify!
  bool first = true;

  //values used on final json
  for(int i = 0; i <= nloop; i++){
    //classify last buffer!
    if(i%10 == 0 && first == false){
      sum /= 10;
      derivMoyenne /= 10;
      deltas.push_back(100); //100 ms
      averages.push_back(sum);
//      derivates.push_back(derivMoyenne);
      if(derivMoyenne <= 1.2 && sum <= 25){
        imgMode.push_back(BLACK);
      }else if(derivMoyenne <= 1.0 && maxDeriv < 1.5){
        imgMode.push_back(FREEZE);
      }else{
        imgMode.push_back(LIVE);
      }
      derivMoyenne = 0;
      maxDeriv = 0;
      sum = 0;
    }
    first = false;
    data[i%10] = analogRead(SENSORPIN);
    sum += data[i%10];
    if(i%10 != 0){
      derivMoyenne += abs(data[i%10] - data[(i-1)%10]);
      maxDeriv = (abs(data[i%10] - data[(i-1)%10])?abs(data[i%10] - data[(i-1)%10]):maxDeriv);
    }
    delay(10);
  }
  //merge multiple status
  int i = 0;
  bool merged = true;
  while(merged){
     merged = false;
     while(i < imgMode.size() - 1){
      //if both are live flows merge
       if(imgMode[i] == LIVE && imgMode[i+1] == LIVE){
          averages[i] = (averages[i] * deltas[i] + averages[i+1] * deltas[i+1])/(deltas[i]+deltas[i+1]);
          deltas[i] += deltas[i+1];
          deltas.remove(i+1);
          imgMode.remove(i+1);
          averages.remove(i+1);
          merged = true;
       }else if( (imgMode[i] == LIVE && imgMode[i+1] == FREEZE) ||
                 (imgMode[i] == FREEZE && imgMode[i+1] == LIVE)){
          averages[i] = (averages[i] * deltas[i] + averages[i+1] * deltas[i+1])/(deltas[i]+deltas[i+1]);
          imgMode[i] = LIVE;
          deltas[i] += deltas[i+1];
          deltas.remove(i+1);
          imgMode.remove(i+1);
          averages.remove(i+1);
          merged = true;
          
       }else if(imgMode[i] == FREEZE && imgMode[i+1] == FREEZE){
          if(abs(averages[i] - averages[i+1]) > 2)
            imgMode[i] == LIVE; 
          averages[i] = (averages[i] * deltas[i] + averages[i+1] * deltas[i+1])/(deltas[i]+deltas[i+1]);
          deltas[i] += deltas[i+1];
          deltas.remove(i+1);
          imgMode.remove(i+1);
          averages.remove(i+1);
          merged = true;
       }else if(imgMode[i] == BLACK && imgMode[i+1] == BLACK){
          averages[i] = (averages[i] * deltas[i] + averages[i+1] * deltas[i+1])/(deltas[i]+deltas[i+1]);
          deltas[i] += deltas[i+1];
          deltas.remove(i+1);
          imgMode.remove(i+1);
          averages.remove(i+1);
          merged = true;
       }else
          i++;
     }
  }

  //create JSON
  client->print("{");
  client->print("\"result\":[");
  for(int i = 0; i < imgMode.size(); i++){
    client->print("{\"plage\":");    
    client->print(deltas[i]);    
    client->print(",\"status\":");
    if(imgMode[i] == BLACK)
      client->print("\"BLACK\",");
    else if(imgMode[i] == LIVE)
      client->print("\"LIVE\",");
    else if(imgMode[i] == FREEZE)
      client->print("\"FREEZE\",");      
    client->print("\"avgvalue\":");
    dtostrf(averages[i], 1, 2, str);
    client->print(str);
    client->print("}");
    if(i != imgMode.size() - 1)
      client->print(",");
    }
  client->print("]}");
}

