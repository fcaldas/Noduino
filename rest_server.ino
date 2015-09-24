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
byte mac[] = { 0xDE , 0xAD, 0xBE, 0xEF, 0x61, 0x05};
byte ip[] = {10, 0, 2, 160};
byte gateway[] = { 10, 0, 0, 1 };
byte subnet[] = { 255, 255, 0, 0 };

restServer *myServer;
EthernetClient http_client;

void getVideo(EthernetClient *client, char args[]){
  int v1 = analogRead(A4);
  int v2 = analogRead(A5);
  client->print("{\"video1\":");
  client->print(v1);
  client->print(",\"video2\":");
  client->print(v2);
  client->println("}"); 
}


void echo(EthernetClient *client, char args[]){
  char data[60];
  JSONParser::getChar(args, "\"text\"", data, 60);
  client->print(data);
}

void dozap(EthernetClient *client, char args[]){
  EthernetClient zapclient;
  byte dec_ip[4];
  int channel;
  //get IP
  JSONParser::getIP(args, "\"ip_box\"", dec_ip );
  //get channel number
  channel = JSONParser::getInt(args, "\"channel\"");
  zapclient.stop();
  int code = zapclient.connect(dec_ip, 8080);
  Serial.println(code);
  Serial.println("channel");
  Serial.println(channel);
  Serial.println("IP");
  Serial.println(dec_ip[0]);
  Serial.println(dec_ip[1]);
  Serial.println(dec_ip[2]);
  Serial.println(dec_ip[3]);
  if (code) {
    Serial.println("connected");
    zapclient.println("POST /message HTTP/1.1");
    zapclient.print("Content-Length: ");
    zapclient.println(45 + channel/10);
    zapclient.println("Accept: */*");
    zapclient.println("Content-Type: application/json");
    zapclient.println("");
    zapclient.print("{\"action\":\"zap\",\"params\":{\"channelNumber\":");
    zapclient.print(channel);
    zapclient.println("}}");
    zapclient.flush();
  } else {
    Serial.println("connection failed");
  }
  client->println("{\"done\":1}");
}


void setup() {
  myServer = new restServer(mac, ip, gateway, subnet,80);
  Serial.begin(9600);           // set up Serial library at 9600 bps
  delay(1000);
  myServer->addRoute("/video", GET, &getVideo);
  myServer->addRoute("/getlivestatus", POST, &getLiveStatus);
  myServer->addRoute("/echo", POST, &echo);
  myServer->addRoute("/state", POST, &decoderState);
  myServer->addRoute("/zap", POST, &dozap);
  //init pins
  pinMode(LEDPIN, OUTPUT);
  //interrupt every 50ms
  //won't start now!
  TimeInterruption::init(50000);
}

void loop() {
  myServer->serve();
}
