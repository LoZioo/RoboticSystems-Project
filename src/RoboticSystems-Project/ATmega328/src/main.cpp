#include <Arduino.h>
#include <EEPROM.h>
#include <NeoSWSerial.h>

#include <const.h>
#include <packet.h>

#include <SerialPlotter.h>
#include <SerialPlotter.cpp>	//Just to avoid linker errors.

#include <LMD18200.h>
#include <RI32.h>
#include <PID.h>
#include <Controllers.h>

inline void handle_packet(), reset_routine(), EEPROM_load(), EEPROM_save(), start_timer2(), stop_timer2();

//Serial plotter.
SerialPlotter<float> plotter(Serial);

//Serial control interface.
NeoSWSerial ss(SS_RX, SS_TX);
packet_t<> packet;

//Hardware.
LMD18200 motor(LEFT_DIRECTION, RIGHT_DIRECTION);
RI32 enc(LEFT_ENCODER_A, LEFT_ENCODER_B, RIGHT_ENCODER_A, RIGHT_ENCODER_B, DELTA_T, ENC_TICKS, ENC_RADIUS, ENC_WHEELBASE);

//Controllers.
SpeedController *speedController;
PositionController *positionController;

//Current settings (loaded from EEPROM).
settings_t settings;

// Target for the position controller.
float	target_x = 0, target_y = 0, target_theta = 0;		//m, m, deg.
float target_rho = hypot(target_x, target_y);					//m.

//Tick flag.
volatile bool tick = false;

//Tick counter (used to plot data periodically).
int c = 0;

void setup(){
	Serial.begin(115200);
	ss.begin(SS_SPEED);

	EEPROM.begin();
	EEPROM_load();		//Load settings into "settings_t settings".

	speedController = new SpeedController(DELTA_T, settings.s_kp, settings.s_ki, LMD18200_PWM_MAX_VAL, motor, enc);
	positionController = new PositionController(
		DELTA_T, settings.p_module_kp, settings.p_phase_kp, settings.max_linear_speed, settings.max_angular_speed, *speedController
	);

	motor.begin();
	motor.start();

	start_timer2();
}

void loop(){
	//Interrupt occurred.
	if(tick){
		tick = false;
		positionController->evaluate(target_x, target_y, radians(target_theta));

		//Tollerance check.
		if(
			(
				enc.getRho() > target_rho - settings.tol_rho &&
				enc.getRho() < target_rho + settings.tol_theta
			) && (
				enc.getTheta() > normalize_angle(radians(target_theta) - radians(settings.tol_theta)) &&
				enc.getTheta() < normalize_angle(radians(target_theta) + radians(settings.tol_theta))
			)
		){
			//Stop engine (but continue evaluating the encoder position).
			motor.stop();
		}

		if(c++ == int(1 / (N_SAMPLES * DELTA_T))){
			c = 0;
			plotter.start();
			plotter.add(freeMemory());
			
			plotter.add(enc.getRho() * 1000);
			plotter.add(enc.getTheta(true));

			plotter.add(target_rho * 1000);
			plotter.add(target_theta);

			plotter.add(enc.getX() * 1000);
			plotter.add(enc.getY() * 1000);

			plotter.add(speedController->getLeftPWM());
			plotter.add(speedController->getRightPWM());
			
			plotter.plot();
		}
	}

	//Time to check for some serial packets.
	if(ss.available()){
		ss.readBytes((uint8_t*) &packet, sizeof(packet));
		handle_packet();
		ss.write((uint8_t*) &packet, sizeof(packet));
	}
}

inline void handle_packet(){
	packet_data_t com = packet.com;

	//Default reply values.
	packet.com = CONTROL_OK;
	packet.argc = 0;

	switch(com){		
		case COMMAND_RESET:
			enc.reset();
			break;
		
		case COMMAND_RESET_ROUTINE:
			reset_routine();
			break;
		
		case COMMAND_POSE:
			packet.argc = 3;

			packet.argv[0] = enc.getX();
			packet.argv[1] = enc.getY();
			packet.argv[2] = enc.getTheta();
			break;
		
		case COMMAND_GOTO:
			target_x = packet.argv[0];
			target_y = packet.argv[1];
			target_theta = packet.argv[2];
			target_rho = hypot(target_x, target_y);

			//Re-enable engine.
			motor.start();
			break;
		
		case COMMAND_STOP:
			packet.com = COMMAND_STOP;
			motor.stop();
			break;
		
		case COMMAND_KPID_SET:
			positionController->setModuleKp(packet.argv[0]);
			positionController->setPhaseKp(packet.argv[1]);
			speedController->setKp(packet.argv[2]);
			speedController->setKi(packet.argv[3]);
		
		//Continue after COMMAND_KPID_SET:
		case COMMAND_KPID_GET:
			packet.argc = 4;

			packet.argv[0] = positionController->getModuleKp();
			packet.argv[1] = positionController->getPhaseKp();
			packet.argv[2] = speedController->getKp();
			packet.argv[3] = speedController->getKi();
			break;
		
		case COMMAND_TOL_SET:
			settings.tol_rho = packet.argv[0];
			settings.tol_theta = packet.argv[1];
		
		case COMMAND_TOL_GET:
			packet.argc = 2;

			packet.argv[0] = settings.tol_rho;
			packet.argv[1] = settings.tol_theta;
			break;
		
		case COMMAND_MAX_SPEED_SET:
			positionController->setMaxLinearSpeed(packet.argv[0]);
			positionController->setMaxAngularSpeed(packet.argv[1]);

		case COMMAND_MAX_SPEED_GET:
			packet.argc = 2;

			packet.argv[0] = positionController->getMaxLinearSpeed();
			packet.argv[1] = positionController->getMaxAngularSpeed();
			break;
		
		case COMMAND_SAVE:
			EEPROM_save();
			break;
		
		case COMMAND_LOAD:
			EEPROM_load();
			break;
		
		case CONTROL_OK:
		case CONTROL_ERROR:
		case CONTROL_INVALID_COM:
			break;

		default:
			packet.com = CONTROL_INVALID_COM;
			break;
	}
}

inline void reset_routine(){
	//SCRIVERE!
}

inline void EEPROM_load(){
	EEPROM.get(0, settings);

	positionController->setModuleKp(settings.p_module_kp);
	positionController->setPhaseKp(settings.p_phase_kp);
	speedController->setKp(settings.s_kp);
	speedController->setKi(settings.s_ki);

	positionController->setMaxLinearSpeed(settings.max_linear_speed);
	positionController->setMaxAngularSpeed(settings.max_angular_speed);
}

inline void EEPROM_save(){
	settings.p_module_kp = positionController->getModuleKp();
	settings.p_phase_kp = positionController->getPhaseKp();
	settings.s_kp = speedController->getKp();
	settings.s_ki = speedController->getKi();

	settings.max_linear_speed = positionController->getMaxLinearSpeed();
	settings.max_angular_speed = positionController->getMaxAngularSpeed();

	EEPROM.put(0, settings);
}

//125Hz sampling rate.
#define TCNT2_OFFSET 125

inline void start_timer2(){
	TCNT2 = 0xff - TCNT2_OFFSET;
	TIMSK2 = (1 << TOIE2);

	TCCR2A = 0;
	TCCR2B = (1 << CS20) | (1 << CS21) | (1 << CS22);
}

inline void stop_timer2(){
	TIMSK2 = 0;
}

ISR(TIMER2_OVF_vect){
	TCNT2 = 0xff - TCNT2_OFFSET;
	tick = true;
}
