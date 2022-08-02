#include <RI32.h>

//----------------------------------------- ISR ---------------------------------------//

uint8_t RI32::pin_l_a, RI32::pin_l_b, RI32::pin_r_a, RI32::pin_r_b;
volatile int16_t RI32::dTicks_l, RI32::dTicks_r;

void ENC_L_ISR(){
	digitalRead(RI32::pin_l_b) ? RI32::dTicks_l-- : RI32::dTicks_l++;
}

void ENC_R_ISR(){
	digitalRead(RI32::pin_r_b) ? RI32::dTicks_r++ : RI32::dTicks_r--;
}

//----------------------------------------- RI32 --------------------------------------//

RI32::RI32(uint8_t pin_l_a, uint8_t pin_l_b, uint8_t pin_r_a, uint8_t pin_r_b, float dt, uint16_t enc_ticks, float enc_radius, float enc_wheelbase)
: dt(dt), enc_ticks(enc_ticks), enc_radius(enc_radius), enc_wheelbase(enc_wheelbase){
	
	RI32::pin_l_a = pin_l_a;
	RI32::pin_l_b = pin_l_b;
	RI32::pin_r_a = pin_r_a;
	RI32::pin_r_b = pin_r_b;

	pinMode(pin_l_a, INPUT);
	pinMode(pin_l_b, INPUT);
	pinMode(pin_r_a, INPUT);
	pinMode(pin_r_b, INPUT);

	attachInterrupt(digitalPinToInterrupt(pin_l_a), ENC_L_ISR, RISING);
	attachInterrupt(digitalPinToInterrupt(pin_r_a), ENC_R_ISR, RISING);
}

void RI32::evaluate(){
	//Left and right cinematic variables.
	//From the last evaluation, dTicks contains the counted ticks.
	float dTheta_l = (2*PI*dTicks_l) / enc_ticks;
	float dTheta_r = (2*PI*dTicks_r) / enc_ticks;

	//From the next interrupt, reset dTicks and start counting again.
	dTicks_l = dTicks_r = 0;

	float ds_l = dTheta_l * enc_radius;
	float ds_r = dTheta_r * enc_radius;

	//Left and right speed.
	v_l = ds_l / dt;
	v_r = ds_r / dt;

	float ds = (ds_l + ds_r) / 2;
	float dTheta = (ds_r - ds_l) / enc_wheelbase;

	//Generic cinematic variables.
	v = (v_l + v_r) / 2;
	omega = (v_r - v_l) / enc_wheelbase;

	x += ds * cos(theta + dTheta/2);
	y += ds * sin(theta + dTheta/2);
	theta = normalize_angle(theta + dTheta);
}

void RI32::resetX(){
	x = 0;
	theta = 0;
}

void RI32::resetY(){
	y = 0;
	theta = 90;
}
