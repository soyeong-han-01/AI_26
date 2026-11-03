/*
 * led.c
 *
 * Created: 2026-06-18 오후 6:40:46
 *  Author: kccistc
 */ 
#include "led.h"

// 포트A의 0, 1, 2번 핀을 신호 출력 모드로 초기화
void init_led(void)
{
	DDRA |= (1 << LED0) | (1 << LED1) | (1 << LED2);
}

// 대기 상태: 0, 1, 2번 세탁 코스 LED를 모두 소등
void standby_led(void)
{
	PORTA &= ~((1 << LED0) | (1 << LED1) | (1 << LED2));
}

// 세탁 상태
void washing_led(void)
{
	PORTA |= (1 << LED0);
	PORTA &= ~((1 << LED1) | (1 << LED2));
}

// 헹굼 상태
void rinse_led(void)
{
	PORTA |= (1 << LED1);
	PORTA &= ~((1 << LED0) | (1 << LED2));
}

// 탈수 상태
void dry_led(void)
{
	PORTA |= (1 << LED2);
	PORTA &= ~((1 << LED0) | (1 << LED1));
}
