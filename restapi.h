/*
 * Rest API Server for Arduino
 * 
 * Filipe C - fcaldas@canal-plus.fr
 */

#include "vector.h"
#include "routing.h" 

class restServer{
private:
  EthernetServer *server;
  vector<__route> routes;

  /*  Searches for an specifical route on the routing table
      Params:
        routename : Requested route
        rtype : Requested method (GET / POST)
   */
  __route *getRoute(String routename, REQUEST_TYPE rtype){
    for(short i = 0; i < routes.size(); i++){
      if(routes[i].routename == routename &&
         routes[i].rtype == rtype)
         return &(routes[i]);
    }
    return NULL;
  }

  /*
   * Send HTTP Json app header
   */
  void sendHeader(EthernetClient *client){
    client->println(F("HTTP/1.1 200 OK"));
    client->println(F("Content-Type: application/json"));
    client->println(F("Connection: close"));
    client->println(F(""));  
  }
  
  /*
   * Send a 404 error message to client
   */
  void send404(EthernetClient *client){
    sendHeader(client);
    client->println(F("{\"message\":\"Route not found\",\"error\":{\"status\":404}}"));
  }

  /*Receives a request and looks for a callback method to reply*/
  void reply(String routename, REQUEST_TYPE rtype, 
             String &json, EthernetClient *client){
     __route* route = getRoute(routename, rtype);
     if(route){
        sendHeader(client);
        route->callback(client, &json);
     }else{
        Serial.write("No route for ");
        Serial.println(routename);
        send404(client); 
     }
  }


  __request * getRequest(EthernetClient &client){
      __request *req = new __request();
      bool readingFirst = true;
      String firstLine, page;
     
      while (client.connected() && readingFirst) {
        if (client.available()) {
          char c = client.read();
          
          firstLine += c;
  
          //decode first line of the request
          if(c == '\n'){
            int start;
            if(firstLine.substring(0,3) == "GET"){
              req->rtype = GET;
              start = 4;
            }else if(firstLine.substring(0,4) == "POST"){
              req->rtype = POST;
              start = 5;
            }
            for(start; start < firstLine.length(); start++){
              if(firstLine[start] == ' ' || firstLine[start] == '\r'){
                readingFirst = false;
                break;
              }
              page += firstLine[start];
            }
          }
        }
     }
     req->routename = page;
     return req;
  }

  /*
   * Return the content of a POST message
   */
  String getPostContent(EthernetClient &client){
     String data = "";
     unsigned short nBraces = 0;
     bool finished = false;
     while (client.connected() && !finished ) {
        if (client.available()) {
          char c = client.read();
          data += c;
          if(c == '\n'){
            if(data == "\r\n"){
              finished = true;
            }else{
              data = "";
            }
          }
        }
     }
     data = "";
     Serial.write("on new line of post request!");
     finished = false;
     while (client.connected() && !finished ) {
        if (client.available()) {
          char c = client.read();
          if(c != '\r' && c != '\n' && c != ' '){
            data += c;
            if(c == '{'){
              nBraces++;
            }else if( c == '}'){
              nBraces--;
              if(nBraces == 0)
                finished = true;
            }
          }
        }
     }
     
     return data;  
  }
  
public:
  restServer(byte mac[6], byte ip[4], byte gway[4], byte subnet[4], int port){
    Ethernet.begin(mac, ip, gway, subnet);
    server = new EthernetServer(port);
    server->begin();
  }  

  void addRoute(String name, REQUEST_TYPE req, void (* callback)(EthernetClient *client, String *args)){
    routes.push_back(__route(name, req, callback));
  }

  void serve() {
    EthernetClient client = server->available();
    String json;
    
    if (client) {
      Serial.write("New client!");
      //get header of request
      __request * req = getRequest(client);
      
      if(req->rtype == POST){
        json = getPostContent(client);
        reply(req->routename, req->rtype, json, &client);
      }else{
        reply(req->routename, req->rtype, json, &client);
      }
      free(req);
      delay(1);
      client.stop();
      Serial.println("client disconnected");

      Ethernet.maintain();
      
    }
  }
  
};
