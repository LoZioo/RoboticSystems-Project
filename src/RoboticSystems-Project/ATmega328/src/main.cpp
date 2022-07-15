#include <Arduino.h>
#include <const.h>

#include <LMD18200.h>
#include <RI32.h>

// #include <SerialController.h>

// NeoSWSerial ss(SS_RX, SS_TX);
// ss.begin(9600);

LMD18200 motor(LEFT_DIRECTION, RIGHT_DIRECTION);
RI32 enc(LEFT_ENCODER_A, LEFT_ENCODER_B, RIGHT_ENCODER_A, RIGHT_ENCODER_B, ENC_TICKS, ENC_RADIUS, DELTA_T);

void setup(){
	Serial.begin(115200);

	motor.begin();

	motor.left(DIRECTION_FORWARD, 200);
	motor.right(DIRECTION_FORWARD, 200);

	motor.start();
}

int c = 0;
void loop(){
	enc.evaluate();

	if(enc.getRightSpace() > .3)
		motor.stop();

	//1s debug.
	if(c == (int)(1 / DELTA_T)){
		Serial.print(enc.getRightSpace());
		Serial.print("m ");
		Serial.print(enc.getRightSpeed());
		Serial.println("m/s");

		c = -1;
	}
	c++;

	delay(DELTA_T * 1000);
}
