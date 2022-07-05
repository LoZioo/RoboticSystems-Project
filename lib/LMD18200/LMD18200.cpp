#include <LMD18200.h>

void LMD18200::__write(uint8_t wheel, uint8_t direction, uint16_t pwm){
	direction &= 1;
	pwm = map(
		constrain(pwm, 0, PWM_MAX_VAL),
		0, PWM_MAX_VAL, 0, ICR1
	);

	if(wheel){
		digitalWrite(left_direction_pin, direction);
		OCR1B = pwm;		//Left wheel.
	}

	else{
		digitalWrite(right_direction_pin, direction);
		OCR1A = pwm;		//Right wheel.
	}
}

LMD18200::LMD18200(uint8_t left_direction_pin, uint8_t right_direction_pin){
	left_direction_pin = left_direction_pin;
	right_direction_pin = right_direction_pin;

	pinMode(left_direction_pin, OUTPUT);
	pinMode(right_direction_pin, OUTPUT);

	pinMode(LEFT_PWM, OUTPUT);
	pinMode(RIGHT_PWM, OUTPUT);

	OCR1A = OCR1B = 0;
}

void LMD18200::begin(){
	//PWM frequency: 50kHz

	ICR1 = 320;

	TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11);
	TCCR1B = (1 << WGM12) | (1 << WGM13);
}

void LMD18200::start(){
	begin();

	TCCR1B |= (1 << CS10);
}

void LMD18200::stop(){
	TCCR1B = 0;

	digitalWrite(LEFT_PWM, LOW);
	digitalWrite(RIGHT_PWM, LOW);
}

void LMD18200::left(uint8_t direction, uint16_t pwm){
	__write(0, direction, pwm);
}

void LMD18200::right(uint8_t direction, uint16_t pwm){
	__write(1, direction, pwm);
}
