#include <Arduino.h>
#include <const.h>

#include <SerialPlotter.h>
#include <SerialPlotter.cpp>	//Just to avoid link errors.

#include <LMD18200.h>
#include <RI32.h>

#include <PID.h>

#include <speedController.h>

// #include <SerialController.h>

//Number of plotted samples per second.
#define N_SAMPLES	30

// NeoSWSerial ss(SS_RX, SS_TX);
// ss.begin(9600);

void start_timer2();

//Serial plotter.
SerialPlotter<float> plotter(Serial);

LMD18200 motor(LEFT_DIRECTION, RIGHT_DIRECTION);
RI32 enc(LEFT_ENCODER_A, LEFT_ENCODER_B, RIGHT_ENCODER_A, RIGHT_ENCODER_B, ENC_TICKS, ENC_RADIUS, ENC_WHEELBASE, DELTA_T);

//Internal speed controller.
SpeedController speedController(DELTA_T, 5000, 8000, LMD18200_PWM_MAX_VAL, motor, enc);

//Distance and heading error to linear and angular speed.
PID PID_module(10, 0, 0, 0, false);
PID PID_phase(10, 0, 0, 0, false);

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

#define TARGET_X	3
#define TARGET_Y	2

void loop(){
	//Interrupt occurred.
	if(tick){
		tick = false;

		float err_rho = TARGET_X - enc.getX();
		float target_theta = TARGET_Y - enc.getY();

		cartesian_to_polar(err_rho, target_theta);

		float err_theta = normalize_angle(target_theta - speedController.getTheta());

		if(err_theta > PI/2 || err_theta < -PI/2){
			err_rho = -err_rho;
			err_theta += PI;
		}

		float target_linear_speed = PID_module.evaluate(err_rho);
		float target_angular_speed = PID_phase.evaluate(err_theta);

		speedController.evaluate(target_linear_speed, target_angular_speed);

		if(c++ == int(1 / (N_SAMPLES * DELTA_T))){
			c = 0;

			plotter.start();
			
			plotter.add(speedController.getX() * 1000);
			plotter.add(speedController.getY() * 1000);
			plotter.add(speedController.getTheta(true));
			
			plotter.add(speedController.getPWM_l());
			plotter.add(speedController.getPWM_r());

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
