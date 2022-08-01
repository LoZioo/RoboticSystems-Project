#include <SpeedController.h>

void SpeedController::__to_lr_speed(float &lin_speed, float &ang_speed){
	const float lin_speed_tmp = lin_speed;
	const float coeff = (ang_speed * enc.getWheelbase()) / 2;
	
	//v_left.
	lin_speed = lin_speed_tmp - coeff;

	//v_right.
	ang_speed = lin_speed_tmp + coeff;
}

SpeedController::SpeedController(float dt, uint16_t kp, uint16_t ki, uint16_t sat, LMD18200 &motor, RI32 &enc)
: motor(motor), enc(enc), dt(dt){

	pid_l = new PID(dt, kp, ki, 0, sat, true);
	pid_r = new PID(dt, kp, ki, 0, sat, true);
}

SpeedController::~SpeedController(){
	delete pid_l;
	delete pid_r;
}

void SpeedController::evaluate(float v_target, float omega_target, bool l_r_target_speed){
	//target_linear_speed/target_angular_speed to target_vleft/target_vright (default).
	if(!l_r_target_speed)
		__to_lr_speed(v_target, omega_target);

	float target_speed_l = v_target;
	float target_speed_r = omega_target;

	//Read new data.
	enc.evaluate();

	//From speed errors to PWM.
	PWM_l = pid_l->evaluate(target_speed_l, enc.getLeftSpeed());
	PWM_r = pid_r->evaluate(target_speed_r, enc.getRightSpeed());

	//Write new data.
	motor.left(PWM_l);
	motor.right(PWM_r);
}
