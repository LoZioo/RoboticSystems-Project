/**
 * @file main.cpp
 * @author Davide Scalisi
 * @brief 
 * @version 1.0
 * @date 2022-08-28
 * 
 * @copyright Copyright (c) 2022
 * 
 * Main control loop of the cart.
 */

#include <Arduino.h>
#include <EEPROM.h>
#include <NeoSWSerial.h>

#include <const.h>
#include <packet.h>

#include <LMD18200.h>
#include <RI32.h>
#include <PID.h>
#include <Controllers.h>

#include <SerialPlotter.h>
#include <SerialPlotter.cpp>	//Just to avoid linker errors.

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
float	target_x = 0, target_y = 0, target_rho = hypot(target_x, target_y);

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

	start_timer2();
}

void loop(){
	//Interrupt occurred.
	if(tick){
		tick = false;

		//Read new data.
		enc.evaluate();

		if(motor.enabled())
			//Main engine controller.
			positionController->evaluate(target_x, target_y);

		//Tollerance check.
		if(
			enc.getRho() > target_rho - settings.tol_rho &&
			enc.getRho() < target_rho + settings.tol_rho
		){
			//Stop engine (but continue evaluating the encoder position).
			motor.stop();
			positionController->reset();
		}

		if(c++ == int(1 / (N_SAMPLES * DELTA_T))){
			c = 0;
			plotter.start();
			
			plotter.add(enc.getX() * 1000);
			plotter.add(enc.getY() * 1000);
			
			plotter.add(enc.getRho() * 1000);
			plotter.add(target_rho * 1000);

			plotter.add(speedController->getLeftPWM());
			plotter.add(speedController->getRightPWM());

			plotter.add(positionController->getTargetLinearSpeed());
			plotter.add(positionController->getTargetAngularSpeed());
			
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
			target_rho = hypot(target_x, target_y);

			//Re-enable engine.
			motor.start();
			break;
		
		case COMMAND_START:
			motor.start();
			break;
		
		case COMMAND_STOP:
			motor.stop();
			break;
		
		case COMMAND_KPID_SET:
			settings.p_module_kp = packet.argv[0];
			settings.p_phase_kp = packet.argv[1];
			settings.s_kp = packet.argv[2];
			settings.s_ki = packet.argv[3];
		
		//Continue after COMMAND_KPID_SET:
		case COMMAND_KPID_GET:
			packet.argc = 4;

			packet.argv[0] = settings.p_module_kp;
			packet.argv[1] = settings.p_phase_kp;
			packet.argv[2] = settings.s_kp;
			packet.argv[3] = settings.s_ki;
			break;
		
		case COMMAND_TOL_SET:
			settings.tol_rho = packet.argv[0];
				
		case COMMAND_TOL_GET:
			packet.argc = 1;
			packet.argv[0] = settings.tol_rho;
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
	//Reset routine with switches (unused).
}

inline void EEPROM_load(){
	EEPROM.get(0, settings);
}

inline void EEPROM_save(){
	EEPROM.put(0, settings);
}

//Timer2 configurations: 125Hz sampling rate.
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
