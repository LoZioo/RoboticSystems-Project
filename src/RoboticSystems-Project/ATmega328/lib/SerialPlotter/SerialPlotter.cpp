#include <SerialPlotter.h>

template<class T>
void SerialPlotter<T>::start(){
	append = true;
	sp = 0;
}

template<class T>
void SerialPlotter<T>::add(T sample){
	if(append){
		data.payload[constrain(sp, 0, PAYLOAD_MAXCHANNELS - 1)] = sample;
		sp++;
	}
}

template<class T>
void SerialPlotter<T>::plot(){
	if(append){
		append = false;

		data.payload_length = sp * sizeof(T);
		serial.write((uint8_t*) &data, sizeof(SerialPlotter_frame_t));

		// char str[100];
		// sprintf(str, "%d * %d = %d", sp, sizeof(T), data.payload_length);
		// serial.println(str);
	}
}
