
#define ROUTESIZE        20
#define MAX_REQUEST_SIZE 80


//IO Pins used for switching
#define P_ALIM          0
#define P_HDMI          1
#define P_COMRF         2
#define P_USB           3
#define P_RJ45          4


//Following inputs are analog ins
//Peritel
#define P_AUDIO         A0
#define P_AUDIOL        A1
#define P_AUDIOR        A2

#define P_RCA           A3

#define P_RFIN1         A4
#define P_RFIN2         A5

#define SERVO_PIN 13

#define INVALID_QUERY "{\"error\":1, \"message\":\"Invalid query\"}"
#define VALID_QUERY "{\"error\":0, \"message\":\"Query executed\"}"

void initIO() {
  pinMode(P_ALIM, OUTPUT);
  pinMode(P_HDMI, OUTPUT);
  pinMode(P_COMRF, OUTPUT);
  pinMode(P_USB, OUTPUT);
  pinMode(P_USB, OUTPUT);
  pinMode(P_RJ45, OUTPUT);
}
