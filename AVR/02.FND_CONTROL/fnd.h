/*
 * fnd.h
 *
 * Created: 2026-06-12 오전 10:46:03
 *  Author: kccistc
 */ 

#define F_CPU 16000000UL
#include <avr/io.h> //포트,디디알 등등
#include <util/delay.h>

#define FND_DATA_DDR DDRC
#define FND_DATA_PORT PORTC

#define FND_DIGIT_DDR DDRB
#define FND_DIGIT_PORT PORTB
#define FND_DIGIT_D1 4
#define FND_DIGIT_D2 5
#define FND_DIGIT_D3 6
#define FND_DIGIT_D4 7


