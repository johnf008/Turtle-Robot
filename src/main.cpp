#include <Arduino.h>
#include <ESP32Servo.h>
#include <L298NX2.h>

// Motor Driver #1 (DRIVE)
const unsigned int EN_A = 3;
const unsigned int IN1_A = 5;
const unsigned int IN2_A = 6;

const unsigned int IN1_B = 7;
const unsigned int IN2_B = 8;
const unsigned int EN_B = 9;

unsigned long delayA = 3000;
int directionA = L298N::FORWARD;

unsigned long delayB = 3000;
int directionB = L298N::FORWARD;

L298NX2 motors(EN_A, IN1_A, IN2_A, EN_B, IN1_B, IN2_B);

// Motor Driver #2 (RACK PINION)
const unsigned int EN_C = 10;
const unsigned int IN1_C = 12;
const unsigned int IN2_C = 13;

const unsigned int IN1_D = 14;
const unsigned int IN2_D = 16;
const unsigned int EN_D = 17;

unsigned long delayC = 1000;
int directionC = L298N::FORWARD;

unsigned long delayD = 3000;
int directionD = L298N::FORWARD;

L298NX2 randp(EN_C, IN1_C, IN2_C, EN_D, IN1_D, IN2_D);

// Servo
Servo claw;
const int SERVO_PIN = 10; // Placeholder for pin later

int pos = 0; // variable to store the servo position
// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200); // setup serial monitor with rate of 115200 bits per second
  claw.setPeriodHertz(50);
  claw.attach(SERVO_PIN, 1000, 2000);

  motors.setSpeedA(80);
  motors.setSpeedB(80);

  randp.setSpeedA(80);
  randp.setSpeedB(80);
}

void loop()
{
  claw.setPeriodHertz(50); // set PWM period, gotten from datasheet
}

// Testing to see the range of the servo, could simplify with a loop
void clawOpenClose()
{
  claw.write(0);
  delay(3000);
  claw.write(60);
  delay(3000);
  claw.write(120);
  delay(3000);
  claw.write(180);
}

void moveForward()
{
  motors.runForA(delayA, L298N::FORWARD, callbackA);
  motors.runForB(delayB, L298N::FORWARD, callbackB);
}

void moveBackward()
{
  motors.runForA(delayA, L298N::BACKWARD, callbackA);
  motors.runForB(delayB, L298N::BACKWARD, callbackB);
}

void lift()
{
  randp.runForA(delayC, L298N::FORWARD, callbackC);
}

void lower()
{
  randp.runForA(delayC, L298N::BACKWARD, callbackC);
}

// Step variable can be used within each callback function to move in different directions if joysticks are used
void callbackA()
{
}

void callbackB()
{
}

void callbackC()
{
}

void callbackD()
{
}