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

//Number of plotted samples per second (debug).
#define N_SAMPLES	30

inline void handle_packet(), reset_routine(), start_timer2(), stop_timer2();

//Serial plotter.
SerialPlotter<float> plotter(Serial);

//Serial control interface.
NeoSWSerial ss(SS_RX, SS_TX);
packet_t<> packet;

//Hardware.
LMD18200 motor(LEFT_DIRECTION, RIGHT_DIRECTION);
RI32 enc(LEFT_ENCODER_A, LEFT_ENCODER_B, RIGHT_ENCODER_A, RIGHT_ENCODER_B, DELTA_T, ENC_TICKS, ENC_RADIUS, ENC_WHEELBASE);

//Controllers.
SpeedController speedController(DELTA_T, S_KP, S_KI, LMD18200_PWM_MAX_VAL, motor, enc);
PositionController positionController(DELTA_T, P_MODULE_KP, P_PHASE_KP, ROB_MAX_SPEED, speedController);

// Target for the position controller.
float	target_x = 0, target_y = 0, target_theta = 0;		//m, m, deg.
float target_rho = hypot(target_x, target_y);					//m.

//Tollerance errors definition.
#define TOL_RHO		0.1	//m.
#define TOL_THETA	5		//deg.

//Tick flag.
volatile bool tick = false;

//Needed to lock the control system when the target is reached.
bool evaluate = false;

//Tick counter (used for debug pourposes).
int c = 0;

void setup(){
	Serial.begin(115200);
	ss.begin(SS_SPEED);

	EEPROM.begin();

	motor.begin();
	motor.start();

	start_timer2();
}

void loop(){
	//Interrupt occurred.
	if(tick){
		tick = false;

		if(evaluate){
			positionController.evaluate(target_x, target_y, radians(target_theta));

			//Tollerance check.
			if(
				(
					enc.getRho() > target_rho - TOL_RHO &&
					enc.getRho() < target_rho + TOL_RHO
				) && (
					enc.getTheta() > normalize_angle(radians(target_theta) - radians(TOL_THETA)) &&
					enc.getTheta() < normalize_angle(radians(target_theta) + radians(TOL_THETA))
				)
			){
				//Stop engine.
				motor.stop();

				//Stop control system.
				evaluate = false;
			}
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

			evaluate = true;
			break;
		
		case COMMAND_KPID_SET:
			positionController.setModuleKp(packet.argv[0]);
			positionController.setPhaseKp(packet.argv[1]);
			speedController.setKp(packet.argv[2]);
			speedController.setKi(packet.argv[3]);
		
		//Continue after COMMAND_KPID_SET:
		case COMMAND_KPID_GET:
			packet.argc = 4;

			packet.argv[0] = positionController.getModuleKp();
			packet.argv[1] = positionController.getPhaseKp();
			packet.argv[2] = speedController.getKp();
			packet.argv[3] = speedController.getKi();
			break;
		
		case COMMAND_KPID_SAVE:
			EEPROM.put(0, packet);
		
		//Continue after COMMAND_KPID_SAVE:
		case COMMAND_KPID_LOAD:
			EEPROM.get(0, packet);

			packet.argc = 4;

			packet.argv[0] = positionController.getModuleKp();
			packet.argv[1] = positionController.getPhaseKp();
			packet.argv[2] = speedController.getKp();
			packet.argv[3] = speedController.getKi();
			break;
		
		case CONTROL_OK:
		case CONTROL_ERROR:
		case CONTROL_INVALID_MSG:
			break;

		default:
			packet.com = CONTROL_INVALID_MSG;
			break;
	}
}

inline void reset_routine(){
	//SCRIVERE!
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
