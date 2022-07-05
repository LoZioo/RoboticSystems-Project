/**
 * @file LMD18200.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-07-05
 * 
 * @copyright Copyright (c) 2022
 * 
 * This library uses the ATmega328 Timer1 to provide
 * the control of two LMD18200 H-bridge driver. 
 */

#ifndef LMD18200_H
#define LMD18200_H

#include <Arduino.h>

#define LEFT_PWM	10	//OC1B
#define RIGHT_PWM	9		//OC1A

#define PWM_MAX_VAL		1000

class LMD18200{
	private:
		uint8_t left_direction_pin;
		uint8_t right_direction_pin;
		
		void __write(uint8_t, uint8_t, uint16_t);

	public:
		LMD18200(uint8_t, uint8_t);

		void begin();

		void start();
		void stop();
		
		//Write in the left wheel the specified direction and speed (range [0, PWM_MAX_VAL]).
		void left(uint8_t, uint16_t);

		//Write in the right wheel the specified direction and speed (range [0, PWM_MAX_VAL]).
		void right(uint8_t, uint16_t);
};

#endif
