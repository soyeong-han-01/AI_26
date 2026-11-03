/*
 * 10.MY_DS1302_burst.c
 *
 * Created: 2026-06-29 오후 6:57:23
 * Author : kccistc
 */ 

#define F_CPU 16000000UL //이걸 먼저해야함
#include <avr/io.h> //포트,디디알 등등
#include <util/delay.h>
#include <avr/interrupt.h> //sei 등 함수
#include <stdio.h>

extern void init_uart0(void);
extern void UART0_transmit(uint8_t data);
extern void dht11_main(void);

FILE OUTPUT = FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE);

int main(void)
{
	init_uart0();
	stdout = &OUTPUT; //printf가 동작 할 수 있도록 stdout을 설정.
	ds1302_main();
	
	while (1)
	{
		dht11_main();
		_delay_ms(1500); // DHT11 안정화 시간(스펙상)
	}
}

