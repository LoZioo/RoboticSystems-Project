/**
 * @file main.cpp
 * @author Davide Scalisi
 * @brief 
 * @version 0.1
 * @date 2022-05-11
 * 
 * @copyright Copyright (c) 2022
 * 
 * Simulazione dei due canali A e B di un
 * encoder rotativo incrementale (a quadratura).
 * 
 * NB:	Calcoli per TCNT1_MIN_ARR e TCNT1_MAX_ARR
 * 			fatti con prescaler unitario.
 */

#include <Arduino.h>

#define TCNT1_MIN_ARR	61535		//65535 - (16.000.000 / (4 * 1kHz))
#define TCNT1_MAX_ARR	65285		//65535 - (16.000.000 / (4 * 16kHz))

#define POWER_BTN			2		//Abilitazione output.
#define DIRECTION_BTN	3		//Cambio rotazione da senso orario ad antiorario.
#define FREQ_POT			A0	//Modifica velocità.

#define CH_A 9
#define CH_B 10

inline void out_enable(bool = true);

volatile uint16_t TCNT1_reload = TCNT1_MIN_ARR;
volatile uint8_t channel = CH_A;

volatile bool enabled = true;
volatile bool toggleDirection = false;

bool CH_A_backup = 0, CH_B_backup = 0;

//Per debouncing software.
uint32_t cooldown = 0;

void setup(){
	Serial.begin(9600);

	pinMode(POWER_BTN, INPUT_PULLUP);
	pinMode(DIRECTION_BTN, INPUT_PULLUP);

	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(CH_A, OUTPUT);
	pinMode(CH_B, OUTPUT);

	out_enable(enabled);
}

void loop(){
	if(millis() > cooldown){
		//Delay per implementare il debouncing.
		cooldown = millis() + 250;

		if(!digitalRead(POWER_BTN)){
			enabled = !enabled;

			digitalWrite(LED_BUILTIN, enabled);
			out_enable(enabled);

			Serial.println("POWER_BTN");
		}
		
		if(!digitalRead(DIRECTION_BTN)){
			toggleDirection = true;

			Serial.println("DIRECTION_BTN");
		}
	}
	
	TCNT1_reload = map(analogRead(FREQ_POT), 0, 1023, TCNT1_MIN_ARR, TCNT1_MAX_ARR);
	delay(10);
}

inline void out_enable(bool enabled){
	if(enabled){
		digitalWrite(CH_A, CH_A_backup);
		digitalWrite(CH_B, CH_B_backup);

		/*
			Timer1
			Normal mode
			Prescaler disattivato
		*/
		TCCR1A = 0;
		TCCR1B = (1<<CS10);
		TIMSK1 = (1<<TOIE1);
		TCNT1 = TCNT1_reload;
	}
	
	else{
		TCCR1B = 0;

		CH_A_backup = digitalRead(CH_A);
		CH_B_backup = digitalRead(CH_B);

		digitalWrite(CH_A, LOW);
		digitalWrite(CH_B, LOW);
	}
}

ISR(TIMER1_OVF_vect){
	TCNT1 = TCNT1_reload;

	if(channel == CH_A)
		channel = CH_B;
	else
		channel = CH_A;
	
	if(toggleDirection){
		toggleDirection = false;
		digitalWrite(CH_B, !digitalRead(CH_B));
	}

	digitalWrite(channel, !digitalRead(channel));
}
