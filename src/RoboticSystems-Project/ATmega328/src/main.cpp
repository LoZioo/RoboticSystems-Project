#include <Arduino.h>
#include <const.h>

#include <SerialPlotter.h>
#include <SerialPlotter.cpp>	//Just to avoid link errors.

#include <LMD18200.h>
#include <RI32.h>

#include <PID.h>

// #include <SerialController.h>

//Number of samples per second.
#define N_SAMPLES	30

// NeoSWSerial ss(SS_RX, SS_TX);
// ss.begin(9600);

void start_timer2();

//Serial plotter.
SerialPlotter<float> plotter(Serial);

LMD18200 motor(LEFT_DIRECTION, RIGHT_DIRECTION);
RI32 enc(LEFT_ENCODER_A, LEFT_ENCODER_B, RIGHT_ENCODER_A, RIGHT_ENCODER_B, ENC_TICKS, ENC_RADIUS, ENC_WHEELBASE, DELTA_T);

//Speed error to PWM; min/max: [-1000, 1000].
#define PID_P	5000
#define PID_I	8000
#define PID_D	0

PID pid_l(DELTA_T, PID_P, PID_I, PID_D, LMD18200_PWM_MAX_VAL, true);
PID pid_r(DELTA_T, PID_P, PID_I, PID_D, LMD18200_PWM_MAX_VAL, true);

float speed_target_l = 0.2;
float speed_target_r = -0.2;
float increment = 0.01;

//Tick flag.
volatile bool tick = false;

//Tick counter (used for debug pourposes).
int c = 0;

void setup(){
	Serial.begin(115200);

	motor.begin();
	motor.start();

	start_timer2();
}

int d = 0;

void loop(){
	//Interrupt occurred.
	if(tick){
		tick = false;

		//Read new data.
		enc.evaluate();

		//From speed errors to PWM.
		int16_t PWM_l = pid_l.evaluate(speed_target_l, enc.getLeftSpeed());
		int16_t PWM_r = pid_r.evaluate(speed_target_r, enc.getRightSpeed());

		motor.left(PWM_l);
		motor.right(PWM_r);

		if(d++ == int(1 / (16 * DELTA_T))){
			d = 0;

			//CORREGGERE A 0m/s.
			speed_target_l += increment;
			if(speed_target_l <= 0.2 || speed_target_l >= 1)
				increment = -increment;

			speed_target_r = -speed_target_l;
		}

		if(c++ == int(1 / (N_SAMPLES * DELTA_T))){
			c = 0;

			plotter.start();
			
			plotter.push(enc.getLeftSpeed() * 1000);
			plotter.push(speed_target_l * 1000);
			plotter.push(PWM_l);

			plotter.plot();
		}
	}
}

//125Hz sampling rate.
#define TCNT2_OFFSET 125

void start_timer2(){
	TCNT2 = 0xff - TCNT2_OFFSET;
	TIMSK2 = (1 << TOIE2);

	TCCR2A = 0;
	TCCR2B = (1 << CS20) | (1 << CS21) | (1 << CS22);
}

ISR(TIMER2_OVF_vect){
	TCNT2 = 0xff - TCNT2_OFFSET;
	tick = true;
}
