#include <Arduino.h>
#include <const.h>

#include <LMD18200.h>

LMD18200 motor(LEFT_DIRECTION, RIGHT_DIRECTION);

void setup(){
	Serial.begin(9600);
	motor.begin();
	
	motor.left(0, 500);
	motor.right(0, 500);

	motor.start();
}

void loop(){
}
