/*
 * Rest API Server for Arduino - Routing classes
 * 
 * Filipe C - fcaldas@canal-plus.fr
 */


enum REQUEST_TYPE {
  GET,
  POST
};

class __request{
public:
  String routename;
  REQUEST_TYPE rtype;
};


class __route{

public:
  String routename;
  REQUEST_TYPE rtype;
  void (*callback)(EthernetClient *client, String *args);

  __route(String rname, REQUEST_TYPE req, 
          void (*callbackf)(EthernetClient *client, String *args) ){
    callback = callbackf;
    routename = rname;
    rtype = req;
  }
  __route(){}
  
};

