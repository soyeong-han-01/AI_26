/*
 * ultrasonic.c
 *
 * Created: 2026-06-17
 * Author : kccistc
 */

/*
 * ultrasonic.c
 */

#include "ultrasonic.h"
#include "fnd.h"

#include <avr/interrupt.h>
#include <util/delay.h>


volatile int ultrasonic_check_time = 0;


volatile int left_distance = 0;
volatile int center_distance = 0;
volatile int right_distance = 0;


volatile uint16_t left_start = 0;
volatile uint16_t center_start = 0;
volatile uint16_t right_start = 0;

extern volatile uint16_t drive_time; //추가한거
volatile uint8_t sensor_state = 0;



ISR(INT4_vect)
{

	if(ECHO_PORT & (1<<ECHO_PIN_left))
	{
		left_start = TCNT3;
	}
	else
	{
		uint16_t pulse;
		pulse = TCNT3 - left_start;
		left_distance =
		(int)((double)pulse * 64.0 / 58.0);
	}
}

ISR(INT5_vect)
{
	if(ECHO_PORT & (1<<ECHO_PIN_center))
	{
		center_start = TCNT3;
	}
	else
	{
		uint16_t pulse;
		pulse = TCNT3 - center_start;
		center_distance =
		(int)((double)pulse * 64.0 / 58.0);
	}
}

ISR(INT6_vect)
{
	if(ECHO_PORT & (1<<ECHO_PIN_right))
	{
		right_start = TCNT3;
	}
	else
	{
		uint16_t pulse;
		pulse = TCNT3 - right_start;
		right_distance =
		(int)((double)pulse * 64.0 / 58.0);
	}
}






// Timer0 Overflow
// 1ms 주기
ISR(TIMER0_OVF_vect)
{

	static uint16_t ms=0;
	static uint16_t end_ms=0;


	TCNT0 = 6;



	// FND 빠른 스캔

	fnd_scan();




	// ===================
	// 주행 시간
	// ===================

	if(car_state != STOP_STATE)
	{

		ms++;


		if(ms>=1000)
		{

			ms=0;

			drive_time++;

		}

	}





	// ===================
	// 종료 화면 시간
	// ===================

	if(car_state == STOP_STATE)
	{

		end_ms++;


		if(end_ms>=2000)
		{

			end_ms=0;

			fnd_end_change();

		}

	}
	else
	{

		end_ms=0;

	}





	// 초음파

	ultrasonic_check_time++;

}







void init_ultrasonic(void)
{


	// TRIG 출력

	TRIG_DDR |=
	(1<<TRIG_PIN_left) |
	(1<<TRIG_PIN_center) |
	(1<<TRIG_PIN_right);




	// ECHO 입력

	ECHO_DDR &= ~(
	(1<<ECHO_PIN_left) |
	(1<<ECHO_PIN_center) |
	(1<<ECHO_PIN_right)
	);




	// INT4 INT5 INT6

	EICRB =
	(1<<ISC40) |
	(1<<ISC50) |
	(1<<ISC60);




	EIMSK |=
	(1<<INT4) |
	(1<<INT5) |
	(1<<INT6);





	// Timer3 거리 측정용

	TCCR3A = 0x00;


	TCCR3B =
	(1<<CS32) |
	(1<<CS30);



	TCNT3=0;

}









void trigger_left(void)
{

	TCNT3=0;
	TRIG_PORT &= ~(1<<TRIG_PIN_left);
	_delay_us(2);

	TRIG_PORT |= (1<<TRIG_PIN_left);
	_delay_us(15);

	TRIG_PORT &= ~(1<<TRIG_PIN_left);
}

void trigger_center(void)
{

	TCNT3=0;
	TRIG_PORT &= ~(1<<TRIG_PIN_center);
	_delay_us(2);

	TRIG_PORT |= (1<<TRIG_PIN_center);
	_delay_us(15);

	TRIG_PORT &= ~(1<<TRIG_PIN_center);
}

void trigger_right(void)
{
	TCNT3=0;
	TRIG_PORT &= ~(1<<TRIG_PIN_right);
	_delay_us(2);

	TRIG_PORT |= (1<<TRIG_PIN_right);
	_delay_us(15);

	TRIG_PORT &= ~(1<<TRIG_PIN_right);
}









void ultrasonic_processing(void)
{

	if(ultrasonic_check_time < 50)
	return;
	ultrasonic_check_time=0;

	switch(sensor_state)
	{
	  case 0:
		trigger_left();
		sensor_state=1;
	    break;
		
	case 1:
		trigger_center();
		sensor_state=2;
	    break;

	case 2:
		trigger_right();
		sensor_state=0;
	    break;
	}
}