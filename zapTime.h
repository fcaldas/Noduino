enum IMG_MODE {
  BLACK,
  LIVE,
  FREEZE
};

/*
 * Callback function called by timer interruption every 50ms
 * to check if zap has started / finished
 * 
 * The vstatus structure will be stored in TimeInterruption::vstatus void pointer
 */

#define lightSensorPin A4

struct dataZap{
  bool isZapping;
  unsigned long nFramesLow;
  unsigned int nFramesHigh;
};

void zapTCallback(){
  dataZap *vstatus = (dataZap *) TimeInterruption::data;
  int value = analogRead(lightSensorPin);
  TimeInterruption::nTimes++;
  //will search for a high luminosity level
  if(vstatus->isZapping){
      if(value < 20){
        vstatus->nFramesLow++;
        vstatus->nFramesHigh = 0;
      }else{
        vstatus->nFramesHigh++;
        Serial.println("Cnt High");
      }
  }else{
      if(value < 20){
        vstatus->nFramesLow++;
      }else{
        vstatus->nFramesLow = 0;
      }
      if(vstatus->nFramesLow > 3){
        vstatus->isZapping = true;
        Serial.println("set ZAP");
      }
  }
  //finished zapping!
  if(vstatus->nFramesHigh > 3){
    TimeInterruption::removeInterruption();
    //TimeInterruption::nTimes = vstatus->nFramesLow;
    
  }
}

struct __livestatus{
  IMG_MODE status;
  int  delta;
  float average;
  float derivate;
};


void decoderState(EthernetClient *client, char args[]){
  unsigned int dt = JSONParser::getInt(args, "\"dt\"");
  //this will be used as a circular buffer with size = 10
  if(dt>10000){
    client->println("{\"error\":1,\"message\":\"Max time exceeded\"}");
    return;
  }
  unsigned int nloop = dt / 10;
  float sum1 = 0;
  float sum2 = 0;
  int maxDeriv1 = 0;
  int maxDeriv2 = 0;
  int v1 = 0, v2 = 0;
  int oldv1 = 0, oldv2 = 0;
  float deriv1 = 0, deriv2 = 0;
  int minV1 = 0, minV2 = 0, maxV1 = 0, maxV2 = 0;
  for(unsigned int i = 0; i <= nloop; i++){
    v1 = analogRead(A4); //master
    
    v2 = analogRead(A5);
    if(i != 0){
      maxDeriv1 = (abs(oldv1 - v1) > maxDeriv1)?abs(oldv1 - v1):maxDeriv1;
      maxDeriv2 = (abs(oldv2 - v2) > maxDeriv2)?abs(oldv2 - v2):maxDeriv2;
      minV1 = (v1 < minV1)?v1:minV1;
      minV2 = (v2 < minV2)?v2:minV2;
    }else{
      minV1 = v1;
      maxV1 = v1;
      minV2 = v2;
      maxV2 = v2;
    }
    deriv1 += abs(oldv1 - v1);
    deriv2 += abs(oldv2 - v2);
    sum1 += v1;
    sum2 += v2;
    oldv1 = v1;
    oldv2 = v2;
    delay(10);
  }
  sum1 /= nloop;
  sum2 /= nloop;
  deriv1 /= nloop;
  deriv2 /= nloop;
  IMG_MODE state;
  if(deriv1 <= 1.2 && sum1 <= 35 && deriv2 <= 1.2 && sum2 <= 35){
    state = BLACK;
  }else if(deriv1 <= 2.0 && maxDeriv1 <= 2 && 
           deriv2 <= 2.0 && maxDeriv2 <= 2 &&
           maxV1 - minV1 < 3 && maxV2 - minV2 < 3){
    state = FREEZE;
  }else{
    state = LIVE;
  }
  client->print("{\"state\":");
  if(state == BLACK)
    client->print("\"BLACK\"");
  else if(state == LIVE)
    client->print("\"LIVE\"");
  else if(state == FREEZE)
    client->print("\"FREEZE\"");      
  client->println("}");
}


