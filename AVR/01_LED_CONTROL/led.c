/*
 * led.c
 *
 * Created: 2026-06-10 오후 3:10:22
 *  Author: user
 */ 

#include "led.h"
void init_led(void);
void led_all_on(void);
void led_all_off(void);
void led_shift_left_on(void);
void led_shift_right_on(void);
void led_shift_left_keep_on(void);
void led_shift_right_keep_on(void);
void led_flower_on(void);
void led_flower_off(void);


int led_main(void)
{
	led_all_off();
	while(1)
	{
		led_shift_left_on();
		led_shift_right_on();
	}
}

void init_led(void)
{
	DDRA=0xff;  // PORTA 를 출력 모드로 설정
	PORTA=0x00;  // PORTA에 물려있는 led를 all off 	
}

void led_all_on(void)
{
	PORTA=0xff;
}

void led_all_off(void)
{
	PORTA=0x00;
}

void led_flower_on(void)
{
	static int i=0;
	if(i==0) PORTA=0b00000000;
	PORTA |=(0b00010000)<<i|(0b00001000)>>i;
	_delay_ms(100);
	i = (i+1)%4;
	
	/*PORTA=0b00011000;
	for(int i=0;i<=4;i++)
	{
		
		PORTA|=(0b00010000>>i)|(0b00001000<<i);

	    _delay_ms(100);
	}*/
}

void led_flower_off(void)
{
	static int i=0;
	if(i==0) PORTA=0xff;
	PORTA &=~((0b10000000>>i)|(0b00000001<<i));
	_delay_ms(100);
	i=(i+1)%4;
	
	/*PORTA = 0xFF;
	_delay_ms(100);
	for(int i=0;i<=3;i++)
	{
		PORTA &=~((0b10000000>>i)|(0b00000001<<i));
		_delay_ms(100);
	}*/
}
void led_shift_left_keep_on(void)
{
	static int i=0;
	if(i==0)PORTA=0b00000000;
	
	PORTA |=1<<i;
	
	_delay_ms(100);
	i=(i+1)%8;
	/*for(int i=0;i<=8;i++)
	{
		PORTA = ~(0xff<<i);
		_delay_ms(100);
	}*/
}

void led_shift_right_keep_on(void)
{
	static int i=0;
	if(i==0) PORTA=0b00000000;
	
	PORTA |=0x80>>i;
	_delay_ms(100);
	i=(i+1)%8;
	/*for(int i=0;i<=8;i++)
	{
		PORTA =~(0xff>>i);
		_delay_ms(100);
	}*/
}
void led_shift_left_on(void)
{
		static int i=0;
		
		PORTA=1<<i;
		_delay_ms(100);
		
		i=(i+1)%8; //다음 인덱스 값을 계산
		/*for(int i=0;i<8;i++)
		{
			PORTA =1 <<i;//0x18번지
			_delay_ms(100);
		}*/
}

void led_shift_right_on(void)
{
	static int i=0;
	
	PORTA=0x80>>i;
	_delay_ms(100);
	
	i=(i+1)%8;
	/*for(int i=7;i>=0;i--)
	{
		PORTA =1 <<i;//0x18번지
		_delay_ms(100);
	}*/
}
