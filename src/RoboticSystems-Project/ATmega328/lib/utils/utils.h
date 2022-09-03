/**
 * @file utils.h
 * @author Davide Scalisi
 * @brief Set of auxiliary functions.
 * @version 0.1
 * @date 2022-08-01
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

//Angle normalization in range [-PI, PI].
inline float normalize_angle(float a){
	while(a > PI)
		a -= 2 * PI;
	
	while(a < -PI)
		a += 2 * PI;
	
	return a;
}

//Free SRAM memory.
#ifdef __arm__
	//Should use uinstd.h to define sbrk but Due causes a conflict.
	extern "C" char* sbrk(int incr);

#else		//__ARM__
	extern char *__brkval;

#endif	//__arm__

inline int freeMemory(){
  char top;

	#ifdef __arm__
		return &top - reinterpret_cast<char*>(sbrk(0));
	
	#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
		return &top - __brkval;
	
	#else  // __arm__
		return __brkval ? &top - __brkval : &top - __malloc_heap_start;
	
	#endif  // __arm__
}

#endif
