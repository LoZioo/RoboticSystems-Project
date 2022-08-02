#include <Arduino.h>
#include <const.h>

#include <SerialPlotter.h>
#include <SerialPlotter.cpp>	//Just to avoid linker errors.

#include <LMD18200.h>
#include <RI32.h>
#include <PID.h>
#include <Controllers.h>

// #include <SerialController.h>

//Number of plotted samples per second.
#define N_SAMPLES	30

#define TARGET_X		0.8
#define TARGET_Y		0.8
#define TARGET_RHO	hypot(TARGET_X, TARGET_Y)

#define TARGET_THETA	90

#define TOL_RHO		0.01
#define TOL_THETA	1			//Deg.

inline void start_timer2(), stop_timer2();

//Serial plotter.
SerialPlotter<float> plotter(Serial);

// NeoSWSerial ss(SS_RX, SS_TX);
// ss.begin(9600);

LMD18200 motor(LEFT_DIRECTION, RIGHT_DIRECTION);
RI32 enc(LEFT_ENCODER_A, LEFT_ENCODER_B, RIGHT_ENCODER_A, RIGHT_ENCODER_B, DELTA_T, ENC_TICKS, ENC_RADIUS, ENC_WHEELBASE);

SpeedController speedController(DELTA_T, 1000, 3000, LMD18200_PWM_MAX_VAL, motor, enc);
PositionController positionController(DELTA_T, 1, ROB_MAX_SPEED, speedController);

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

void loop(){
	//Interrupt occurred.
	if(tick){
		tick = false;

		if(evaluate){
			positionController.evaluate(TARGET_X, TARGET_Y, radians(TARGET_THETA));

			//Tollerance check.
			if(
				(
					enc.getRho() > TARGET_RHO - TOL_RHO &&
					enc.getRho() < TARGET_RHO + TOL_RHO
				) && (
					enc.getTheta() > normalize_angle(radians(TARGET_THETA) - radians(TOL_THETA)) &&
					enc.getTheta() < normalize_angle(radians(TARGET_THETA) + radians(TOL_THETA))
				)
			){
				//Stop engine.
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
			
			plotter.add(speedController.getLeftPWM());
			plotter.add(speedController.getRightPWM());

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
