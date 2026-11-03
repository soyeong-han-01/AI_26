/*
 * ultrasonic.h
 *
 * Created: 2026-06-17 오후 1:32:44
 *  Author: kccistc
 */ 

#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define TRIG_DDR DDRA
#define TRIG_PORT PORTA

#define TRIG_PIN_left     0
#define TRIG_PIN_center   1
#define TRIG_PIN_right    2

#define ECHO_DDR DDRE
#define ECHO_PORT PINE

#define ECHO_PIN_left     4
#define ECHO_PIN_center   5
#define ECHO_PIN_right    6

extern volatile int left_distance;
extern volatile int center_distance;
extern volatile int right_distance;

extern volatile int ultrasonic_check_time;

void init_ultrasonic(void);
void ultrasonic_processing(void);

void trigger_left(void);
void trigger_center(void);
void trigger_right(void);

#endif /* ULTRASONIC_H_ */