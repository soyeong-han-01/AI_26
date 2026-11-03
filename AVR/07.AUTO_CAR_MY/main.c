#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


#include "button.h"
#include "led.h"
#include "ultrasonic.h"
#include "pwm.h"
#include "def.h"
#include "fnd.h"



void auto_mode(void);
void manual_mode(void);
void mode_check(void);



extern volatile uint8_t bt_data;


volatile uint8_t mode = MANUAL_MODE;





void manual_mode(void)
{
	switch(bt_data)
	{
		case 'F':
		case 'f':
		forward(500);
		break;

		case 'B':
		case 'b':
		backward(500);
		break;

		case 'L':
		case 'l':
		turn_left(700);
		break;

		case 'R':
		case 'r':
		turn_right(700);
		break;
		
		case 'S':
		case 's':
		stop();
		break;
		
		default:
		break;
	}
}







void mode_check(void)
{

	if(get_button(BUTTON0,BUTTON0PIN))
	{
		switch(mode)
		{
			case MANUAL_MODE:
			// 매뉴얼 -> 자동
			fnd_reset_record();
			mode = AUTO_MODE;
			led_on();
			break;

			case AUTO_MODE:
			// 자동 -> 종료
			stop();
			mode = END_MODE;
			led_off();
			break;
			case END_MODE:

			// 종료 -> 매뉴얼
			mode = MANUAL_MODE;
			fnd_reset_record();
			stop();
			break;
		}
		_delay_ms(200);
	}
}


void auto_mode(void)
{

	// 앞이 비었으면 직진
	if(center_distance > 20 ||
	center_distance == 0)
	{
		forward(600);
		return;
	}

	// 장애물 발견
	if(center_distance >0 &&
	center_distance <=20)
	{
		stop();
		_delay_ms(40);
		// 후진
		backward(400);
		_delay_ms(120);
		stop();
		_delay_ms(30);

		// 방향 결정
		if(left_distance > right_distance)
		{
			turn_left(600);
			_delay_ms(150);
		}

		else
		{
			turn_right(600);
			_delay_ms(150);
		}
		stop();
		_delay_ms(30);
		return;
	}
	// 벽에 부딪히지 않도록
	if(left_distance >0 &&
	right_distance >0)
	{
		int diff;
		diff = left_distance - right_distance;
		if(diff > 12)
		{
			turn_left(350);
			_delay_ms(25);
			return;
		}
		else if(diff < -12)
		{
			turn_right(350);
			_delay_ms(25);
			return;
		}
	}
	forward(600);
}














int main(void)
{

	init_led();
	init_button();
	init_ultrasonic();
	init_timer1_pwm();
	init_motor_driver();
	init_uart1();
	init_fnd();

	// Timer0 1ms
    TCNT0 = 6;
    TCCR0 &= ~(1<<CS02 | 1<<CS01 | 1<<CS00);

    // 64분주 설정
    TCCR0 |= (1<<CS02);
    TIMSK |= (1<<TOIE0);

	sei();
	stop();
	led_off();

	while(1)
	{

		// 초음파 처리
		ultrasonic_processing();

		// 버튼 확인
		mode_check();

		switch(mode)
		{
			case MANUAL_MODE:
			manual_mode();
			break;
			
			case AUTO_MODE:
			auto_mode();
			break;

			case END_MODE:
			stop();
			fnd_end_view();
			break;
		}
	}
	return 0;
}