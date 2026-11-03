/*
 * pwm.c
 *
 * Created: 2026-06-18 오후 2:23:04
 *  Author: kccistc
 */ 
#include "pwm.h"

extern int get_button(int button_num, int button_pin);

void init_timer3_pwm(void);
void init_motor_driver(void);
void dcmotor_pwm_control_main(void);

//16비트 3번 타이머/카운터를 사용
/*
pwm 출력 신호
=============
PE3 : OC3A
PE4 : OC3B : INT4 사용 (초음파 센서)
PE5 : OC3C ==> DC MOTOR (pwm 출력)
BTN1 : speed-up
BTN2 : speed-down
*/
void init_timer3_pwm(void)
{
	//pE4 초음파 연결
	DDRE |= 1 << 3| 1<<5;
	
	//모드5 : 8비트 고속 pwm 모드 사용 하겠다. timer3(348p 15-6 mode5)
	TCCR3A |= 1<<WGM30;
	TCCR3B |= 1<<WGM32;
	
	//비반전 모드 top : 0x00ff 비교일치 값(pwm) 지정 OCR3C p350 표15-7
	TCCR3A |= 1<<COM3C1;
	
	//---------분주비 설정 ----------
	// 분주비 64분주
	// 16000000/64 --> 250000Hz (250KHz)
	// T=1/f 1/250000Hz --> 0.000004sec --> 4us
	// 250000Hz에서 256개 펄스를 카운트하면 소요 시간: 1.02ms-->0.5s
	// TCNT3 : 0~255(0x00ff)까지 카운트한 후 다시 0으로 돌아 간다.
	TCCR3B |= 1 << CS31 | 1 << CS30; //분주비 64
	OCR3C = 0; //OCR (OUTput Compare Register): pwm값
	
	//OCR3C : 50인 경우 duty (high)가 몇%인가?
	//duty cycle = OCR3C / TOP) x 100 = 50 / 255 x 100 = 19.61%
}
/*
PE5 : pwm control
PE6: IN1 방향 설정
PE7: IN2
*/
void init_motor_driver(void)
{
	DDRF |= 1<<6 | 1<< 7; //출력 모드 설정
	PORTF &= ~(1<<6 | 1<<7); // 6,7 reset
	PORTF |= 1 << 6; //
}

void dcmotor_pwm_control_main(void)
{
	int start_button=0;
	int forward=0;
	
	while(1)
	{
		if(get_button(BUTTON0,BUTTON0PIN)) //start stop
		{
			start_button = !start_button;
			if(start_button)
			{
				OCR3C = 250; //선풍기 세게
			}
			else
			{
				OCR3C = 0;
			}
		}
		else if(get_button(BUTTON1,BUTTON1PIN)) //speed up
		{
			if(OCR3C >= 250)
			OCR3C = 250;
			else OCR3C += 20;
		}
		else if(get_button(BUTTON2,BUTTON2PIN)) //speed down
		{
			if(OCR3C <= 70)
			OCR3C = 70;
			else OCR3C -= 20;
		}
		else if(get_button(BUTTON3,BUTTON3PIN)) // 방향 설정
		{
			forward =! forward;
			PORTF &= ~( 1 << 6 | 1 << 7); //reset
			if (forward)
			PORTF |= 1 << 6; //정회전
			else PORTF |= 1 << 7; //역회전
		}
	}
}