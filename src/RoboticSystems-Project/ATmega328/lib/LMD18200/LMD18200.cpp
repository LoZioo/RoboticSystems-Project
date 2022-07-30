#include <LMD18200.h>

void LMD18200::__write(uint8_t wheel, uint8_t direction, uint16_t pwm){
	direction &= 1;
	pwm = map(
		constrain(pwm, 0, LMD18200_PWM_MAX_VAL),
		0, LMD18200_PWM_MAX_VAL, 0, ICR1
	);

	if(wheel){
		if(pwm == 0)
			stop_left();

		else{
			if(!state_left())
				start_left();

			digitalWrite(left_direction_pin, direction);
			OCR1A = pwm;		//Left wheel.
		}
	}

	else{
		if(pwm == 0)
			stop_right();
		
		else{
			if(!state_right())
				start_right();
			
			digitalWrite(right_direction_pin, direction);
			OCR1B = pwm;		//Right wheel.
		}
	}
}

void LMD18200::__start(bool left, bool right){
	//PB2
	if(left)
		TCCR1A |= (1 << COM1B1);

	//PB1
	if(right)
		TCCR1A |= (1 << COM1A1);
}

void LMD18200::__stop(bool left, bool right){
	//PB2
	if(left){
		TCCR1A &= (uint8_t) ~(1 << COM1B1);
		DDRB |= (1 << DDB2);
	}

	//PB1
	if(right){
		TCCR1A &= (uint8_t) ~(1 << COM1A1);
		DDRB |= (1 << DDB1);
	}
}

LMD18200::LMD18200(uint8_t left_direction_pin, uint8_t right_direction_pin){
	this->left_direction_pin = left_direction_pin;
	this->right_direction_pin = right_direction_pin;

	pinMode(left_direction_pin, OUTPUT);
	pinMode(right_direction_pin, OUTPUT);

	pinMode(OC1A, OUTPUT);
	pinMode(OC1B, OUTPUT);

	left(0);
	right(0);
}

void LMD18200::begin(){
	/*
		PWM frequency: 50kHz
		16.000.000 / (160 * 2 * 1) = 50kHz.

		Where 2 is because we're using Phase Correct PWM mode.
	*/

	ICR1 = 160;

	TCCR1A = (1 << WGM11);
	TCCR1B = (1 << WGM13) | (1 << CS10);

	OCR1A = OCR1B = 0;
}

void LMD18200::left(uint8_t direction, uint16_t pwm){
	__write(0, direction, pwm);
}

void LMD18200::left(int16_t pwm_direction_speed){
	__write(0, pwm_direction_speed >= 0 ? DIRECTION_FORWARD : DIRECTION_BACKWARD, abs(pwm_direction_speed));
}

void LMD18200::right(uint8_t direction, uint16_t pwm){
	__write(1, direction, pwm);
}

void LMD18200::right(int16_t pwm_direction_speed){
	__write(1, pwm_direction_speed >= 0 ? DIRECTION_FORWARD : DIRECTION_BACKWARD, abs(pwm_direction_speed));
}

void LMD18200::start(){
	__start(true, true);
}

void LMD18200::start_left(){
	__start(true, false);
}

void LMD18200::start_right(){
	__start(false, true);
}

void LMD18200::stop(){
	__stop(true, true);
}

void LMD18200::stop_left(){
	__stop(true, false);
}

void LMD18200::stop_right(){
	__stop(false, true);
}
