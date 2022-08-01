#include <PID.h>

PID::PID(float dt, float kp, float ki, float kd, float sat, float anti_windup){
	this->dt = dt;
	
	this->kp = kp;
	this->ki = ki;
	this->kd = kd;

	this->sat = sat;
	this->anti_windup = anti_windup;
}

float PID::evaluate(float err){
	float derivative = (err - prev_err) / dt;
	prev_err = err;

	// if(!(anti_windup && in_saturation))
	// 	integral += ki * err * dt;

	if(!anti_windup)
		integral += ki * err * dt;
	
	else if(!in_saturation)
		integral += ki * err * dt;
	
	float output = (kp * err) + integral + derivative * kd;

	if(sat != 0){
		if(output > sat){
			output = sat;
			in_saturation = true;
		}

		else if(output < -sat){
			output = -sat;
			in_saturation = true;
		}

		else
			in_saturation = false;
	}

	return output;
}

float PID::evaluate(float target, float current){
	return evaluate(target - current);
}
