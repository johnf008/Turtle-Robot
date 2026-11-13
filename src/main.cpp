#include <ESP32Servo.h>
#include <L298NX2.h>
#include <TurtleReceiver.h>

NetController controller;

Servo carriageservo;

#if defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)
constexpr int servoPin = 17;
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
constexpr int servoPin = 7;
#else
constexpr int servoPin = 18;
#endif

constexpr unsigned int EN_C = 14;
constexpr unsigned int IN1_C = 27;
constexpr unsigned int IN2_C = 26;

constexpr unsigned int EN_LEFT = 25;
constexpr unsigned int IN1_LEFT = 33;
constexpr unsigned int IN2_LEFT = 32;

constexpr unsigned int EN_RIGHT = 23;
constexpr unsigned int IN1_RIGHT = 21;
constexpr unsigned int IN2_RIGHT = 22;

L298N carriagemotor(EN_C, IN1_C, IN2_C);
L298N leftmotor(EN_LEFT, IN1_LEFT, IN2_LEFT);
L298N rightmotor(EN_RIGHT, IN1_RIGHT, IN2_RIGHT);

constexpr unsigned short carriageSpeed = 100;
constexpr unsigned short leftMotorSpeed = 100;
constexpr unsigned short rightMotorSpeed = 100;

void setDriveMotor(L298N& motor, float output, unsigned short maxSpeed)
{
    constexpr float deadzone = 0.05f;

    if (output > 1.0f) {
        output = 1.0f;
    } else if (output < -1.0f) {
        output = -1.0f;
    }

    if (output > deadzone) {
        motor.setSpeed(static_cast<unsigned short>(maxSpeed * output));
        motor.forward();
    } else if (output < -deadzone) {
        motor.setSpeed(static_cast<unsigned short>(maxSpeed * -output));
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
    carriageservo.setPeriodHertz(50);
    carriageservo.attach(servoPin, 950, 1550);

    carriagemotor.setSpeed(carriageSpeed);
    leftmotor.setSpeed(leftMotorSpeed);
    rightmotor.setSpeed(rightMotorSpeed);
    controller.controllerSetup();
    Serial.begin(115200);
    printMacAddress();
}

void loop()
{
    // Left stick drives; right stick steers.
    float forward = controller.getJoy1Y();
    float turn = controller.getJoy2X();
    float leftOutput = forward + turn;
    float rightOutput = forward - turn;

    float leftMagnitude = leftOutput < 0 ? -leftOutput : leftOutput;
    float rightMagnitude = rightOutput < 0 ? -rightOutput : rightOutput;
    float maxMagnitude = leftMagnitude > rightMagnitude ? leftMagnitude : rightMagnitude;
    if (maxMagnitude > 1.0f) {
        leftOutput /= maxMagnitude;
        rightOutput /= maxMagnitude;
    }

    setDriveMotor(leftmotor, leftOutput, leftMotorSpeed);
    setDriveMotor(rightmotor, rightOutput, rightMotorSpeed);

    // CONTROLLER TOP
    static bool wasLbPressed = false;
    static bool wasRbPressed = false;
    bool lbPressed = controller.getLb();
    bool rbPressed = controller.getRb();
    if (lbPressed && !wasLbPressed) {
        // left bumper pressed
        // servo moves to grab block
        for (int pos = 0; pos <= 180; pos += 30) {
            carriageservo.write(pos);
            delay(3);
        }
    }
    if (rbPressed && !wasRbPressed) {
        // right bumper pressed
        // servo moves to release block
        for (int pos = 180; pos >= 0; pos -= 30) {
            carriageservo.write(pos);
            delay(3);
        }
    }
    wasLbPressed = lbPressed;
    wasRbPressed = rbPressed;
    // prevents the code from running multiple times if the bumper is held down, only runs once when the bumper is pressed

    bool leftTrigger = controller.getLt();
    bool rightTrigger = controller.getRt();
    if (leftTrigger && !rightTrigger) {
        carriagemotor.forward();
    } else if (rightTrigger && !leftTrigger) {
        carriagemotor.backward();
    } else {
        carriagemotor.stop();
    }

    // UNUSED JOYSTICK BUTTONS

    /*else if(controller.getJoy1Z() == true){
        // left joystick button pressed
    }*/
    
    /*else if(controller.getJoy2Y() > 0){
        // right joystick held up
    }
    else if(controller.getJoy2Y() < 0){
        // right joystick held down
    }
    else if(controller.getJoy2Z() == true){
        // right joystick button pressed
    }*/
    /*if(controller.getJoy1X() > 0){
        // left joystick held right
    }
    else if(controller.getJoy1X() < 0){
        // left joystick held left
    }*/

    // BUTTONS (unused)
    /*if (controller.getButtonA() == true){
        // button A pressed (south)
    }
    if (controller.getButtonB() == true){
        // button B pressed (east)
    }
    if (controller.getButtonX() == true){
        // button X pressed (west)
    }
    if (controller.getButtonY() == true){
        // button Y pressed (north)
    }
    if (controller.getDpad() == true){
        // dpad pressed
    }*/

    
    /*
    carriagemotor.setSpeed(255);
    carriagemotor.forward();
    delay(750);

    carriagemotor.setSpeed(carriageSpeed);
    carriagemotor.backward();
    delay(750);

    carriagemotor.stop();*/
}
