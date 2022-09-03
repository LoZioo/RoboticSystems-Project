/**
 * @file PID.h
 * @author Davide Scalisi
 * @brief Implementation of a PID controller.
 * @version 0.1
 * @date 2022-07-16
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef PID_H
#define PID_H

#include <Arduino.h>

class PID{
	private:
		float dt;

		float &kp, &ki, &kd, &sat;
		bool anti_windup;

		float integral = 0;
		float prev_err = 0;

		bool in_saturation = false;

	public:
		//dt, kp, ki, kd, sat, anti_windup
		PID(float dt, float &kp, float &ki, float &kd, float &sat, bool anti_windup = false)
			: dt(dt), kp(kp), ki(ki), kd(kd), sat(sat), anti_windup(anti_windup) {}

		float evaluate(float);
		float evaluate(float, float);

		void reset()	{	integral = prev_err = in_saturation = 0;	}

		float	getSat()					{	return sat;					}
		bool	getAntiwindup()		{	return anti_windup;	}

		void setSat(float sat)									{	this->sat = sat;									}
		void setAntiwindup(float anti_windup)		{	this->anti_windup = anti_windup;	}
};

#endif
