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

//Network configuration for arduino
byte mac[] = {0x33, 0xAA, 0xDE, 0xAD, 0xC0, 0xD4}; 
byte ip[] = {10, 0, 2, 160};
byte gateway[] = { 10, 0, 0, 1 };
byte subnet[] = { 255, 255, 0, 0 };
restServer *myServer;

void callbackFunction(EthernetClient *client, String *args){
    Serial.println("GET called!!");
    client->println("{\"callback\":1}");
}

void postFunction(EthernetClient *client, String *args){
    Serial.println("Post called!!!!!!");
    client->println(*args);
}

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  myServer = new restServer(mac, ip, gateway, subnet,80);
  myServer->addRoute(F("/index.php"), GET, &callbackFunction);
  myServer->addRoute("/post.php", POST, &postFunction);
  Serial.println("Starting API");
}

void loop() {
  myServer->serve();
}



