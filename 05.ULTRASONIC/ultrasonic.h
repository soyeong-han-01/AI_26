/*
 * ultrasonic.h
 *
 * Created: 2026-06-17 오후 1:32:44
 *  Author: kccistc
 */ 

#define F_CPU 16000000UL //이걸 먼저해야함
#include <avr/io.h> //포트,디디알 등등
#include <util/delay.h>
#include <avr/interrupt.h> //sei 등 함수

#define TRIG_DDR DDRG
#define TRIG_PORT PORTG
#define TRIG_PIN  4

#define ECHO_DDR DDRE
#define ECHO_PORT PINE //External 인터럽트 4번 
#define ECHO_PIN 4