//CAB 1.1.2022
//collide.ino collide.cpp
//robot collide or halt behavior, it drives forward and stops at an obstacle

#include <Arduino.h>                // include Arduino library
#include <wpi-32u4-lib.h>           // include WPI robot library
#include <Rangefinder.h>            // include sonar range finder
#include <Chassis.h>                // define robot chassis
Rangefinder rangefinder(11, 4);     // create instance of sonar echo pin 11, trigger pin 4
Chassis chassis(7.0, 1440, 14.9);   // Declares a chassis object with nominal dimensions
#define LED_PIN 13                  // LED connected to builtin pin 13
#define red_LED 0                   // red LED on pin 5
#define grn_LED 12                   // green LED on pin 
int robotSpeed = 10;                //set robot sp6eed
float distance;                     //sonar distance in cm
float inches;                       //sonar distance in inches
int stopDistance = 6;               //obstacle stop distance

enum ROBOT_STATE {ROBOT_IDLE, ROBOT_WANDER,ROBOT_AVOID}; // Define the robot states
ROBOT_STATE robotState = ROBOT_IDLE;  // initialze robt in wander state


void setLED(bool value)             //function to turn on and off LED on pin 13
{
  //Serial.println("setLED()");
  digitalWrite(LED_PIN, value);
}

void readSonar(){                         //read ultrasonic range sensor
  distance = rangefinder.getDistance();   //distance in cm
  inches = distance*0.393701;             //distance in inches
}

void idle(void)                         //helper function to stop the motors
{
  Serial.println("idle()");         //print idle to serial monitor
  setLED(LOW);                      // turn off LED on pin 13
  chassis.idle();                   //stop motors 
  //robotState = ROBOT_IDLE;        //set state to idle
}


void drive(float dist, float speed)   //helper function to drive a set distance
{
  Serial.println("drive()");              //print drive to serial monitor
  setLED(HIGH);                           // turn on LED on pin 13
  //chassis.setWheelSpeeds(speed, speed);   //MOVES WHEELS FOREVER
  chassis.driveFor(dist,speed);         //MOVES FOR A CERTAIN DISTANCE
}

void turn(float ang, float speed)     //helper function to turn a set angle
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
//try changing the robot speed, random number seed and modulo
void randomWander(){      //function to move robot random forward and turn
  digitalWrite(red_LED,LOW);
  digitalWrite(grn_LED,HIGH);
  Serial.println("randomWander()");
  int randNumber = random(millis());    //generate a random number
  int distance = randNumber % 50;       //limit the values between 0 and 50 cm
  int angle = randNumber % 120;          //limit the values between 0 and 120 degrees
  Serial.print(distance);
  Serial.println(" deg\t");
  Serial.print(angle);
  Serial.print(" cm\t");
  if(randNumber > 0) drive(distance, robotSpeed);     //only move forward to test halt behavior
  else turn(-randNumber % 120, 30);      //sit still to test avoid behavior
  // delay(1000);
}

//TO DO: Modify the Halt behavior to stop at a given distance when an obstacle is detected.
void Collide(){
  digitalWrite(red_LED,HIGH);
  digitalWrite(grn_LED,LOW);
  idle();     //stop the robot
  //TO DO: add code to turn away from obstacle and continue moving
  turnUntilDone(90,5*robotSpeed);
}

void avoidObstacle(){
    Collide();  //Robot collide behavior to top before hitting an obstacle.
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
  //digitalWrite(red_LED,HIGH);         //test RED LED
  digitalWrite(grn_LED,HIGH);         //test GREEN LED
  delay(5000);                        //insert delay to get robot off test stand before moving on floor
  robotState = ROBOT_WANDER;          //change robot state to random wander
  digitalWrite(red_LED,LOW);         //test RED LED
  digitalWrite(grn_LED,LOW);         //test GREEN LED
  
  Serial.println("To infinity and beyond but don't hit anything! ");
}

//runc continuously on the microcontroller
void loop() 
{
  readSonar();
  //delay(100);                                  //read code and sonar recharge delay

  if (inches<stopDistance){                    //check for osbtacles
    robotState = ROBOT_AVOID;
  }

  // A basic state machine
  switch(robotState)
  {
    case ROBOT_IDLE: 
       if(chassis.checkMotionComplete()) handleMotionComplete(); 
       break;
    case ROBOT_WANDER: 
        if(chassis.checkMotionComplete()) randomWander();
        robotState = ROBOT_WANDER;
       break;
    case ROBOT_AVOID: 
        avoidObstacle();
        robotState = ROBOT_WANDER;
       break;
    default:
      break;
  }
}