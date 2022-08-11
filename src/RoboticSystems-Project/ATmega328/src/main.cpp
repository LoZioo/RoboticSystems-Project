#include <Arduino.h>
#include <const.h>

#include <SerialPlotter.h>
#include <SerialPlotter.cpp>	//Just to avoid linker errors.

#include <LMD18200.h>
#include <RI32.h>
#include <PID.h>
#include <Controllers.h>

#include <NeoSWSerial.h>
#include <packet.h>

//Number of plotted samples per second (debug).
#define N_SAMPLES	30

//Tollerance errors definition.
#define TOL_RHO		0.01	//m.
#define TOL_THETA	1			//deg.

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

//Target for the position controller.
float	target_x = 0, target_y = 0, target_theta = 0;		//m, m, deg.
float target_rho = 0;																	//hypot(target_x, target_y);

//Tick flag.
volatile bool tick = false;

//It needs to lock the control system when the target is reached.
bool evaluate = true;

//Tick counter (used for debug pourposes).
int c = 0;

void setup(){
	Serial.begin(115200);
	ss.begin(SS_SPEED);

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
			
			plotter.add(enc.getX() * 1000);
			plotter.add(enc.getY() * 1000);
			plotter.add(enc.getRho() * 1000);
			plotter.add(enc.getTheta(true));

			plotter.add(target_rho * 1000);
			plotter.add(target_theta);
			
			plotter.add(speedController.getLeftPWM());
			plotter.add(speedController.getRightPWM());

			plotter.plot();
		}
	}

	//Time to check for some serial packets.
	else{
		if(ss.available()){
			ss.readBytes((uint8_t*) &packet, sizeof(packet));
			handle_packet();
			ss.write((uint8_t*) &packet, sizeof(packet));
		}
	}
}

inline void handle_packet(){
	//Check for CRC8 errors.
	if(crc8_packet<>(packet) != packet.crc8){
		packet.payload.msg = CONTROL_WRONG_CRC;
		packet.payload.argc = 0;
		return;
	}

	packet_command_t com = packet.payload.msg;

	//Default reply values.
	packet.payload.msg = CONTROL_OK;
	packet.payload.argc = 0;

	switch(com){		
		case COMMAND_RESET:
			enc.reset();
			break;
		
		case COMMAND_RESET_ROUTINE:
			reset_routine();
			break;
		
		case COMMAND_POSE:
			packet.payload.argc = 3;

			packet.payload.argv[0] = enc.getX();
			packet.payload.argv[1] = enc.getY();
			packet.payload.argv[2] = enc.getTheta();
			break;
		
		case COMMAND_GOTO:
			target_x = packet.payload.argv[0];
			target_y = packet.payload.argv[1];
			target_theta = packet.payload.argv[2];
			target_rho = hypot(target_x, target_y);
			break;
		
		case COMMAND_KPID_SET:
			positionController.setModuleKp(packet.payload.argv[0]);
			positionController.setPhaseKp(packet.payload.argv[1]);
			speedController.setKp(packet.payload.argv[2]);
			speedController.setKi(packet.payload.argv[3]);
		
		//Continue after COMMAND_KPID_SET:
		case COMMAND_KPID_GET:
			packet.payload.argc = 4;

			packet.payload.argv[0] = positionController.getModuleKp();
			packet.payload.argv[1] = positionController.getPhaseKp();
			packet.payload.argv[2] = speedController.getKp();
			packet.payload.argv[3] = speedController.getKi();
			break;
		
		case CONTROL_OK:
		case CONTROL_WRONG_CRC:
		case CONTROL_INVALID_MSG:
			break;

		default:
			packet.payload.msg = CONTROL_INVALID_MSG;
			break;
	}

	packet.crc8 = crc8_packet<>(packet);
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
