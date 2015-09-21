/*
 * Primitive JSON parser for arduino
 * 
 * Parsing functions: 
 * 
 */

class JSONParser{
public:
  static bool failure;

  /* getInt()
   * 
   *   Reads an integer from a json buffer and returns it
   *   
   *   {"value" : 123}
   */
  static int getInt(char json[], char key[]){
    JSONParser::failure = false;
    char *posKey = strstr(json,key);
    int value = 0;
    bool isNeg = false;
    if(posKey == NULL){
      JSONParser::failure = true;
      return -1;
    }
    //get initial position of value
    short pos = posKey - json + 1 + strlen(key);
    if(json[pos] == '+'){
      pos++;
    }else if(json[pos] == '-'){
      isNeg = true;
      pos++;
    }
    while(json[pos] - '0' <= 9 && json[pos] - '0' >= 0){
      value = value * 10;
      value = value + (json[pos] - '0');
      pos++;
    }
    if(isNeg)
      return -1 * value;
    return value;
  }

  /* getChar()
   * 
   *   Reads an string from a json buffer to a char array
   *   
   *   {"data" : "hello"}
   */
  static void getChar(char json[], char key[], char dbuffer[], int bufsize){
    JSONParser::failure = false;
    char *posKey = strstr(json,key);
    int value = 0;
    bool isNeg = false;
    if(posKey == NULL){
      JSONParser::failure = true;
      return;
    }
    //get initial position of value
    short pos = posKey - json + 1 + strlen(key);
    short initialPos, finalPos;
    if(json[pos] == '"'){
      initialPos = ++pos;
    }else{
      JSONParser::failure = true;
      return;
    }
    bool wasBackslash = false;
    for(pos; pos < strlen(json); pos++){
      if(wasBackslash){
        wasBackslash = false;
      }else{
        if(json[pos] == '\\')
          wasBackslash = true;
        else if(json[pos] == '"'){
          finalPos = pos;
          break;
        }
      }
    }
    short n = finalPos - initialPos;
    n = (n>bufsize)?bufsize:n;
    strncpy(dbuffer, &(json[initialPos]), n);
    dbuffer[n] = 0x00;
    return;
  }

  /* getIP()
   * 
   *   Reads an IP from a json buffer to a unsigned byte [4]
   *   
   *   {"ip" : "192.168.2.1"}
   */
  static void getIP(char json[], char key[], byte addr[] ){
    JSONParser::failure = false;
    char ipString[20];
    JSONParser::getChar(json, key, ipString, 20);
    if(JSONParser::failure)
      return;
    short byteNow = 0;
    addr[0] = 0x00;
    addr[1] = 0x00;
    addr[2] = 0x00;
    addr[3] = 0x00;
    for(short i = 0; i < strlen(ipString); i++){
      if(ipString[i] == '.'){
        byteNow++;
        continue;
      }
      addr[byteNow] = addr[byteNow]*10 + (ipString[i] - '0');
    }
  }

};

bool JSONParser::failure;
