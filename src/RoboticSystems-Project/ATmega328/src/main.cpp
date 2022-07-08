#include <Arduino.h>
#include <const.h>

#include <LMD18200.h>

LMD18200 motor(LEFT_DIRECTION, RIGHT_DIRECTION);

void setup(){
	Serial.begin(115200);
	motor.begin();

	motor.left(DIRECTION_FORWARD, 250);
	motor.right(DIRECTION_BACKWARD, 250);

	motor.start();
}

void loop(){
}
