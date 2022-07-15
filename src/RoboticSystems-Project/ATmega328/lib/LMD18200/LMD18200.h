/**
 * @file LMD18200.h
 * @author Davide Scalisi
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

#define OC1A	9
#define OC1B	10

#define PWM_MAX_VAL		1000

enum{
	DIRECTION_FORWARD,
	DIRECTION_BACKWARD
};

class LMD18200{
	private:
		uint8_t left_direction_pin;
		uint8_t right_direction_pin;
		
		//Write data on the specified wheel.
		void __write(uint8_t, uint8_t, uint16_t);

		//Start or stop the specified wheel.
		void __start(bool, bool);
		void __stop(bool, bool);

	public:
		//LEFT_DIRECTION, RIGHT_DIRECTION.
		LMD18200(uint8_t, uint8_t);

		void begin();
		
		//Write in the left wheel the specified direction and speed (range [0, PWM_MAX_VAL]).
		void left(uint8_t, uint16_t);

		//Write in the right wheel the specified direction and speed (range [0, PWM_MAX_VAL]).
		void right(uint8_t, uint16_t);

		void start();
		void start_left();
		void start_right();

		void stop();
		void stop_left();
		void stop_right();

		bool state_left()			{ return (TCCR1A & (1 << COM1B1)) != 0; }
		bool state_right()		{ return (TCCR1A & (1 << COM1A1)) != 0; }
};

#endif
