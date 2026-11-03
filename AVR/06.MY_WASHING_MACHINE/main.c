/*
 * 06.MY_WASHING_MACHINE.c
 *
 * Created: 2026-06-18 오후 6:33:09
 * Author : kccistc
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "button.h"
#include "led.h"
#include "fnd.h"
#include "pwm.h"

#define SELECT_WASHING 4

enum Washing_System_State {
	STATE_STANDBY,
	STATE_WASH,
	STATE_RINSE,
	STATE_DRY
};

enum Washing_System_State washing_state = STATE_STANDBY;

volatile int is_running = 0;
volatile int total_remaining_sec = 0;
volatile int time_over_flag = 0;
volatile uint32_t ms_count = 0;
volatile uint32_t msec_count = 0;

volatile uint32_t motor_direction_count = 0;
volatile int forward = 1;

int wash_set_min = 0;
int rinse_set_min = 0;
int dry_set_min = 0;

void (*washing_fp[])(void) = { standby_led, washing_led, rinse_led, dry_led };

int washing_button_func(void)
{
	if(get_button(BUTTON0, BUTTON0PIN)) return 0;
	if(get_button(BUTTON1, BUTTON1PIN)) return 1;
	if(get_button(BUTTON2, BUTTON2PIN)) return 2;
	return -1;
}

ISR(TIMER0_OVF_vect)
{
	TCNT0 = 6;
	msec_count++;
	
	if (is_running == 1)
	{
		motor_direction_count++; 
		ms_count++;
		
		// 10초마다 모터 방향 자동 전환
		if (washing_state == STATE_WASH || washing_state == STATE_RINSE)
		{
			if (motor_direction_count >= 10000) //10초가 넘어가면
			{
				motor_direction_count = 0;
				forward = !forward;
				
				PORTF &= ~((1 << 6) | (1 << 7));
				if (forward) PORTF |= (1 << 6);
				else         PORTF |= (1 << 7);
			}
		}
		
		/* 1초 주기 세탁 시간 차감 */
		if (ms_count >= 1000)
		{
			ms_count = 0;
			
			if (total_remaining_sec > 0)
			{
				total_remaining_sec--;
				
				if (washing_state == STATE_WASH) 
				{
					if (total_remaining_sec == (rinse_set_min + dry_set_min) * 60) time_over_flag = 1;
				}
				else if (washing_state == STATE_RINSE)
				{
					if (total_remaining_sec == (dry_set_min * 60)) time_over_flag = 1;
				}
				else if (washing_state == STATE_DRY)
				{
					if (total_remaining_sec == 0) time_over_flag = 1;
				}
			}
		}
	}
}

int main(void)
{
	init_led();
	init_button();
	init_fnd();
	init_timer3_pwm();
	init_motor_driver();
	
	TCCR0 |= (1 << CS02) | (0 << CS01) | (1 << CS00);
	TIMSK |= (1 << TOIE0);
	TCNT0 = 6;
	
	sei();
	standby_led();

	while(1)
	{
		int btn = washing_button_func();

		if (btn == 0) //0번 버튼이 눌리면
		{
			if (is_running == 0)
			{
				total_remaining_sec = (wash_set_min + rinse_set_min + dry_set_min) * 60;
				if (total_remaining_sec > 0)
				{
					is_running = 1; //가동중으로 변경
					ms_count = 0; 
					msec_count = 0;
					motor_direction_count = 0;
					forward = 1;
					
					PORTF &= ~((1 << 6) | (1 << 7));
					PORTF |= (1 << 6);
					
					washing_state = STATE_WASH;
				}
			}
			else //작동중이라면
			{
				is_running = 0; //멈추기
				OCR3C = 0;
				PORTF &= ~((1 << 6) | (1 << 7));
				wash_set_min=0;
				rinse_set_min=0;
				dry_set_min=0;
				washing_state = STATE_STANDBY;
			}
		}

		// 모드 변경 및 시간 설정
		if (is_running == 0) // 작동을 하지 않은 상태에서
		{
			if (btn == 1) //버튼 1번이 눌린다면 모드 바꾸기
			{
				if (washing_state == STATE_STANDBY)      washing_state = STATE_WASH;
				else if (washing_state == STATE_WASH)    washing_state = STATE_RINSE;
				else if (washing_state == STATE_RINSE)   washing_state = STATE_DRY;
				else if (washing_state == STATE_DRY)           washing_state = STATE_STANDBY;
			}

			if (btn == 2) //버튼 2번이 눌린다면 작동 시간(분) 증가
			{
				if (washing_state == STATE_WASH)        wash_set_min++;
				else if (washing_state == STATE_RINSE)  rinse_set_min++;
				else if (washing_state == STATE_DRY)          dry_set_min++;
			}
		}

		// 시간 흐름에 따른 작동 상황 변경
		if (is_running == 1 && time_over_flag == 1)
		{
			time_over_flag = 0;
			
			if (washing_state == STATE_WASH)
			{
				washing_state = STATE_RINSE;
				motor_direction_count = 0; 
				forward = 1;
				PORTF &= ~((1 << 6) | (1 << 7));
				PORTF |= (1 << 6);
			}
			else if (washing_state == STATE_RINSE)
			{
				washing_state = STATE_DRY;
				forward = 1;
				PORTF &= ~((1 << 6) | (1 << 7));
				PORTF |= (1 << 6);
			}
			else if (washing_state == STATE_DRY)
			{
				is_running = 0;
				OCR3C = 0;
				PORTF &= ~((1 << 6) | (1 << 7));
				washing_state = STATE_STANDBY;
			}
		}

		if (is_running == 0) //작동 중이 아니라면
		{
			OCR3C = 0;
			PORTF &= ~((1 << 6) | (1 << 7));
			
			if (washing_state == STATE_STANDBY)      { standby_led(); fnd_washing_display(0); }
			else if (washing_state == STATE_WASH)    { washing_led(); fnd_washing_display(wash_set_min * 60); }
			else if (washing_state == STATE_RINSE)   { rinse_led();   fnd_washing_display(rinse_set_min * 60); }
			else if (washing_state == STATE_DRY)     { dry_led();     fnd_washing_display(dry_set_min * 60); }
		}
		else //작동 중이면
		{
			if (washing_state == STATE_WASH)
			{
				washing_led();
				OCR3C = 140;
			}
			else if (washing_state == STATE_RINSE)
			{
				rinse_led();
				OCR3C = 190;
			}
			else if (washing_state == STATE_DRY)
			{
				dry_led();
				OCR3C = 250;
				
				// 탈수는 정회전으로만
				forward = 1;
				PORTF &= ~((1 << 7));
				PORTF |= (1 << 6);
			}
			
			fnd_washing_display(total_remaining_sec); //화면에 남은 시간 표시
		}
		
	}
	return 0;
}


