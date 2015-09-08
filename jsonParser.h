/*
 * Primitive JSON parser for arduino
 */

class JSONParse{
public:
  static bool failure;
  
  static int getInt(char json[], char key[]){
    JSONParse::failure = false;
    char *posKey = strstr(json,key);
    int value = 0;
    bool isNeg = false;
    if(posKey == NULL){
      JSONParse::failure = true;
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
      Serial.println(json[pos]);
      value = value * 10;
      value = value + (json[pos] - '0');
      pos++;
    }
    if(isNeg)
      return -1 * value;
    return value;
  }

};

bool JSONParse::failure;
