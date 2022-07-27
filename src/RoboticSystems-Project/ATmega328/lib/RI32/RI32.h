/**
 * @file RI32.h
 * @author Davide Scalisi
 * @brief 
 * @version 0.1
 * @date 2022-07-15
 * 
 * @copyright Copyright (c) 2022
 * 
 * This library is used to read data from two RI32
 * rotary encoder: one is the left sensing wheel
 * and the other one is the right sensing wheel.
 */

#ifndef RI32_H
#define RI32_H

#include <Arduino.h>

class RI32{
	private:
		friend void ENC_L_ISR(), ENC_R_ISR();

		//Quadrature pins; pin_l_a and pin_r_a must be both interrupt pins.
		static uint8_t pin_l_a, pin_l_b, pin_r_a, pin_r_b;

		//Encoder traits.
		uint16_t	enc_ticks;
		float			enc_radius, enc_wheelbase;

		//Sampling period.
		float dt;

		//Encoder interrupt ticks.
		static volatile int16_t dTicks_l, dTicks_r;

		//Left and right speed.
		float v_l, v_r;

		//Generic cinematic variables.
		float v, omega;
		float x = 0, y = 0, theta = 0;

	public:
		//LEFT_ENCODER_A, LEFT_ENCODER_B, RIGHT_ENCODER_A, RIGHT_ENCODER_B, ENC_TICKS, ENC_RADIUS, DELTA_T.
		RI32(uint8_t, uint8_t, uint8_t, uint8_t, uint16_t, float, float, float);
		
		//Must be called 1/dt times per seconds.
		void evaluate();

		//Odomerty reset.
		void reset();

		float getLeftSpeed()			{ return v_l;	}
		float getRightSpeed()			{ return v_r;	}
				
		float getLinearSpeed()		{ return v;			}
		float getAngularSpeed()		{ return omega;	}

		float getX()											{	return x;	}
		float getY()											{	return y;	}
		float getTheta(bool deg = false)	{ return deg ? degrees(theta) : theta;	}
};

#endif
