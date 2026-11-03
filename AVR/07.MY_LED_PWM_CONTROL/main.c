/*
 * 07.MY_LED_PWM_CONTROL.c
 *
 * Created: 2026-06-18 오전 10:25:30
 * Author : kccistc
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#define LED_TIME 20
#define PB4 4
#define PB5 5
#define PB6 6

#if 1
int main(void)
{
	DDRB |= (1<<PB5) | (1<<PB6);
	TCCR1A |= (1 << WGM10);
	TCCR1A |= (1<<COM1A1);
	TCCR1A |= (1 << COM1B1)|(1<<COM1B0);
	TCCR1B |= (1 << CS12); 
	
	int dim=0;
	int direction =1;
	
	while(1)
	{
		OCR1A = dim;
		OCR1B = dim;
		
		_delay_ms(10);
		
		dim += direction;
		if(dim==0) direction=1;
		if(dim==255) direction= -1;
	}
	return 0;
}
#endif
#if 0
int main(void)
{
	int dim=0;
	int direction=1;
	
	
	DDRB |= (1 << PB4);
	TCCR0 |= (1<<WGM01) | (1 << WGM00);
	TCCR0 |= (1<<COM01);
	TCCR0 |= (1<<CS02)|(1<<CS01) | (1<<CS00);
	
	while(1)
	{
		OCR0 = dim;
		_delay_ms(10);
		
		dim+=direction;
		
		if (dim==0) direction=1;
		if(dim==255)direction=-1;
	}
	return 0;
}

void turn_on_LED_in_PWM_manner(int dim)
{
	int i;

	PORTA = 0x01; //출력
	
	for(i=0; i<256;i++)
	{
		if(i>dim) PORTA=0x00; //끄기
		_delay_us(LED_TIME);
	}
}
#endif

