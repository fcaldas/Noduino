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
#include "restapi.h"
#include "jsonParser.h"
#include "timeinter.h"
#include "zapTime.h"

#define LEDPIN 2

#define BUTTON1 6
#define BUTTON2 7

//Network configuration for arduino
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = {10, 0, 2, 160};
byte gateway[] = { 10, 0, 0, 1 };
byte subnet[] = { 255, 255, 0, 0 };
//byte server[] = { 10,0,2,158 };
byte server[] = { 10,0,1,25};
restServer *myServer;
EthernetClient http_client;

void getVideo(EthernetClient *client, char args[]){
  int v = analogRead(SENSORPIN);
  client->print("{\"video\":");
  client->print(v);
  client->println("}"); 
}


void echo(EthernetClient *client, char args[]){
  char data[60];
  JSONParser::getChar(args, "\"text\"", data, 60);
  client->print(data);
}


void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  myServer = new restServer(mac, ip, gateway, subnet,80);
  delay(1000);
  myServer->addRoute("/video", GET, &getVideo);
  myServer->addRoute("/getlivestatus", POST, &getLiveStatus);
  myServer->addRoute("/echo", POST, &echo);
  //init pins
  pinMode(LEDPIN, OUTPUT);
  //interrupt every 50ms
  //won't start now!
  TimeInterruption::init(50000);
  
}

void loop() {
  myServer->serve();
}
