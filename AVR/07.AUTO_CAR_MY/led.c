/*
 * led.c
 *
 * Created: 2026-06-10 오후 3:10:22
 *  Author: user
 */ 

#include "led.h"
void init_led(void);

void init_led(void);
void led_on(void);
void led_off(void);


//extern volatile uint32_t msec_count; //volatile 최적화 방지


//int func_state=0;

void init_led(void)
{
	DDRG|= 1<<3;  // PORTG.3을 출력 모드로 설정
	PORTG &= ~(1<<3); // PORTG.3의 led off
}

void led_on(void)
{
	PORTG |=(1<<3);
}

void led_off(void)
{
	PORTG &= ~(1<<3);
}
