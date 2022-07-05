#include <RI32.h>

RI32& RI32::getInstance(){
	static RI32 instance;
	return instance;
}

void RI32::begin(int pin_l_a, int pin_l_b, int pin_r_a, int pin_r_b, float dt){
	this->pin_l_a = pin_l_a;
	this->pin_l_b = pin_l_b;
	this->pin_r_a = pin_r_a;
	this->pin_r_b = pin_r_b;
	this->dt = dt;

	pinMode(pin_l_a, INPUT);
	pinMode(pin_l_b, INPUT);
	pinMode(pin_r_a, INPUT);
	pinMode(pin_r_b, INPUT);

	attachInterrupt(digitalPinToInterrupt(pin_l_a), ENC_L_ISR, RISING);
	attachInterrupt(digitalPinToInterrupt(pin_r_a), ENC_R_ISR, RISING);

	reset();
}

void RI32::evaluate(){
	//From the last evaluation, dTicks contains the counted ticks.
	r_dTheta = (2*PI*r_dTicks) / ENC_TICKS;
	l_dTheta = (2*PI*l_dTicks) / ENC_TICKS;

	r_ds = r_dTheta * ENC_RADIUS;
	l_ds = l_dTheta * ENC_RADIUS;

	r_s += r_ds;
	l_s += l_ds;

	r_v = r_ds / dt;
	l_v = l_ds / dt;

	//From the next interrupt, reset dTicks and start counting again.
	r_dTicks = l_dTicks = 0;
}

void RI32::reset(){
	l_dTicks = l_dTheta = l_s = l_ds = l_v = 0;
	r_dTicks = r_dTheta = r_s = r_ds = r_v = 0;
}

void ENC_L_ISR(){
	RI32 enc = RI32::getInstance();
	ENC_ISR(enc.pin_l_b, enc.l_dTicks);
}

void ENC_R_ISR(){
	RI32 enc = RI32::getInstance();
	ENC_ISR(enc.pin_r_b, enc.r_dTicks);
}

inline void ENC_ISR(uint8_t pin_b, volatile int16_t &dTicks){
	//CCW
	if(digitalRead(pin_b))
		dTicks--;
	
	//CW
	else
		dTicks++;
}
