#include <ESP32Servo.h>
#include <L298NX2.h>
#include <TurtleReceiver.h>

NetController controller;

Servo carriageServo;
const int carriageServoPin = 17;

// Door servo (hinge, opens/closes 90 degrees)
Servo doorServo;
const int doorServoPin = 25;

// Motor driver pins
const unsigned int enablePin = 14;

const unsigned int rackPinionDirection1_C = 27;
const unsigned int rackPinionDirection2_C = 26;
L298N R_P(enablePin, rackPinionDirection1_C, rackPinionDirection2_C);

const unsigned int left1_C = 23;
const unsigned int left2_C = 22;
L298N LeftMotor(enablePin, left1_C, left2_C);

const unsigned int right1_C = 19;
const unsigned int right2_C = 18;
L298N RightMotor(enablePin, right1_C, right2_C);

// Servo positions
const int servoGrabPos = 45;
const int servoReleasePos = 120;
const int doorClosedPos = 0;
const int doorOpenPos = 90;   // swap these two if it opens the wrong way

// Pseudo-PWM for the rack and pinion
const unsigned long pwmPeriod = 20;  // ms per cycle (50 Hz)
const unsigned long onTime = 12;     // ms ON per cycle (60% duty)

// Drive stick deadzone
const float deadzone = 0.05f;

// Drives one drive motor from a -1..1 value (on/off only, since all
// motors share one enable pin)
void driveMotor(L298N& motor, float output)
{
    if (output > deadzone) {
        motor.forward();
    } else if (output < -deadzone) {
        motor.backward();
    } else {
        motor.stop();
    }
}

void setup()
{
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);
    carriageServo.setPeriodHertz(50);
    carriageServo.attach(carriageServoPin, 500, 2400);

    doorServo.setPeriodHertz(50);
    doorServo.attach(doorServoPin, 500, 2400);
    doorServo.write(doorClosedPos);  // start closed

    controller.controllerSetup();
    Serial.begin(115200);
    printMacAddress(); // 80:f3:da:41:25:08
}

void loop()
{
    // ---- Drive: left stick Y = forward/back, right stick X = turn ----
    float forward = controller.getJoy1Y();
    float turn = controller.getJoy2X();
    float leftOutput = forward + turn;
    float rightOutput = forward - turn;

    driveMotor(LeftMotor, leftOutput);
    driveMotor(RightMotor, rightOutput);

    // ---- Grab servo: press LB to toggle grab/release (once per press) ----
    static bool wasLbPressed = false;
    static bool servoGrabbed = false;
    bool lbPressed = controller.getLb();

    if (lbPressed && !wasLbPressed) {
        servoGrabbed = !servoGrabbed;
        carriageServo.write(servoGrabbed ? servoGrabPos : servoReleasePos);
    }
    wasLbPressed = lbPressed;

    // ---- Door servo: press RB to toggle open/closed (once per press) ----
    static bool wasRbPressed = false;
    static bool doorOpen = false;
    bool rbPressed = controller.getRb();

    if (rbPressed && !wasRbPressed) {
        doorOpen = !doorOpen;
        doorServo.write(doorOpen ? doorOpenPos : doorClosedPos);
    }
    wasRbPressed = rbPressed;

    // ---- Carriage: hold LT = up, hold RT = down ----
    bool leftTrigger = controller.getLt();
    bool rightTrigger = controller.getRt();

    if (leftTrigger != rightTrigger) {  // exactly one trigger held
        unsigned long cyclePosition = millis() % pwmPeriod;
        if (cyclePosition < onTime) {
            if (leftTrigger) {
                R_P.forward();
            } else {
                R_P.backward();
            }
        } else {
            R_P.stop();
        }
    } else {
        R_P.stop();  // neither or both held
    }
}