void getLiveStatus(EthernetClient *client, char args[]){
  
  unsigned int dt = JSONParser::getInt(args, "\"dt\"");
  //this will be used as a circular buffer with size = 10
  if(dt>6000){
    client->println("{\"error\":1,\"message\":\"Max time exceeded\"}");
    return;
  }
  int data[10];
  char str[10];
  unsigned int nloop = dt / 10;
  float sum = 0;

  float derivMoyenne = 0;
  int maxDeriv = 0;

  vector<__livestatus> vstatus;
  //collect vstatus and classify!
  Serial.println(nloop/10);
  vstatus.reserve(nloop/10);
  bool first = true;
  //values used on final json
  for(int i = 0; i <= nloop; i++){
    //classify last buffer!
    if(i%10 == 0 && first == false){
      Serial.println(i/10 - 1);
      sum /= 10;
      derivMoyenne /= 10;
      vstatus[i/10 - 1].delta = 100;
      vstatus[i/10 - 1].average = sum;
      vstatus[i/10 - 1].derivate = derivMoyenne;

      if(derivMoyenne <= 1.2 && sum <= 35){
        vstatus[i/10 - 1].status = BLACK;
      }else if(derivMoyenne <= 1.1 && maxDeriv < 1.5){
        vstatus[i/10 - 1].status = FREEZE;
      }else{
        vstatus[i/10 - 1].status = LIVE;
      }
      derivMoyenne = 0;
      maxDeriv = 0;
      sum = 0;
    }
    first = false;
    long v = analogRead(A4);
    v += analogRead(A5);
    data[i%10] = v/2;
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
     Serial.println(vstatus.size());
     while(i < vstatus.size() - 1){
      //if both are live flows merge
       if(vstatus[i].status == LIVE && vstatus[i+1].status == LIVE){
          vstatus[i].average = (vstatus[i].average * vstatus[i].delta + vstatus[i+1].average * vstatus[i+1].delta)/(vstatus[i].delta + vstatus[i+1].delta);
          vstatus[i].delta += vstatus[i+1].delta;
          vstatus.remove(i+1);
          merged = true;
       }else if( (vstatus[i].status == LIVE && vstatus[i+1].status == FREEZE) ||
                 (vstatus[i].status == FREEZE && vstatus[i+1].status == LIVE)){
          vstatus[i].average = (vstatus[i].average * vstatus[i].delta + vstatus[i+1].average * vstatus[i+1].delta)/(vstatus[i].delta+vstatus[i+1].delta);
          vstatus[i].status = LIVE;
          vstatus[i].delta += vstatus[i+1].delta;
          vstatus.remove(i+1);
          merged = true;
       }else if(vstatus[i].status == FREEZE && vstatus[i+1].status == FREEZE){
          if(abs(vstatus[i].average - vstatus[i+1].average) > 2)
            vstatus[i].status == LIVE; 
          vstatus[i].average = (vstatus[i].average * vstatus[i].delta + vstatus[i+1].average * vstatus[i+1].delta)/(vstatus[i].delta+vstatus[i+1].delta);
          vstatus[i].delta += vstatus[i+1].delta;
          vstatus.remove(i+1);
          merged = true;
       }else if(vstatus[i].status == BLACK && vstatus[i+1].status == BLACK){
          vstatus[i].average = (vstatus[i].average * vstatus[i].delta + vstatus[i+1].average * vstatus[i+1].delta)/(vstatus[i].delta+vstatus[i+1].delta);
          vstatus[i].delta += vstatus[i+1].delta;
          vstatus.remove(i+1);
          merged = true;
       }else
          i++;
     }
  }
  Serial.println("Sending data to client");
  //create JSON
  client->print("{");
  client->print("\"result\":[");
  for(int i = 0; i < vstatus.size(); i++){
    client->print("{\"plage\":");    
    client->print(vstatus[i].delta);    
    client->print(",\"status\":");
    if(vstatus[i].status == BLACK)
      client->print("\"BLACK\",");
    else if(vstatus[i].status == LIVE)
      client->print("\"LIVE\",");
    else if(vstatus[i].status == FREEZE)
      client->print("\"FREEZE\",");      
    client->print("\"avgvalue\":");
    dtostrf(vstatus[i].average, 1, 2, str);
    client->print(str);
    client->print(",\"avgderiv\":");
    dtostrf(vstatus[i].derivate, 1, 2, str);
    client->print(str);
    client->print("}");
    if(i != vstatus.size() - 1)
      client->print(",");
  }
  client->print("]}");
}

