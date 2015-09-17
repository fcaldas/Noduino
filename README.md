# Noduino

Noduino implements an http server using the Arduino ethernet shield and can easily work with the JSON 
format to monitor and control an arduino.

And it is as easy as that:
```c++
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

void callbackFunction(EthernetClient *client, char *args){
    Serial.println("GET called!!");
    client->println("{\"callback\":1}");
}

void postFunction(EthernetClient *client, char *args){
    Serial.println("Post called!!!!!!");
    Serial.println(args);
    client->println(args);
}

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  myServer = new restServer(mac, ip, gateway, subnet,80);
  myServer->addRoute("/index.php", GET, &callbackFunction);
  myServer->addRoute("/setButton", POST, &postFunction);
  Serial.println("Starting API");
}

void loop() {
  myServer->serve();
}

```

To use this library all you need is to copy this code to your .ino main file
and the following files to your project directory:

[restapi.h](https://github.com/fcaldas/Noduino/blob/master/restapi.h)

[vector.h](https://github.com/fcaldas/Noduino/blob/master/vector.h)

[routing.h](https://github.com/fcaldas/Noduino/blob/master/routing.h)


You can add new routes to your server using ```myServer->addRoute()```.
Callback functions always have the format: ```void request(EthernetClient *client, char *args)```
And args is the json sent to the file on POST requests, for the moment only GET and POST request are supported.
