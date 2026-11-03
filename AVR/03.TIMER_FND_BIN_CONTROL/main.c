/*
 * 03.TIMER_CONTROL.c
 */ 

#define F_CPU 16000000UL 
#include <avr/io.h> 
#include <util/delay.h>
#include <avr/interrupt.h> 

void init_timer2(void);
extern int fnd_main(void);

volatile uint32_t msec_count = 0;

ISR(TIMER2_OVF_vect)
{
	TCNT2 = 192;
	msec_count++; 
}

int main(void)
{
	init_timer2();
	
	fnd_main(); 
	
	return 0;
}

void init_timer2(void)
{
   TCNT2 = 192;
   
   TCCR2 &= ~(1 << CS22 | 1 << CS21 | 1 << CS20); 
   TCCR2 |= (1 << CS22) | (0 << CS21) | (0 << CS20); 
   
   TIMSK |= (1 << TOIE2);
   
   sei(); 
}






