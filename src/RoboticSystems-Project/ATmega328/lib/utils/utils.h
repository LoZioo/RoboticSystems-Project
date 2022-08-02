/**
 * @file utils.h
 * @author Davide Scalisi
 * @brief 
 * @version 0.1
 * @date 2022-08-01
 * 
 * @copyright Copyright (c) 2022
 * 
 * Set of auxiliary functions.
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

#endif
