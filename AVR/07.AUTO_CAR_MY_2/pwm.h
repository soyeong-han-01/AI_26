/*
 * pwm.h
 *
 * Created: 2026-06-18 오후 2:22:52
 *  Author: kccistc
 */ 

#ifndef PWM_H_
#define PWM_H_

#include "button.h"
#include <avr/interrupt.h>
#include <stdio.h>

void init_timer1_pwm(void);
void init_motor_driver(void);
void forward(int speed);
void backward(int speed);
void turn_left(int speed);
void turn_right(int speed);
void stop(void);

#endif /* PWM_H_ */