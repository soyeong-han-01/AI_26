/*
 * dht11.h
 *
 * Created: 2026-06-26 오전 9:26:04
 *  Author: kccistc
 */ 

#define F_CPU 16000000UL  //16MHz
#include <avr/io.h>  // PORTA PORTB PORTD... IO관련 reg가 들어 있다.
#include <util/delay.h>  // _delay_ms _delay_us 등
#include <stdio.h>
#include <string.h>