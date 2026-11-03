/*
 * ds1302.h
 *
 * Created: 2026-06-26 오후 2:42:08
 *  Author: kccistc
 */ 

#define F_CPU 16000000UL  //16MHz
#include <avr/io.h>  // PORTA PORTB PORTD... IO관련 reg가 들어 있다.
#include <util/delay.h>  // _delay_ms _delay_us 등
#include <stdio.h>
#include <string.h>

#define DS1302_CLK_DDR DDRF
#define DS1302_CLK_PORT PORTF

#define DS1302_DAT_DDR DDRF
#define DS1302_DAT_PORT PORTF //write
#define DS1302_DAT_PIN PINF //read

#define DS1302_RST_DDR DDRF //reset
#define DS1302_RST_PORT PORTF

#define DS1302_CLK 0 //0번 pin
#define DS1302_DAT 1 //0번 pin
#define DS1302_RST 2 //0번 pin

#define ADDR_SECONDS 0x80 //write만 define하자 read는 write에 +1만 하면 되니까
#define ADDR_MINUTES 0x82
#define ADDR_HOUR 0x84
#define ADDR_DATE 0x86
#define ADDR_MONTH 0x88
#define ADDR_DAYOFWEEK 0x8a
#define ADDR_YEAR 0x8c
#define ADDR_WRITEPROTECTED 0x8e

typedef struct _ds1302
{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t date;
	uint8_t month;
	uint8_t dayofweek; //1:sun 2:mon
	uint8_t year;
	uint8_t ampm; 
	uint8_t hourmode; //0:24 1:12
	}t_ds1302;

t_ds1302 ds1302; //전역변수를 지역변수로???