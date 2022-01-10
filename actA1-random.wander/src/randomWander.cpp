//CAB 1.1.2022
//randomWander.cpp
//generate a random number to move the robot
//random wander, go to goal, subsumption architecture

#include <Arduino.h>                // include Arduino library
#include <wpi-32u4-lib.h>           // include WPI robot library
#include <Rangefinder.h>            // include sonar range finder
#include <Chassis.h>                // define robot chassis
Rangefinder rangefinder(11, 4);     // create instance of sonar echo pin 11, trigger pin 4
Chassis chassis(7.0, 1440, 14.9);   // Declares a chassis object with nominal dimensions
#define LED_PIN 13                  // LED connected to pin 13

enum ROBOT_STATE {ROBOT_IDLE, ROBOT_WANDER}; // Define the robot states
ROBOT_STATE robotState = ROBOT_IDLE;            // initialze robt in wander state
int robotSpeed = 10;                        //set robot speed

void setLED(bool value)             //function to turn on and off LED on pin 13
{
  //Serial.println("setLED()");
  digitalWrite(LED_PIN, value);
}

void idle(void)     //A helper function to stop the motors
{
  Serial.println("idle()");
  setLED(LOW);
  chassis.idle();             //stop motors 
  //robotState = ROBOT_IDLE;    //set state to idle
}


void drive(float dist, float speed) // A helper function to drive a set distance
{
  Serial.println("drive()");
  setLED(HIGH);
  //chassis.setWheelSpeeds(speed, speed);   //MOVES WHEELS FOREVER
  chassis.driveFor(dist,speed);         //MOVES FOR A CERTAIN DISTANCE
}

void turn(float ang, float speed) // A helper function to turn a set angle
{
  Serial.println("turn()");
  setLED(HIGH);
  //chassis.setWheelSpeeds(speed, speed);
  chassis.turnFor(ang,speed);
}

void driveUntilDone(float dist, float speed) // A helper function to drive a set distance
{
  Serial.println("drive()");
  setLED(HIGH);
  chassis.driveFor(dist,speed);         //MOVES FOR A CERTAIN DISTANCE
  while(!chassis.checkMotionComplete()) {delay(1);}
}

void turnUntilDone(float ang, float speed) // A helper function to turn a set angle
{
  Serial.println("turn()");
  setLED(HIGH);
  chassis.turnFor(ang,speed);
  while(!chassis.checkMotionComplete()) {delay(1);}
}

//TO DO: Modify this function to make a unique random wander routine
//try changing the random number seed and modulo
void randomWander(){      //function to move robot random forward and turn
  Serial.println("randomWander()");
  int randNumber = random(millis());    //generate a random number
  int distance = randNumber % 50;       //limit the values between 0 and 50 cm
  int angle = randNumber % 90;          //limit the values between 0 and 90 degrees
  Serial.print(distance);
  Serial.println(" deg\t");
  Serial.print(angle);
  Serial.print(" cm\t");
  driveUntilDone(distance,robotSpeed);
  turnUntilDone(angle,robotSpeed);
}

// Used to check if the motions above are complete
void handleMotionComplete(void)
{
  idle();
}

// run this once at beginning
void setup() 
{
  Serial.begin(115200);               //begin serial communication for debuggint
  chassis.init();                     //initialize the chassis (which also initializes the motors)
  chassis.setMotorPIDcoeffs(5, 0.5);  //PID controller for driving robot chassis
  rangefinder.init();                 // Call init() to set up the rangefinder
  delay(5000);                        //insert delay to get robot off test stand before moving on floor
  robotState = ROBOT_WANDER;          //change robot state to random wander
  Serial.println("To infinity and beyond but don't hit anything! ");
}

//runc continuously on the microcontroller
void loop() 
{
  float distance = rangefinder.getDistance();   //distance in cm
  float inches = distance*0.393701;             //distance in inches
  delay(200);                                  //read code and sonar recharge delay
  // Serial.print(millis());
  // Serial.print('\t');
  // Serial.print(distance); 
  // Serial.print(" cm\t");
  // Serial.print(inches); 
  // Serial.print(" in\n");
  // A basic state machine
  switch(robotState)
  {
    case ROBOT_IDLE: 
       if(chassis.checkMotionComplete()) handleMotionComplete(); 
       break;
    case ROBOT_WANDER: 
        randomWander();
       break;
    default:
      break;
  }
}