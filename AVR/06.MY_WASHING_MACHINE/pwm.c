/*
 * pwm.c
 *
 * Created: 2026-06-18 오후 2:23:04
 *  Author: kccistc
 */ 
#include "pwm.h"

void init_timer3_pwm(void)
{
	DDRE |= (1 << 3) | (1 << 5);
	
	//모드5 : 8비트 고속 pwm 모드 사용 하겠다.
	TCCR3A |= (1 << WGM30);
	TCCR3B |= (1 << WGM32);
	
	//비반전 모드 top : 0x00ff 비교일치 값(pwm) 지정 OCR3C p350 표15-7
	TCCR3A |= (1 << COM3C1);
	TCCR3B |= (1 << CS31) | (1 << CS30); //분주비 64
	OCR3C = 0;
}

void init_motor_driver(void)
{
	DDRF |= (1 << 6) | (1 << 7);
	PORTF &= ~((1 << 6) | (1 << 7));
}
