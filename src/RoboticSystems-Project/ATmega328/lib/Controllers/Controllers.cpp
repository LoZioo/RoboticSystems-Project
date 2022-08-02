#include <Controllers.h>

//----------------------------------------- SpeedController ---------------------------------------//

void SpeedController::__to_lr_speed(float &lin_speed, float &ang_speed){
	const float lin_speed_tmp = lin_speed;
	const float coeff = (ang_speed * enc.getWheelbase()) / 2;
	
	//v_left.
	lin_speed = lin_speed_tmp - coeff;

	//v_right.
	ang_speed = lin_speed_tmp + coeff;
}

SpeedController::SpeedController(float dt, float kp, float ki, uint16_t sat, LMD18200 &motor, RI32 &enc)
: motor(motor), enc(enc), dt(dt){

	pid_l = new PID(dt, kp, ki, 0, sat, true);
	pid_r = new PID(dt, kp, ki, 0, sat, true);
}

SpeedController::~SpeedController(){
	delete pid_l;
	delete pid_r;
}

void SpeedController::evaluate(float target_v, float target_omega, bool l_r_target_speed){
	//target_linear_speed/target_angular_speed to target_vleft/target_vright (default).
	if(!l_r_target_speed)
		__to_lr_speed(target_v, target_omega);

	float target_speed_l = target_v;
	float target_speed_r = target_omega;

	//Read new data.
	enc.evaluate();

	//From speed errors to PWM.
	PWM_l = pid_l->evaluate(target_speed_l, enc.getLeftSpeed());
	PWM_r = pid_r->evaluate(target_speed_r, enc.getRightSpeed());

	//Write new data.
	motor.left(PWM_l);
	motor.right(PWM_r);
}

//----------------------------------------- PositionController ---------------------------------------//

PositionController::PositionController(float dt, float kp, float sat, SpeedController &speedController)
: speedController(speedController), dt(dt){

	PID_module = new PID(dt, kp, 0, 0, sat);
	PID_phase = new PID(dt, kp, 0, 0, sat);
}

PositionController::~PositionController(){
	delete PID_module;
	delete PID_phase;
}

void PositionController::evaluate(float target_x, float target_y, float target_theta){
	float err_x = target_x - speedController.enc.getX();
	float err_y = target_y - speedController.enc.getY();

	float err_rho = hypot(err_x, err_y);
	float err_theta = normalize_angle(atan2(err_y, err_x) - speedController.enc.getTheta());

	if(err_theta < -PI/2 || err_theta > PI/2){
		err_rho = -err_rho;
		err_theta = normalize_angle(err_theta + PI);
	}

	target_linear_speed = PID_module->evaluate(err_rho);
	target_angular_speed = PID_phase->evaluate(err_theta);

	speedController.evaluate(target_linear_speed, target_angular_speed);
}
