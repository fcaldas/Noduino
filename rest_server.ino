/* Arduino Test Bank Controller
 * 
 * This program exposes an REST API to multiple switches and inputs
 * on a Test Bank.
 * 
 * Filipe C.
 */

#include <TimerOne.h>
#include <SPI.h>
#include <EthernetUdp.h>
#include <EthernetServer.h>
#include <Dhcp.h>
#include <Ethernet.h>
#include "restapi.h"
#include "jsonParser.h"
#include "audioCap.h"
#include "timeinter.h"

#define LEDPIN 2
#define SENSORPIN A0
#define BUTTON1 6
#define BUTTON2 7

//Network configuration for arduino
byte mac[] = {0x33, 0xAA, 0xDE, 0xAD, 0xC0, 0xD4}; 
byte ip[] = {10, 0, 2, 160};
byte gateway[] = { 10, 0, 0, 1 };
byte subnet[] = { 255, 255, 0, 0 };
restServer *myServer;

void readButtons(EthernetClient *client, char args[]){
    bool button1 = digitalRead(BUTTON1);
    bool button2 = digitalRead(BUTTON2);
    char retStr[40];
    sprintf(retStr,"{\"b1\":%d,\"b2\":%d}",button1, button2);
    client->println(retStr);    
}

void setOn(EthernetClient *client, char args[]){
  int v = JSONParse::getInt(args, "\"time\"");
  digitalWrite(LEDPIN, HIGH);
  if(v == 1){
    TimeInterruption::startCount();
    Serial.println("Starting timer!");    
    TimeInterruption::wait();
    client->print("{\"done\":1,\"ms\":");
    char buf[20];
    TimeInterruption::getMs(buf);
    client->print(buf);
    client->print("}");
    
  }else{
    client->println("{\"done\":1}"); 
  }
}

void setOff(EthernetClient *client, char args[]){
  digitalWrite(LEDPIN, LOW);
  client->println("{\"done\":1}"); 
}

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  myServer = new restServer(mac, ip, gateway, subnet,80);
  myServer->addRoute("/buttons", GET, &readButtons);
  myServer->addRoute("/setOn", POST, &setOn);
  myServer->addRoute("/setOff", GET, &setOff);
  Serial.println("Starting API");
  //init pins
  pinMode(LEDPIN, OUTPUT);
  pinMode(BUTTON1, INPUT);
  pinMode(BUTTON2, INPUT);
  //interrupt every 100ms
  TimeInterruption::init(50000);
}

void loop() {
  myServer->serve();
}



