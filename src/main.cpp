#include <ESP32Servo.h>
#include <L298NX2.h>
#include <TurtleReceiver.h>

NetController controller;

Servo myservo; // create servo object to control a servo
//  16 servo objects can be created on the ESP32

int pos = 0; // variable to store the servo position
// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33
// Possible PWM GPIO pins on the ESP32-S2: 0(used by on-board button),1-17,18(used by on-board LED),19-21,26,33-42
// Possible PWM GPIO pins on the ESP32-S3: 0(used by on-board button),1-21,35-45,47,48(used by on-board LED)
// Possible PWM GPIO pins on the ESP32-C3: 0(used by on-board button),1-7,8(used by on-board LED),9-10,18-21
#if defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)
int servoPin = 17;
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
int servoPin = 7;
#else
int servoPin = 18;
#endif

// Motor Driver #2 (RACK PINION)
const unsigned int EN_C = 14;
const unsigned int IN1_C = 27;
const unsigned int IN2_C = 26;

const unsigned int IN1_D = 14;
const unsigned int IN2_D = 16;
const unsigned int EN_D = 17;

unsigned long delayC = 2000;
int directionC = L298N::FORWARD;

unsigned long delayD = 5;
int directionD = L298N::FORWARD;

L298N randp(EN_C, IN1_C, IN2_C);

// initial speed
unsigned short speed = 200;

void setup()
{
    // Allow allocation of all timers
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);
    myservo.setPeriodHertz(50);          // standard 50 hz servo
    myservo.attach(servoPin, 950, 1550); // attaches the servo on pin 18 to the servo object, the following min and max values work to hold the stack: (1050, 1550)

    // using default min/max of 1000us and 2000us
    // different servos may require different min/max settings
    // for an accurate 0 to 180 sweep
    randp.setSpeed(speed);
    controller.controllerSetup();
    Serial.begin(115200);
    printMacAddress();
}

void loop()
{

    for (pos = 0; pos <= 180; pos += 30)
    { // goes from 0 degrees to 180 degrees
        // in steps of 1 degree
        myservo.write(pos); // tell servo to go to position in variable 'pos'
        delay(3);           // waits 15ms for the servo to reach the position
    }
    delay(3000);
    for (pos = 180; pos >= 0; pos -= 30)
    {                       // goes from 180 degrees to 0 degrees
        myservo.write(pos); // tell servo to go to position in variable 'pos'
        delay(3);           // waits 15ms for the servo to reach the position
    }

    delay(3000);

    randp.setSpeed(255);
    randp.forward();
    delay(750);

    randp.setSpeed(speed);
    randp.backward();
    delay(750);

    randp.stop();
}
