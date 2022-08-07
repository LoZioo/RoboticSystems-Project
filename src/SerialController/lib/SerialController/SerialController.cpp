#include <SerialController.h>

//------------------------------ SerialController_Target ------------------------------//

bool SerialController_target::__decode(SerialController_data_t &data, String &com, String *args, uint8_t len){
	//DEFINIRE I VARI COMANDI, SCRIVERE DECODE ED INFINE SCRIVERE ENCODE.

	return false;
}

SerialController_target::SerialController_target(uint8_t rx, uint8_t tx){
	ss = new NeoSWSerial(rx, tx);
}

SerialController_target::~SerialController_target(){
	delete ss;
}

void SerialController_target::begin(uint32_t baudrate){
	ss->begin(baudrate);
}

bool SerialController_target::recv(SerialController_data_t &data){
	if(ss->available()){
		String str = ss->readString();

		//Remove CrLf from the string.
		while(str[str.length() - 1] == '\r' || str[str.length() - 1] == '\n')
			str.remove(str.length() - 1);
		
		//Remove spaces.
		str.replace(" ", "");

		//Ping.
		if(str == "AT"){
			ss->println("OK");
			return false;
		}

		//Commands.
		if(str.substring(0, 2) == "AT"){
			bool decode_res;

			if(str[str.length() - 1] == '?'){

				//Test command.
				if(str[str.length() - 2] == '=')
					str = str.substring(2, str.length() - 2);

				//Read command.
				else
					str = str.substring(2, str.length() - 1);

				decode_res = __decode(data, str);
			}

			//Set or execution command.
			else{
				const uint8_t eq_index = str.indexOf('=');
				
				//Args.
				String args = str.substring(eq_index + 1);
				
				//Command.
				str = str.substring(2, eq_index);
				
				//Decode args.
				String arg[SERIAL_CONTROLLER_ARG_MAX_LEN];

				int i = 1;
				char *tmp;

				arg[0] = tmp = strtok((char*) args.c_str(), ",");

				while(i < SERIAL_CONTROLLER_ARG_MAX_LEN && tmp != NULL)
					arg[i++] = tmp = strtok(NULL, ",");
				
				if(tmp == NULL)
					i--;

				decode_res = __decode(data, str, arg, i);
			}

			return decode_res;
		}

		ss->println("ERROR");
	}

	return false;
}

void SerialController_target::send(SerialController_data_t &data){}

//------------------------------- SerialController_Host -------------------------------//
