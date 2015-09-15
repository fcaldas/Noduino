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
#include "zapTime.h"

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

void getVideo(EthernetClient *client, char args[]){
  int v = analogRead(A4);
  client->print("{\"video\":");
  client->print(v);
  client->println("}"); 
}

void getZap(EthernetClient *client, char args[]){
  dataZap dZap;
  dZap.isZapping = false;
  dZap.nFramesLow = 0;
  dZap.nFramesHigh = 0;
  TimeInterruption::data = (void *) &dZap;
  Serial.println("Launching interruption Cback");
  TimeInterruption::startCount(&zapTCallback);
  TimeInterruption::wait();
  client->print("{\"done\":1,\"ms\":");
  char buf[20];
  TimeInterruption::getMs(buf);
  client->print(buf);
  client->print("}");
}

void setup() {
  Serial.begin(19200);           // set up Serial library at 9600 bps
  myServer = new restServer(mac, ip, gateway, subnet,80);
  myServer->addRoute("/buttons", GET, &readButtons);
  myServer->addRoute("/video", GET, &getVideo);
  myServer->addRoute("/zaptime", GET, &getZap);
  Serial.println("Starting API");
  //init pins
  pinMode(LEDPIN, OUTPUT);
  pinMode(BUTTON1, INPUT);
  pinMode(BUTTON2, INPUT);
  //interrupt every 50ms
  //wont start now!
  TimeInterruption::init(50000);
}

void loop() {
  myServer->serve();
}
