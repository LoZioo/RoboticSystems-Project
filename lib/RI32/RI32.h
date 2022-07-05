#ifndef RI32_H
#define RI32_H

#include <Arduino.h>

//RI32 and wheel parameters.
#define ENC_RADIUS	0.021
#define ENC_TICKS		4000

//ISR.
void ENC_L_ISR(), ENC_R_ISR();
inline void ENC_ISR(uint8_t pin_b, volatile int16_t &dTicks);

//Double RI32 encoder for both left and right wheels.
//That's a singleton class: use getInstance() to get the object instance.
class RI32{
	private:
		friend void ENC_L_ISR(), ENC_R_ISR();

		//Singleton locks.
		RI32();
		RI32(RI32 const& copy);
		RI32& operator=(RI32 const& copy);

		//Quadrature pins; pin_l_a and pin_r_a must be both interrupt pins.
		uint8_t pin_l_a, pin_l_b, pin_r_a, pin_r_b;

		//Sampling period.
		float dt;

		//Left and right cinematic variables.
		volatile int16_t l_dTicks = 0, r_dTicks = 0;

		float l_dTheta, l_s = 0, l_ds, l_v;
		float r_dTheta, r_s = 0, r_ds, r_v;

	public:
		//RIVEDERE QUESTA PARTE! IL PROBLEMA è CHE NON SI è CAPITO COME INSTANZIARE IL SINGLETON.


		
		//Singleton instance; usage: RI32& enc = RI32::getInstance();
		static RI32& getInstance();

		//Set parameters.
		void begin(int pin_l_a, int pin_l_b, int pin_r_a, int pin_r_b, float dt);
		
		//Must be called 1/dt times per seconds.
		void evaluate();

		//Odomerty reset.
		void reset();

		float getSpace(bool left)				{ return left ? l_s : r_s;		}
		// float getDeltaSpace(bool left)	{ return left ? l_ds : r_ds;	}
		float getSpeed(bool left)				{ return left ? l_v : r_v;		}
};

#endif
