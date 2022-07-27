#include <Arduino.h>
#include <const.h>

#include <LMD18200.h>
#include <RI32.h>

#include <PID.h>

#define	SPEED_TARGET_L	0.1
#define	SPEED_TARGET_R	0.1

// #include <SerialController.h>

// NeoSWSerial ss(SS_RX, SS_TX);
// ss.begin(9600);

void start_timer2();

LMD18200 motor(LEFT_DIRECTION, RIGHT_DIRECTION);
RI32 enc(LEFT_ENCODER_A, LEFT_ENCODER_B, RIGHT_ENCODER_A, RIGHT_ENCODER_B, ENC_TICKS, ENC_RADIUS, ENC_WHEELBASE, DELTA_T);

PID pid_l(DELTA_T, 100, 40, 0, 20, true);
PID pid_r(DELTA_T, 100, 40, 0, 20, true);

//Speed error to torque; max: 5N/m.
// PID pid_l(DELTA_T, 3, 1, 0, 5, true);
// PID pid_r(DELTA_T, 3, 1, 0, 5, true);

//Tick flag.
volatile bool tick = false;

//Tick counter (used for debug pourposes).
int c = 0;

void setup(){
	Serial.begin(115200);

	motor.begin();

	motor.left(DIRECTION_FORWARD, 300);
	motor.right(DIRECTION_FORWARD, 300);

	motor.start();

	// CODICE NON FUNZIONANTE, NON ABILITARE...
	// start_timer2();
}

void loop(){
	//Interrupt occurred.
	if(tick){
		tick = false;

		//Read new data.
		enc.evaluate();

		//From speed errors to torques.
		float torque_l = pid_l.evaluate(SPEED_TARGET_L, enc.getLeftSpeed());
		float torque_r = pid_r.evaluate(SPEED_TARGET_R, enc.getRightSpeed());

		//To forces.
		float force_l = torque_l / LOC_RADIUS;
		float force_r = torque_r / LOC_RADIUS;

		//To thrust force and rotational torque.
		float force = force_l + force_r;
		float torque = LOC_WHEELBASE * (force_r - force_l);

		//To linear and angular speed.
		float lin_speed = enc.getLinearSpeed();
		lin_speed = (DELTA_T / ROB_WEIGHT) * (force - ROB_LIN_AIR_FRICTION * lin_speed) + lin_speed;

		float ang_speed = enc.getAngularSpeed();
		ang_speed = ( (2 * DELTA_T) / (ROB_WEIGHT * pow(LOC_RADIUS, 2)) ) * (torque - ROB_ROT_AIR_FRICTION * ang_speed) + ang_speed;

		//To left and right speed.
		float speed_l = lin_speed - (ang_speed * LOC_WHEELBASE / 2);
		float speed_r = lin_speed + (ang_speed * LOC_WHEELBASE / 2);

		speed_l = constrain(speed_l, -ROB_MAX_SPEED, ROB_MAX_SPEED);
		speed_r = constrain(speed_r, -ROB_MAX_SPEED, ROB_MAX_SPEED);

		//To PWM (you can do it just because ROB_MAX_SPEED = 1).
		uint16_t pwm_l = abs(speed_l) * PWM_MAX_VAL;
		uint16_t pwm_r = abs(speed_r) * PWM_MAX_VAL;

		motor.left(speed_l >= 0 ? DIRECTION_FORWARD : DIRECTION_BACKWARD, pwm_l);
		motor.right(speed_r >= 0 ? DIRECTION_FORWARD : DIRECTION_BACKWARD, pwm_r);

		if(c++ == int(1 / DELTA_T)){
			//Debug every second.
			c = 0;

			// Serial.print("x: ");
			// Serial.print(enc.getX() * 100);
			// Serial.print("cm  ");

			// Serial.print("y: ");
			// Serial.print(enc.getY() * 100);
			// Serial.print("cm  ");

			// Serial.print("th: ");
			// Serial.print(enc.getAngle(true));
			// Serial.print("deg  ");

			// Serial.print("F: ");
			// Serial.print(force);
			// Serial.print("N  ");

			// Serial.print("T: ");
			// Serial.print(torque);
			// Serial.print("N/m  ");

			Serial.print("v: ");
			Serial.print(enc.getLinearSpeed());
			Serial.print("m/s  ");

			Serial.print("w: ");
			Serial.print(enc.getAngularSpeed());
			Serial.print("rad/s  ");

			Serial.print("v_t: ");
			Serial.print(lin_speed);
			Serial.print("m/s  ");

			Serial.print("w_t: ");
			Serial.print(ang_speed);
			Serial.print("rad/s  ");

			Serial.print("pwm_l: ");
			Serial.print(pwm_l);
			Serial.print("  ");

			Serial.print("d_l: ");
			Serial.print(speed_l >= 0 ? 'F' : 'B');
			Serial.print("  ");

			Serial.print("pwm_r: ");
			Serial.print(pwm_r);
			Serial.print("  ");

			Serial.print("d_r: ");
			Serial.print(speed_r >= 0 ? 'F' : 'B');
			Serial.print("  ");

			Serial.println();
		}
	}
}

//1kHz sampling rate.
#define TCNT2_OFFSET 250

void start_timer2(){
	TCNT2 = 0xff - TCNT2_OFFSET;
	TIMSK2 = (1 << TOIE2);

	TCCR2A = 0;
	TCCR2B = (1 << CS22);
}

ISR(TIMER2_OVF_vect){
	TCNT2 = 0xff - TCNT2_OFFSET;
	tick = true;
}
