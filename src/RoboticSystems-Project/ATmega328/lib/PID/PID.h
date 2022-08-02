/**
 * @file PID.h
 * @author Davide Scalisi
 * @brief 
 * @version 0.1
 * @date 2022-07-16
 * 
 * @copyright Copyright (c) 2022
 * 
 * Implementation of a PID controller.
 */

#ifndef PID_H
#define PID_H

#include <Arduino.h>

class PID{
	private:
		float dt;

		float kp, ki, kd, sat;
		bool anti_windup;

		float integral = 0;
		float prev_err = 0;

		bool in_saturation = false;

	public:
		//dt, kp, ki, kd, sat, anti_windup
		PID(float dt, float kp, float ki, float kd, float sat, bool anti_windup = false)
			: dt(dt), kp(kp), ki(ki), kd(kd), sat(sat), anti_windup(anti_windup) {}

		float evaluate(float);
		float evaluate(float, float);
};

#endif
