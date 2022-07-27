#include <Arduino.h>
#include <const.h>

#include <LMD18200.h>
#include <RI32.h>

#include <PID.h>

#define	SPEED_TARGET_L	0.5
#define	SPEED_TARGET_R	-0.5

// #include <SerialController.h>

// NeoSWSerial ss(SS_RX, SS_TX);
// ss.begin(9600);

template<class T> void plot(T);
void start_timer2();

LMD18200 motor(LEFT_DIRECTION, RIGHT_DIRECTION);
RI32 enc(LEFT_ENCODER_A, LEFT_ENCODER_B, RIGHT_ENCODER_A, RIGHT_ENCODER_B, ENC_TICKS, ENC_RADIUS, ENC_WHEELBASE, DELTA_T);

//Speed error to PWM; min/max: [-1000, 1000].
PID pid_l(DELTA_T, 100, 50, 0, PWM_MAX_VAL, true);
PID pid_r(DELTA_T, 100, 50, 0, PWM_MAX_VAL, true);

//Tick flag.
volatile bool tick = false;

//Tick counter (used for debug pourposes).
int c = 0;

void setup(){
	Serial.begin(115200);

	motor.begin();

	motor.left(DIRECTION_FORWARD, 0);
	motor.right(DIRECTION_BACKWARD, 0);

	motor.start();

	start_timer2();
}

void loop(){
	//Interrupt occurred.
	if(tick){
		tick = false;

		//Read new data.
		enc.evaluate();

		//From speed errors to PWM.
		int16_t PWM_l = pid_l.evaluate(SPEED_TARGET_L, enc.getLeftSpeed());
		int16_t PWM_r = pid_r.evaluate(SPEED_TARGET_R, enc.getRightSpeed());

		//Update engine speed.
		motor.left(PWM_l >= 0 ? DIRECTION_FORWARD : DIRECTION_BACKWARD, abs(PWM_l));
		motor.right(PWM_r >= 0 ? DIRECTION_FORWARD : DIRECTION_BACKWARD, abs(PWM_r));

		if(c++ == int(1 / (16 * DELTA_T))){
			c = 0;

			plot<float>(enc.getLeftSpeed() * 100);
			plot<float>(SPEED_TARGET_L * 100);
			plot<int16_t>(PWM_l);
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

template<class T> void plot(T data){
	float tmp = data;
	Serial.write((uint8_t*) &tmp, sizeof(float));
}
