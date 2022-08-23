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

SpeedController::SpeedController(float dt, float kp, float ki, uint16_t max_pwm, LMD18200 &motor, RI32 &enc)
: motor(motor), enc(enc), dt(dt){

	PID_l = new PID(dt, kp, ki, 0, max_pwm, true);
	PID_r = new PID(dt, kp, ki, 0, max_pwm, true);
}

SpeedController::~SpeedController(){
	delete PID_l;
	delete PID_r;
}

void SpeedController::evaluate(float target_v, float target_omega, bool l_r_target_speed){
	//target_linear_speed/target_angular_speed to target_vleft/target_vright (default).
	if(!l_r_target_speed)
		__to_lr_speed(target_v, target_omega);

	float target_speed_l = target_v;
	float target_speed_r = target_omega;

	//From speed errors to PWM.
	PWM_l = PID_l->evaluate(target_speed_l, enc.getLeftSpeed());
	PWM_r = PID_r->evaluate(target_speed_r, enc.getRightSpeed());

	//Write new data.
	motor.left(PWM_l);
	motor.right(PWM_r);
}

void SpeedController::reset(){
	PID_l->reset();
	PID_r->reset();

	PWM_l = PWM_r = 0;
}

//----------------------------------------- PositionController ---------------------------------------//

PositionController::PositionController(float dt, float module_kp, float phase_kp, float max_linear_speed, float max_angular_speed, SpeedController &speedController)
: speedController(speedController), dt(dt){

	PID_module = new PID(dt, module_kp, 0, 0, max_linear_speed);
	PID_phase = new PID(dt, phase_kp, 0, 0, max_angular_speed);
}

PositionController::~PositionController(){
	delete PID_module;
	delete PID_phase;
}

void PositionController::evaluate(float target_x, float target_y){
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

void PositionController::reset(){
	PID_module->reset();
	PID_phase->reset();

	target_linear_speed = target_angular_speed = 0;

	speedController.reset();
}
