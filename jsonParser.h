/*
 * Primitive JSON parser for arduino
 * 
 * Parsing functions: 
 * 
 *   -getInt(json, key)
 *      
 *      Returns int value for key:int pair
 */

class JSONParser{
public:
  static bool failure;
  
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

//  static void getChar(char json[], char key[], char bufer[], int bufsize){
//    JSONParser::failure = false;
//    char *posKey = strstr(json,key);
//    int value = 0;
//    bool isNeg = false;
//    if(posKey == NULL){
//      JSONParser::failure = true;
//      return -1;
//    }
//    //get initial position of value
//    short pos = posKey - json + 1 + strlen(key);
//    short initialPos;
//    if(json[pos] == '"'){
//      initialPos = ++pos;
//    }else{
//      JSONParser::failure = true;
//      return -1;
//    }
//    bool wasBackslash = false;
//    for(pos; pos < strlen(json); pos++){
//      if(wasBackslash){
//        
//      }else{
//        if  
//      }
//    }
//
//    
//    while(json[pos] - '0' <= 9 && json[pos] - '0' >= 0){
//      value = value * 10;
//      value = value + (json[pos] - '0');
//      pos++;
//    }
//    if(isNeg)
//      return -1 * value;
//    return value;
//  }

};

bool JSONParser::failure;
