
#ifdef USE_TIMER

#define BUF_DIMENSION 200

int BUFFER[BUF_DIMENSION];
short bufcounter = 0;

//not implemented. 
//

#else


void captureAudio(EthernetClient *client, char args[]){
    short counter = 0;
    int buffer[200];
    char data[12];

    //following loop takes 220ms to read 200 values
    for(counter = 0; counter < 200 ; counter++){
      buffer[counter] =  analogRead(A0);
      delay(1);
    }
    client->print("{\"values\":[");
    counter = 0;
    
    while(counter < 200){
      client->print(itoa(buffer[counter], data, 10));
      counter++;
      if(counter != 200)
        client->print(",");
    }
    
    client->print("]}");

}


#endif
