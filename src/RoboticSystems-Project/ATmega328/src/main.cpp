#include <Arduino.h>
#include <const.h>

#include <LMD18200.h>
#include <RI32.h>

#include <PID.h>

// #include <SerialController.h>

// NeoSWSerial ss(SS_RX, SS_TX);
// ss.begin(9600);

void setup_timer2();

LMD18200 motor(LEFT_DIRECTION, RIGHT_DIRECTION);
RI32 enc(LEFT_ENCODER_A, LEFT_ENCODER_B, RIGHT_ENCODER_A, RIGHT_ENCODER_B, ENC_TICKS, ENC_RADIUS, ENC_WHEELBASE, DELTA_T);

// Speed error to torque; max: 5N/m.
PID pid_l(DELTA_T, 3, 1, 0, 5, true);
PID pid_r(DELTA_T, 3, 1, 0, 5, true);

void setup(){
	Serial.begin(115200);

	motor.begin();

	motor.left(DIRECTION_FORWARD, 300);
	motor.right(DIRECTION_FORWARD, 300);

	motor.start();

	setup_timer2();
}

void loop(){
	Serial.print(enc.getX() * 100);
	Serial.print("cm ");

	Serial.print(enc.getY() * 100);
	Serial.print("cm ");

	Serial.print(enc.getAngle(true));
	Serial.print("deg ");

	Serial.println();
	
	delay(1000);
}

//1kHz sampling rate.
#define TCNT2_OFFSET 250

void setup_timer2(){
	TCNT2 = 0xff - TCNT2_OFFSET;
	TIMSK2 = (1 << TOIE2);

	TCCR2A = 0;
	TCCR2B = (1 << CS22);
}

#define	SPEED_TARGET_L	0.3
#define	SPEED_TARGET_R	0.3

ISR(TIMER2_OVF_vect){
	TCNT2 = 0xff - TCNT2_OFFSET;

	//Read new data.
	enc.evaluate();

	//Conversion from speed error to torque.
	float torque_l = pid_l.evaluate(SPEED_TARGET_L, enc.getLeftSpeed());
	float torque_r = pid_r.evaluate(SPEED_TARGET_R, enc.getRightSpeed());

	float force_l = torque_l / LOC_RADIUS;
	float force_r = torque_r / LOC_RADIUS;

	float force = force_l + force_r;
	float torque = LOC_WHEELBASE * (force_r - force_l);
}
