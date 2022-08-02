/**
 * @file SpeedController.h
 * @author Davide Scalisi
 * @brief 
 * @version 0.1
 * @date 2022-08-01
 * 
 * @copyright Copyright (c) 2022
 * 
 * Implementation of a cart speed controller.
 * About the angular speed sign:
 * 	+ is defined as left rotation.
 * 	- is defined as right rotation.
 */

#ifndef SPEEDCONTROLLER_H
#define SPEEDCONTROLLER_H

#include <Arduino.h>

#include <LMD18200.h>
#include <RI32.h>

#include <PID.h>

class SpeedController{
	private:
		LMD18200 &motor;
		RI32 &enc;

		//Speed error to PWM; min/max: [-LMD18200_PWM_MAX_VAL, LMD18200_PWM_MAX_VAL].
		PID *pid_l, *pid_r;

		//Sampling period.
		float dt;

		//From PIDs.
		int16_t PWM_l, PWM_r;

		void __to_lr_speed(float&, float&);

	public:
		//delta_t, proportional_k, integral_k, saturation, engine, encoder.
		SpeedController(float, uint16_t, uint16_t, uint16_t, LMD18200&, RI32&);
		~SpeedController();

		//target_linear_speed/target_vleft, target_angular_speed/target_vright, (false = lin/ang, true = vleft/vright).
		void evaluate(float, float, bool = false);

		int16_t getPWM_l()	{	return PWM_l;	}
		int16_t getPWM_r()	{	return PWM_r;	}
};

#endif
