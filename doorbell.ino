#include <Servo.h>

#define FADETIME 20 //time before the light begin to turn off after a press (sec)
#define OPENTIME 10 //time you have to open the door

Servo myservo;  // create servo object to control a servo

const int sonnette = 5; //bell button
const int OPEN = 8; //opening the door (inside switch)
const int alim = 3; //transistor TIP120 or solid relay 5V for the servo
const int light = 11; //TIP120 for the LED
const int soleno = 10; //TIP120 solenoid for the door
const int LED = 7; //Signal LED green/red
const int closeSwitch = 4; //switch that tests if the door is closed

byte fade = 0; //PWM state of the LED

boolean bolLight = LOW; //light ON or OFF?
boolean bolOPEN = false; //door and signal LED state
boolean respi = true; //respiration pattern for the light

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
  pinMode(sonnette, INPUT_PULLUP);
  pinMode(OPEN, INPUT_PULLUP);
  pinMode(closeSwitch, INPUT_PULLUP);
  pinMode(soleno, OUTPUT);
  pinMode(LED, OUTPUT);

  myservo.write(58);              // tell servo to go to position 58...forever!
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

  if (digitalRead(OPEN) == LOW) {
    //opening the door
    timerOPEN = millis();
    bolOPEN = true;
  }
  if (millis() > timerOPEN + (OPENTIME * 1000) && closedCounter > 90) {
    //closing the door if closeSwitch comes activated for 90 loops
    bolOPEN = false;
    //bolLight = LOW;
  }

  if (digitalRead(closeSwitch) == LOW) {
    ++closedCounter;
  }
  else
    closedCounter = 0;

  if (digitalRead(sonnette) == LOW) {
    bellRot = HIGH;
    bolLight = HIGH; //passer un bolean en mode allumage de lumiere
    timerPress = millis();
  }

  if (bolLight == HIGH) {
    //light ON
    if (bolOPEN == false) {
      //door not open, respiration pattern light
      if (fade > 100) {
        respi = false;
      }
      if (fade < 10) {
        respi = true;
      }
      if (respi == true) {
        ++fade;
        //delay(2 / (fade + 1));
      }
      if (respi == false) {
        --fade;
        delay(3 / (fade + 1));
      }
    }
  }

  if (bolOPEN == true) {
    if (fade < 255) {
      ++fade;
      delay(2 / (fade + 1));
    }
  }


  if (millis() > timerPress + (FADETIME * 1000) && fade >= 1 && bolOPEN == false) {
    //light OFF
    bolLight = LOW;
    respi = false;
    --fade;
    delay(45 / (fade + 1));
  }



}
