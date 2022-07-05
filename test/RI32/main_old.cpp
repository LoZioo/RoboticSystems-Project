#include <Arduino.h>

#define ENC_RADIUS	0.021
#define ENC_TICKS		4000

// #include <RI32.h>
#include <const.h>

void ENC_L_ISR();

void setup(){
	Serial.begin(9600);
	pinMode(LED_BUILTIN, OUTPUT);

	pinMode(ENC_L_PIN_A, INPUT);
	pinMode(ENC_L_PIN_B, INPUT);

	attachInterrupt(digitalPinToInterrupt(ENC_L_PIN_A), ENC_L_ISR, RISING);
}

volatile int16_t dTicks = 0;
float dTheta, s = 0, ds, dv;

void loop(){
	//Dall'ultimo azzeramento, dTicks contiene i tick percorsi.
	dTheta = (2*PI*dTicks) / ENC_TICKS;

	ds = dTheta * ENC_RADIUS;
	s += ds;

	dv = ds / DT;

	Serial.print(s * 1000);
	Serial.print(" ");
	Serial.print(ds * 1000);
	Serial.print(" ");
	Serial.print(dv * 1000);
	Serial.println();

	//Dal prossimo interrupt, riprendi a contare da zero.
	dTicks = 0;
	delay(DT * 1000);
}

void ENC_L_ISR(){
	//Antiorario
	if(digitalRead(ENC_L_PIN_B))
		dTicks--;
	
	//Orario
	else
		dTicks++;
}
