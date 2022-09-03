/**
 * @file Controllers.h
 * @author Davide Scalisi
 * @brief 
 * 	1)	Implementation of a cart speed controller.
 * 			About the angular speed sign:
 * 				+ is defined as left rotation.
 * 				- is defined as right rotation.
 * 
 * 	2)	Implementation of a cart position controller.
 * 
 * @version 0.1
 * @date 2022-08-01
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef ROBOT_CONTROLLERS_H
#define ROBOT_CONTROLLERS_H

#include <Arduino.h>

#include <LMD18200.h>
#include <RI32.h>

#include <PID.h>

class PositionController;
class SpeedController{
	friend PositionController;

	private:
		LMD18200 &motor;
		RI32 &enc;

		//Speed error to PWM; min/max: [-LMD18200_PWM_MAX_VAL, LMD18200_PWM_MAX_VAL].
		PID *PID_l, *PID_r;

		//Sampling period.
		float dt;

		//From PIDs.
		int16_t PWM_l, PWM_r;

		void __to_lr_speed(float&, float&);

	public:
		//delta_t, proportional_k, integral_k, max_pwm, engine, encoder.
		SpeedController(float, float&, float&, uint16_t, LMD18200&, RI32&);
		~SpeedController();

		//target_linear_speed/target_vleft, target_angular_speed/target_vright, (false = lin/ang, true = vleft/vright).
		void evaluate(float, float, bool = false);
		void reset();

		int16_t getLeftPWM()	{	return PWM_l;	}
		int16_t getRightPWM()	{	return PWM_r;	}
};

class PositionController{
	private:
		SpeedController &speedController;

		//Distance and heading error to linear and angular speed.
		PID *PID_module, *PID_phase;

		//Sampling period.
		float dt;

		//From PIDs.
		float target_linear_speed, target_angular_speed;

	public:
		//delta_t, module_proportional_k, phase_proportional_k, max_linear_speed (m/s), max_angular_speed (rad/s), speed_controller.
		PositionController(float, float&, float&, float&, float&, SpeedController&);
		~PositionController();

		//target_x, target_y.
		void evaluate(float, float);
		void reset();

		float getTargetLinearSpeed()	{	return target_linear_speed;		}
		float getTargetAngularSpeed()	{	return target_angular_speed;	}

		float getMaxLinearSpeed()		{	return PID_module->getSat();	}
		float getMaxAngularSpeed()	{	return PID_phase->getSat();		}

		void setMaxLinearSpeed(float sat)		{	PID_module->setSat(sat);	}
		void setMaxAngularSpeed(float sat)	{	PID_phase->setSat(sat);		}
};

#endif
