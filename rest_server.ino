/* 
 *  Arduino Test Bank Controller
 * 
 *  This program exposes an REST API to multiple switches and inputs
 *  on a Test Bank.
 * 
 *  Filipe Caldas
 */

#define SENSORPIN A4

#include <TimerOne.h>
#include <SPI.h>
#include <EthernetUdp.h>
#include <EthernetServer.h>
#include <Ethernet.h>
#include <Servo.h>
#include "restapi.h"
#include "jsonParser.h"
#include "timeinter.h"

//Network configuration for arduino
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = {10, 0, 2, 160};
byte gateway[] = { 10, 0, 0, 1 };
byte subnet[] = { 255, 255, 0, 0 };
//byte server[] = { 10,0,2,158 };
byte server[] = { 10,0,1,25};
restServer *myServer;
EthernetClient http_client;

void getAudio(EthernetClient *client, char args[]){
  int v1, v2;
  v1 = analogRead(P_AUDIOL);
  v2 = analogRead(P_AUDIOR);
  client->print("{\"audiol\":");
  client->print(v1);
  client->print(",\"audior\":");
  client->print(v2);
  client->println("}");  
}

void getVideo(EthernetClient *client, char args[]){
  int v1;
  v1 = analogRead(P_VIDEO);
  client->print("{\"video\":");
  client->print(v1);
  client->println("}");
}

void setCard(EthernetClient *client, char args[]){
  int v = JSONParser::getInt(args, "\"on\"");  
  if(JSONParser::failure){
    client->println(INVALID_QUERY);
  }else if(v == 0){
    servoCard.write(90);
    client->println(VALID_QUERY);
  }else if(v == 1){
    servoCard.write(0);
    client->println(VALID_QUERY);
  }else{
    client->println(INVALID_QUERY);
  }
}

void p7(EthernetClient *client, char args[]){
  int v = JSONParser::getInt(args, "\"on\"");  
  if(JSONParser::failure){
    client->println(INVALID_QUERY);
  }else if(v == 0){
    digitalWrite(P_7, LOW);
    client->println(VALID_QUERY);
  }else if(v == 1){
    digitalWrite(P_7, HIGH);
    client->println(VALID_QUERY);
  }else{
    client->println(INVALID_QUERY);
  }
}

void setPower(EthernetClient *client, char args[]){
  int v = JSONParser::getInt(args, "\"on\"");
  if(JSONParser::failure){
    client->println(INVALID_QUERY);
  }else if(v == 0){
    digitalWrite(P_POWER, HIGH);
    client->println(VALID_QUERY);
  }else if(v == 1){
    digitalWrite(P_POWER, LOW);
    client->println(VALID_QUERY);
  }else{
    client->println(INVALID_QUERY);
  }
}


void setUsb(EthernetClient *client, char args[]){
  int v = JSONParser::getInt(args, "\"on\"");
  if(JSONParser::failure){
    client->println(INVALID_QUERY);
  }else if(v == 0){
    digitalWrite(P_USB, HIGH);
    client->println(VALID_QUERY);
  }else if(v == 1){
    digitalWrite(P_USB, LOW);
    client->println(VALID_QUERY);
  }else{
    client->println(INVALID_QUERY);
  }
}

void setEth0(EthernetClient *client, char args[]){
  int v = JSONParser::getInt(args, "\"on\"");
  if(JSONParser::failure){
    client->println(INVALID_QUERY);
  }else if(v == 0){
    digitalWrite(P_ETH0, HIGH);
    client->println(VALID_QUERY);
  }else if(v == 1){
    digitalWrite(P_ETH0, LOW);
    client->println(VALID_QUERY);
  }else{
    client->println(INVALID_QUERY);
  }
}

void setP3(EthernetClient *client, char args[]){
  int v = JSONParser::getInt(args, "\"on\"");
  if(JSONParser::failure){
    client->println(INVALID_QUERY);
  }else if(v == 0){
    digitalWrite(P_3, HIGH);
    client->println(VALID_QUERY);
  }else if(v == 1){
    digitalWrite(P_3, LOW);
    client->println(VALID_QUERY);
  }else{
    client->println(INVALID_QUERY);
  }
}

void setP7(EthernetClient *client, char args[]){
  int v = JSONParser::getInt(args, "\"on\"");
  if(JSONParser::failure){
    client->println(INVALID_QUERY);
  }else if(v == 0){
    digitalWrite(P_7, HIGH);
    client->println(VALID_QUERY);
  }else if(v == 1){
    digitalWrite(P_7, LOW);
    client->println(VALID_QUERY);
  }else{
    client->println(INVALID_QUERY);
  }
}

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  myServer = new restServer(mac, ip, gateway, subnet,80);
  delay(1000);
  myServer->addRoute("/ethernet", POST, &setEth0);
  myServer->addRoute("/power", POST, &setPower);
  myServer->addRoute("/usb", POST, &setUsb);
  myServer->addRoute("/card", POST, &setCard);
  myServer->addRoute("/audio", GET, &getAudio);
  myServer->addRoute("/video", GET, &getVideo);
  myServer->addRoute("/p3", POST, &setP3);
  myServer->addRoute("/p7", POST, &setP7);
  Serial.println("Starting API");
  //init pins
  initIO();
}

void loop() {
//  digitalWrite(P_ALIM, HIGH);
  myServer->serve();
}
