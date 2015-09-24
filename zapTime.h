#define SLAVESENSOR A5
#define MASTERSENSOR A4

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

#define lightSensorPin MASTERSENSOR

struct dataZap{
  bool isZapping;
  unsigned long nFramesLow;
  unsigned int nFramesHigh;
};

struct __livestatus{
  IMG_MODE status;
  int  delta;
  float average;
  float derivate;
};


/*
 * Analyses the TV state for twait ms
 * and returns a __livestatus object
 */
__livestatus getTVStatus(int twait){
  unsigned int dt = twait;
  __livestatus rstatus;
  rstatus.delta = twait;
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
    v1 = analogRead(MASTERSENSOR); //master
    v2 = analogRead(SLAVESENSOR);
    if(i != 0){
      maxDeriv1 = (abs(oldv1 - v1) > maxDeriv1)?abs(oldv1 - v1):maxDeriv1;
      maxDeriv2 = (abs(oldv2 - v2) > maxDeriv2)?abs(oldv2 - v2):maxDeriv2;
      maxV1 = (v1 > maxV1)?v1:maxV1;
      maxV2 = (v2 > maxV2)?v2:maxV2;
      minV1 = (v1 < minV1)?v1:minV1;
      minV2 = (v2 < minV2)?v2:minV2;
      deriv1 += abs(oldv1 - v1);
      deriv2 += abs(oldv2 - v2);
    }else{
      minV1 = v1;
      maxV1 = v1;
      minV2 = v2;
      maxV2 = v2;
    }
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
  Serial.println("yo");
  Serial.println(sum1);
  Serial.println(sum2);
  Serial.println(deriv1);
  Serial.println(deriv2);
  Serial.println(maxV2 - minV2);
  Serial.println(maxV1 - minV1);
  
  if(deriv1 <= 1.2 && sum1 <= 35 && deriv2 <= 1.2 && sum2 <= 35){
    rstatus.status = BLACK;
  }else if(deriv1 <= 2.0 && maxDeriv1 <= 2 && 
           deriv2 <= 2.0 && maxDeriv2 <= 2 &&
           maxV1 - minV1 < 3 && maxV2 - minV2 < 3){
    rstatus.status = FREEZE;
  }else{
    rstatus.status = LIVE;
  }
  rstatus.average = (sum1 + sum2)/2;
  rstatus.derivate = (deriv1 + deriv2)/2;
  return rstatus;
}


void decoderState(EthernetClient *client, char args[]){
  unsigned int dt = JSONParser::getInt(args, "\"dt\"");
  //this will be used as a circular buffer with size = 10
  if(dt>10000){
    client->println("{\"error\":1,\"message\":\"Max time exceeded\"}");
    return;
  }
  __livestatus rstatus = getTVStatus(dt);
  
  client->print("{\"state\":");
  if(rstatus.status == BLACK)
    client->print("\"BLACK\"");
  else if(rstatus.status == LIVE)
    client->print("\"LIVE\"");
  else if(rstatus.status == FREEZE)
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
  //number of 100ms intervals
  int nPlages = dt / 100;
  vector<__livestatus> vstatus;
  vstatus.reserve(nPlages);
  
  for(int i = 0; i < nPlages; i++){
    vstatus[i] = getTVStatus(100);
  }

  
  //merge multiple intervals
  bool merged = true;
  int i = 0;
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
  char str[10];
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

