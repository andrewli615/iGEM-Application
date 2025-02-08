/*
Code to run two RGB sensors to control servo motors to press the arrow keys and play the dinosaur game on Chrome
*/

/*
Import three libraries:
 - Adafruit library to use the sensor's functions
 - Servo library to control the servo motors
 - Wire library for I2C (Using the SDA and SDL pins)
*/
#include <Adafruit_TCS34725.h>
#include <Servo.h>
#include <Wire.h>

/*
Set brightness threshold for the sensor to detect
This value should be adjusted based on the brightness of the screen
*/
const int THRESHOLD = 700;

/*
Set game initial speed, max speed, and acceleration
*/
const float INITIAL_SPEED = 6.0;
const float MAX_SPEED = 13.0;
const float ACCELERATION = 0.001;

/*
Create a variable to track the time
*/
unsigned long lastTime = 0;

/*
Create a variable for the current speed
*/
float currentSpeed = INITIAL_SPEED;

/*
Create servo objects for two servos, one for jump and one for duck
*/
Servo jumpS, duckS;

/*
Create sensor objects for two sensors, one for the top and one for the bottom from the Adafruit TCS34725 library
*/
Adafruit_TCS34725 sensorTop = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);
Adafruit_TCS34725 sensorBottom = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X, 0x2A); // bottom sensor has changed I2C address

/*
Attach the servos and set them to the neutral position
Set data rate for serial communication
*/
void setup() {
  jumpS.attach(9);
  jumpS.write(0);

  duckS.attach(10);
  duckS.write(0);

  lastTime = millis();

  Serial.begin(9600);
}

/*
Helper method that calculates the delay time based on the current speed
*/
int calculateDelay(float speed) {
  return 200 - (100 * (speed - INITIAL_SPEED) / (MAX_SPEED - INITIAL_SPEED)); 
}

/*
Define the red, green, blue, and brightness values for the top and bottom sensors.
The top and bottom sensors get the data values of the screen pixels and if cTop or cBottom are below the threshold (black),
the respective servo presses the respective key on the keyboard.
Prints the brightness to the serial monitor.
Servo then holds the key for 200ms initially but as the game speeds up, the delay decreases to 100ms before the servo returns to the neutral position.
The program is paused for 25ms to prevent sensor spamming.
We then update the speed if the current time has passed a second since the last loop.
This program then loops over and over.
*/
void loop() {
  int rTop, gTop, bTop, cTop;
  int rBottom, gBottom, bBottom, cBottom;

  sensorTop.getRawData(rTop, gTop, bTop, cTop);
  sensorBottom.getRawData(rBottom, gBottom, bBottom, cBottom);

  int delayTime = calculateDelay(currentSpeed);

  if (cBottom < THRESHOLD) {
    Serial.print("Brightness (Jump): ");
    Serial.println(cBottom);
    jumpS.write(75);
    delay(delayTime);
    jumpS.write(0);
  } else if (cTop < THRESHOLD) {
    Serial.print("Brightness (Duck): ");
    Serial.println(cTop);
    duckS.write(75);
    delay(delayTime);
    duckS.write(0);
  }

  delay(25);

  unsigned long currentTime = millis();
  if (currentTime - lastTime >= 1000) {
    if (currentSpeed < MAX_SPEED) {
      currentSpeed += ACCELERATION;
    }
    lastTime = currentTime;
  }
}