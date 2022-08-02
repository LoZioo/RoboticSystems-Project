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

inline void start_timer2(), stop_timer2();

//Serial plotter.
SerialPlotter<float> plotter(Serial);

LMD18200 motor(LEFT_DIRECTION, RIGHT_DIRECTION);
RI32 enc(LEFT_ENCODER_A, LEFT_ENCODER_B, RIGHT_ENCODER_A, RIGHT_ENCODER_B, ENC_TICKS, ENC_RADIUS, ENC_WHEELBASE, DELTA_T);

//Internal speed controller.
// SpeedController speedController(DELTA_T, 5000, 8000, LMD18200_PWM_MAX_VAL, motor, enc);
SpeedController speedController(DELTA_T, 1000, 3000, LMD18200_PWM_MAX_VAL, motor, enc);

//Distance and heading error to linear and angular speed.
PID PID_module(DELTA_T, 1, 0, 0, 0.5);
PID PID_phase(DELTA_T, 1, 0, 0, 0.5);

//Tick flag.
volatile bool tick = false;

//It needs to lock the control system when the target is reached.
bool evaluate = true;

//Tick counter (used for debug pourposes).
int c = 0;

void setup(){
	Serial.begin(115200);

	motor.begin();
	motor.start();

	start_timer2();
}

#define TARGET_X		-0.8
#define TARGET_Y		-0.8
#define TARGET_RHO	hypot(TARGET_X, TARGET_Y)

#define TARGET_THETA	90

#define TOL_RHO		0.01
#define TOL_THETA	1			//Deg.

void loop(){
	//Interrupt occurred.
	if(tick){
		tick = false;

		if(evaluate){
			float err_x = TARGET_X - enc.getX();
			float err_y = TARGET_Y - enc.getY();

			float err_rho = hypot(err_x, err_y);
			float err_theta = normalize_angle(atan2(err_y, err_x) - enc.getTheta());

			if(err_theta < -PI/2 || err_theta > PI/2){
				err_rho = -err_rho;
				err_theta = normalize_angle(err_theta + PI);
			}

			float target_linear_speed = PID_module.evaluate(err_rho);
			float target_angular_speed = PID_phase.evaluate(err_theta);

			speedController.evaluate(target_linear_speed, target_angular_speed);

			//Tollerance check.
			if(
				// (enc.getX() > TARGET_X - TOL_X && enc.getX() < float(TARGET_X + TOL_X)) &&
				// (enc.getY() > TARGET_Y - TOL_Y && enc.getY() < float(TARGET_Y + TOL_Y)) &&

				(enc.getRho() > TARGET_RHO - TOL_RHO && enc.getRho() < TARGET_RHO + TOL_RHO) &&
				(enc.getTheta() > normalize_angle(radians(TARGET_THETA) - radians(TOL_THETA)) && enc.getTheta() < normalize_angle(radians(TARGET_THETA) + radians(TOL_THETA)))
			){
				//Stop engines.
				motor.stop();

				//Stop control system.
				evaluate = false;
			}
		}

		if(c++ == int(1 / (N_SAMPLES * DELTA_T))){
			c = 0;

			plotter.start();
			
			plotter.add(enc.getX() * 1000);
			plotter.add(enc.getY() * 1000);
			plotter.add(enc.getRho() * 1000);
			plotter.add(enc.getTheta(true));

			plotter.add(TARGET_RHO * 1000);
			plotter.add(TARGET_THETA);

			// plotter.add(target_linear_speed * 1000);
			// plotter.add(degrees(target_angular_speed));
			
			plotter.add(speedController.getPWM_l());
			plotter.add(speedController.getPWM_r());

			plotter.plot();
		}
	}
}

//125Hz sampling rate.
#define TCNT2_OFFSET 125

inline void start_timer2(){
	TCNT2 = 0xff - TCNT2_OFFSET;
	TIMSK2 = (1 << TOIE2);

	TCCR2A = 0;
	TCCR2B = (1 << CS20) | (1 << CS21) | (1 << CS22);
}

inline void stop_timer2(){
	TIMSK2 = 0;
}

ISR(TIMER2_OVF_vect){
	TCNT2 = 0xff - TCNT2_OFFSET;
	tick = true;
}
