
#define ROUTESIZE        20
#define MAX_REQUEST_SIZE 80


//IO Pins used for switching
#define P_7             7
#define P_POWER         6
#define P_4             4
#define P_3             3
#define P_2             2
#define P_SERVO         5
Servo servoCard;

//Following inputs are analog ins
//for the Peritel plug
#define P_AUDIOL        A0
#define P_AUDIOR        A1
#define P_VIDEO         A2
#define P_SWITCHS       A3
#define P_SWITCHF       0

#define P_RFIN1         A4
#define P_RFIN2         A5

#define INVALID_QUERY "{\"error\":1, \"message\":\"Invalid query\"}"
#define VALID_QUERY "{\"error\":0, \"message\":\"Query executed\"}"

void initIO() {
  pinMode(P_7, OUTPUT);
  pinMode(P_POWER, OUTPUT);
  pinMode(P_4, OUTPUT);
  pinMode(P_3, OUTPUT);
  pinMode(P_2, OUTPUT);
  servoCard.attach(P_SERVO);
  digitalWrite(P_POWER, HIGH);
}
