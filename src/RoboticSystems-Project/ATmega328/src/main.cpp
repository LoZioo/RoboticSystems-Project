#include <Arduino.h>
#include <const.h>

#include <SerialController.h>

// NeoSWSerial ss(SS_RX, SS_TX);
LMD18200 motor(LEFT_DIRECTION, RIGHT_DIRECTION);

void setup(){
	Serial.begin(115200);
	// ss.begin(9600);

	motor.begin();

	motor.left(DIRECTION_FORWARD, 100);
	motor.right(DIRECTION_BACKWARD, 100);

	motor.start();
}

void loop(){
}
