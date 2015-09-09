/* Arduino Test Bank Controller
 * 
 * This program exposes an REST API to multiple switches and inputs
 * on a Test Bank.
 * 
 * Filipe C.
 */
 
#include <SPI.h>
#include <EthernetUdp.h>
#include <EthernetServer.h>
#include <Dhcp.h>
#include <Ethernet.h>
#include "restapi.h"
#include "jsonParser.h"

#define LEDPIN 2

//Network configuration for arduino
byte mac[] = {0x33, 0xAA, 0xDE, 0xAD, 0xC0, 0xD4}; 
byte ip[] = {10, 0, 2, 160};
byte gateway[] = { 10, 0, 0, 1 };
byte subnet[] = { 255, 255, 0, 0 };
restServer *myServer;

void callbackFunction(EthernetClient *client, char args[]){
    client->println("{\"callback\":1}");
}

void postFunction(EthernetClient *client, char args[]){
    client->println(args);
}

void ledFunc(EthernetClient *client, char args[]){
    int v = JSONParse::getInt(args, "\"ledOn\"");
    Serial.write("Led set status:");  
    Serial.println(v);  
    if(v == 1)
      digitalWrite(LEDPIN, HIGH);
    else if(v == 0)
      digitalWrite(LEDPIN, LOW);
    client->println("{\"success\":1}");
}

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  myServer = new restServer(mac, ip, gateway, subnet,80);
  myServer->addRoute("/index.php", GET, &callbackFunction);
  myServer->addRoute("/post.php", POST, &postFunction);
  myServer->addRoute("/led", POST, &ledFunc);
  Serial.println("Starting API");
  //init pins
  pinMode(LEDPIN, OUTPUT);
}

void loop() {
  myServer->serve();
}



