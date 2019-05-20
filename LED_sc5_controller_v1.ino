/*
  Switch statement

  Demonstrates the use of a switch statement. The switch statement allows you
  to choose from among a set of discrete values of a variable. It's like a
  series of if statements.

  To see this sketch in action, put the board and sensor in a well-lit room,
  open the Serial Monitor, and move your hand gradually down over the sensor.

  The circuit:
  - photoresistor from analog in 0 to +5V
  - 10K resistor from analog in 0 to ground

  created 1 Jul 2009
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/SwitchCase
*/

// these constants won't change. They are the lowest and highest readings you
// get from your sensor:
const int sensorMin = 0;      // sensor minimum, discovered through experiment
const int sensorMax = 1023;    // sensor maximum, discovered through experiment

// Temp, LED lighting setup
int led1 = 8;
int led2 = 9;
int led3 = 10;
int led4 = 11;
int led5 = 12;

int rm1 = 3;
int rm2 = 4;
int rm3 = 5;
int rm4 = 6;

int currentRange;
int range;

void setup() {
  // initialize serial communication:
  Serial.begin(9600);

  // LED pins output
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT);

  pinMode(rm1, OUTPUT);
  pinMode(rm2, OUTPUT);
  pinMode(rm3, OUTPUT);
  pinMode(rm4, OUTPUT);
}

void loop() {
  // read the sensor:
  int sensorReading = analogRead(A0);
  // map the sensor range to a range of four options:
  int range = map(sensorReading, sensorMin, sensorMax, 0, 5);

  Serial.print(currentRange);

  switch (range) {
    
    case 0:
      Serial.println("OFF");
      currentRange = range;
      digitalWrite(led1, LOW);
      digitalWrite(led2, LOW);
      digitalWrite(led3, LOW);
      digitalWrite(led4, LOW);
      digitalWrite(led5, LOW);
      allOff();
      break;    
    case 1:    // your hand is on the sensor
      Serial.println("Zen Mode");
      currentRange = range;
      digitalWrite(led1, HIGH);
      digitalWrite(led2, LOW);
      digitalWrite(led3, LOW);
      digitalWrite(led4, LOW);
      digitalWrite(led5, LOW);
      allOff();
      break;
    case 2:    // your hand is close to the sensor
      Serial.println("Study Mode");
      currentRange = range;
      digitalWrite(led1, HIGH);
      digitalWrite(led2, HIGH);
      digitalWrite(led3, LOW);
      digitalWrite(led4, LOW);
      digitalWrite(led5, LOW);
      res1 ();
      break;
    case 3:    // your hand is a few inches from the sensor
      Serial.println("Classroom Mode");
      currentRange = range;
      digitalWrite(led1, HIGH);
      digitalWrite(led2, HIGH);
      digitalWrite(led3, HIGH);
      digitalWrite(led4, LOW);
      digitalWrite(led5, LOW);
      res2 ();
      break;
    case 4:    // your hand is nowhere near the sensor
      Serial.println("Party Mode");
      currentRange = range;
      digitalWrite(led1, HIGH);
      digitalWrite(led2, HIGH);
      digitalWrite(led3, HIGH);
      digitalWrite(led4, HIGH);
      digitalWrite(led5, LOW);
      res3 ();
      break;
    case 5:
      Serial.println("Rock concert");
      currentRange = range;
      digitalWrite(led1, HIGH);
      digitalWrite(led2, HIGH);
      digitalWrite(led3, HIGH);
      digitalWrite(led4, HIGH);
      digitalWrite(led5, HIGH);
      res4 ();
  }
  delay(5);        // delay in between reads for stability
}

void res1()
{
  delay(500);
  Serial.println("Room A");
  digitalWrite(rm1, HIGH);
  digitalWrite(rm2, LOW);
  digitalWrite(rm3, LOW);
  digitalWrite(rm4, LOW);
  if (currentRange != range)
  {
    loop();
  }
}

void res2()
{
  delay(500);
  Serial.println("Room B");
  digitalWrite(rm1, LOW);
  digitalWrite(rm2, HIGH);
  digitalWrite(rm3, LOW);
  digitalWrite(rm4, LOW);
  if (currentRange != range)
  {
    loop();
  }
}

void res3()
{
  delay(500);
  Serial.println("Room C");
  digitalWrite(rm1, LOW);
  digitalWrite(rm2, LOW);
  digitalWrite(rm3, HIGH);
  digitalWrite(rm4, LOW);
  if (currentRange != range)
  {
    loop();
  }
}

void res4()
{
  delay(500);
  Serial.println("Room D");
  digitalWrite(rm1, LOW);
  digitalWrite(rm2, LOW);
  digitalWrite(rm3, LOW);
  digitalWrite(rm4, HIGH);
  if (currentRange != range)
  {
    loop();
  }
}

void allOff()
{
  delay(500);
  Serial.println("No Room");
  digitalWrite(rm1, LOW);
  digitalWrite(rm2, LOW);
  digitalWrite(rm3, LOW);
  digitalWrite(rm4, LOW);
  if (currentRange != range)
  {
    loop();
  }
}
