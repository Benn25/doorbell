#include <Servo.h>

#define FADETIME 8 //time before the light begin to turn off after a press (sec)
#define OPENTIME 10 //time you have to open the door

Servo myservo;  // create servo object to control a servo

const int sonnette = 5; //la sonnette
const int OPEN = 8; //opening the door
const int alim = 3; //transistor OU relai solid sur du 5V poiur alim le servo
const int light = 11; //TIP120 for the LED
const int soleno = 10; //solenoid for the door
const int LED = 7; //Signal LED green/red
const int closeSwitch = 4; //switch that tests if the door is closed

byte fade = 0; //PWM state of the LED

boolean bolLight = LOW; //light ON or OFF?
boolean bolOPEN = false; //door and signal LED state

//int timerBell = 0;
unsigned long timerPress = 0; //time passed since the button was pressed
unsigned long timerOPEN = 0;
volatile int bellRot = LOW;

int closedCounter = 0;

void setup()
{

  Serial.begin(9600);
  attachInterrupt(0, STOPROT, RISING); //stop la rotation quand le switch RISE
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object

  pinMode(alim, OUTPUT); //sans dec
  pinMode(sonnette, INPUT);
  pinMode(OPEN, INPUT);
  pinMode(closeSwitch, INPUT);
  pinMode(soleno, OUTPUT);
  pinMode(LED, OUTPUT);

  myservo.write(105);              // tell servo to go to position 70...forever!
}

void STOPROT()
{
  bellRot = LOW;
}

void loop()
{
  Serial.print("etat fade= ");
  Serial.println(fade);
//  Serial.print("open= ");
//  Serial.println(bolOPEN);
//  Serial.print("etat du sw open= ");
//  Serial.println(digitalRead(OPEN));

  analogWrite(light, fade);
  digitalWrite(alim, bellRot);
  digitalWrite(soleno, bolOPEN);
  digitalWrite(LED, bolOPEN);

  if (digitalRead(OPEN) == HIGH) {
    //opening the door
    timerOPEN = millis();
    bolOPEN = true;
  }
  if (millis() > timerOPEN + (OPENTIME * 1000) && closedCounter > 50) {
    //closing the door if closeSwitch comes HIGH for 5 sec
    bolOPEN = false;
  }

  if (digitalRead(closeSwitch) == HIGH)
    ++closedCounter;
  else
    closedCounter = 0;

  if (digitalRead(sonnette) == HIGH) {
    bellRot = HIGH;
    bolLight = HIGH; //passer un bolean en mode allumage de lumiere
    timerPress = millis();
  }

  if (bolLight == HIGH && fade < 255) {
    //light ON
    ++fade;
    delay(15 / (fade+1));
  }

  if (millis() > timerPress + (FADETIME * 1000) && fade >= 1) {
    //light OFF
    bolLight = LOW;
    --fade;
    delay(25 / (fade+1));
  }




}
