#include <RI32.h>

//----------------------------------------- ISR ---------------------------------------//

uint8_t RI32::pin_l_a, RI32::pin_l_b, RI32::pin_r_a, RI32::pin_r_b;
int16_t RI32::l_dTicks, RI32::r_dTicks;

inline void ENC_ISR(uint8_t pin_b, volatile int16_t &dTicks){
	//CCW
	if(digitalRead(pin_b))
		dTicks--;
	
	//CW
	else
		dTicks++;
}

void ENC_L_ISR(){
	ENC_ISR(RI32::pin_l_b, RI32::l_dTicks);
}

void ENC_R_ISR(){
	ENC_ISR(RI32::pin_r_b, RI32::r_dTicks);
}

//----------------------------------------- RI32 --------------------------------------//

RI32::RI32(uint8_t pin_l_a, uint8_t pin_l_b, uint8_t pin_r_a, uint8_t pin_r_b, uint16_t enc_ticks, float enc_radius, float dt){
	RI32::pin_l_a = pin_l_a;
	RI32::pin_l_b = pin_l_b;
	RI32::pin_r_a = pin_r_a;
	RI32::pin_r_b = pin_r_b;

	this->enc_ticks = enc_ticks;
	this->enc_radius = enc_radius;
	
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
	r_dTheta = (2*PI*r_dTicks) / enc_ticks;
	l_dTheta = (2*PI*l_dTicks) / enc_ticks;

	r_ds = r_dTheta * enc_radius;
	l_ds = l_dTheta * enc_radius;

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
