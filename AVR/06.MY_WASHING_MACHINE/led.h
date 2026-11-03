/*
 * led.h
 *
 * Created: 2026-06-18 오후 6:40:11
 *  Author: kccistc
 */ 

#define F_CPU 16000000UL  // 16MHz 주파수 설정
#include <avr/io.h>       // PORTA, DDRA 레지스터 도구 상자

#define LED0  0  // 세탁 상태 LED (0번 핀)
#define LED1  1  // 헹굼 상태 LED (1번 핀)
#define LED2  2  // 탈수 상태 LED (2번 핀)

void init_led(void);
void standby_led(void);
void washing_led(void);
void rinse_led(void);
void dry_led(void);
