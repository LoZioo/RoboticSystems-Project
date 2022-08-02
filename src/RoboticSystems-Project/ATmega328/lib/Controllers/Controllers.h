/**
 * @file Controllers.h
 * @author Davide Scalisi
 * @brief 
 * @version 0.1
 * @date 2022-08-01
 * 
 * @copyright Copyright (c) 2022
 * 
 * 1)	Implementation of a cart speed controller.
 * 		About the angular speed sign:
 * 			+ is defined as left rotation.
 * 			- is defined as right rotation.
 * 
 * 2)	Implementation of a cart position controller.
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
		PID *pid_l, *pid_r;

		//Sampling period.
		float dt;

		//From PIDs.
		int16_t PWM_l, PWM_r;

		void __to_lr_speed(float&, float&);

	public:
		//delta_t, proportional_k, integral_k, saturation_PWM, engine, encoder.
		SpeedController(float, float, float, uint16_t, LMD18200&, RI32&);
		~SpeedController();

		//target_linear_speed/target_vleft, target_angular_speed/target_vright, (false = lin/ang, true = vleft/vright).
		void evaluate(float, float, bool = false);

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
		//delta_t, proportional_k, saturation_speed, speed_controller, engine, encoder.
		PositionController(float, float, float, SpeedController&);
		~PositionController();

		//target_x, target_y, target_theta.
		void evaluate(float, float, float);

		float getTargetLinearSpeed()	{	return target_linear_speed;		}
		float getTargetAngularSpeed()	{	return target_angular_speed;	}
};

#endif
