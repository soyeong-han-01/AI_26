/*
 * pwm.c
 *
 * Created: 2026-06-18 오후 2:23:04
 *  Author: kccistc
 */ 
#include "pwm.h"

extern int get_button(int button_num, int button_pin);

void init_timer1_pwm(void);
void init_motor_driver(void);
void forward(int speed);
void backward(int speed);
void turn_left(int speed);
void turn_right(int speed);
void stop(void);




//16비트 3번 타이머/카운터를 사용
/*
pwm 출력 신호
=============
PB5 : OC1A : 왼쪽 바퀴
PB6 : OC1B : INT4 사용 : 오른쪽 바퀴
BTN0 : 오토모드 or 매뉴얼 모드 설정

방향 설정
========
1. 왼쪽 바퀴
   PORTF0 -- IN1 (DC motor Driver)
   PORTF1 -- IN2
   
2. 오른쪽 바퀴
   PORTF2 -- IN3 (DC motor Driver)
   PORTF3 -- IN4
   
   IN1/IN3  IN2/IN4
   =================
      0       1  : 역회전
	  1       0  : 정회전
	  1       1  : 스탑
*/
#define MOTOR_PWM_DDR DDRB
#define MOTOR_LEFT_PORT_DDR 5 //OC1A
#define MOTOR_RIGHT_PORT_DDR 6 //OC1B

#define MOTOR_DIRECTION_PORT PORTF
#define MOTOR_DIRECTION_PORT_DDR PORTF

void init_timer1_pwm(void)
{
	//---------분주비 설정 ----------
	// 분주비 64분주
	// 16000000/64 --> 250000Hz (250KHz)
	// T=1/f 1/250000Hz --> 0.000004sec --> 4us
	// 250000Hz에서 256개 펄스를 카운트하면 소요 시간: 1.02ms-->0.5s
	// 0x3ff(1023) ---> 4ms
	// TCNT3 : 0~255(0x00ff)까지 카운트한 후 다시 0으로 돌아 간다.
	TCCR1B |= 1 << CS11 | 1 << CS10; //분주비 64
	
	//OCR3C : 50인 경우 duty (high)가 몇%인가?
	//duty cycle = OCR3C / TOP) x 100 = 50 / 255 x 100 = 19.61%
	
	//모드14 : 16비트 고속 pwm 모드 사용 하겠다. timer1(327p 14-5)
	TCCR1A |= 1<<WGM11; //top값을 ICR1에 저장하겠다.
	TCCR1B |= 1<< WGM13 | 1 << WGM12;
	
	//비반전 모드 top : ICR1 비교일치 값(pwm) 지정 OCR1A OCR1B p350 표15-7
	//비교 일치 발생시 OCR1, OCR1B의 출력핀은 LOW로 바뀌고 BOTTOM에서 HIGH로 바뀐다.
	TCCR1A |= 1<<COM1A1;
	TCCR1A |= 1<<COM1B1;
	
	ICR1 = 0x3ff; //1023*4ms ==> 4ms TOP값
	

}
/*
방향 설정
========
1. 왼쪽 바퀴
PORTF0 -- IN1 (DC motor Driver)
PORTF1 -- IN2

2. 오른쪽 바퀴
PORTF2 -- IN3 (DC motor Driver)
PORTF3 -- IN4

IN1/IN3  IN2/IN4
=================
0       1  : 역회전
1       0  : 정회전
1       1  : 스탑
*/

void init_motor_driver(void)
{
	//1. 출력 모드로 설정
	MOTOR_PWM_DDR &= ~(1<<5 | 1<< 6); //0으로 초기화 하고 시작
	MOTOR_PWM_DDR |= 1<<5 | 1<< 6; 
	
	MOTOR_DIRECTION_PORT_DDR &= ~(1<< 0 | 1 << 1| 1<< 2 |1<< 3);
	MOTOR_DIRECTION_PORT_DDR |= 1<< 0 | 1 << 1| 1<< 2 | 1<< 3;
	
	//2. 모터를 전진 모드로
	MOTOR_DIRECTION_PORT &= ~(1<< 0 | 1 << 1| 1<< 2 |1<< 3); //0으로 초기화 하고 시작
	MOTOR_DIRECTION_PORT |= 1<< 2 | 1 << 0; //모터를 전진 모드로 IN4IN3IN2IN1
	                                        //                   0  1  0  1  
	
	DDRF |= 1<<6 | 1<< 7; //출력 모드 설정
	PORTF &= ~(1<<6 | 1<<7); // 6,7 reset
	PORTF |= 1 << 6; //
}
void forward(int speed)
{
	//모터를 전진 모드로
	MOTOR_DIRECTION_PORT &= ~(1<< 0 | 1 << 1| 1<< 2 |1<< 3);
	MOTOR_DIRECTION_PORT |= 1<< 2 | 1 << 0;
	
	OCR1A=OCR1B=speed; //OCR1A: pwm 왼쪽 OCR1B: pwm 오른쪽 바퀴
}

void backward(int speed)
{
	//모터를 후진 모드로
	MOTOR_DIRECTION_PORT &= ~(1<< 0 | 1 << 1| 1<< 2 |1<< 3);
	MOTOR_DIRECTION_PORT |= 1<< 3 | 1 << 1; // IN4IN3IN2IN1 1 0 1 0 
	
	OCR1A=OCR1B=speed; //OCR1A: pwm 왼쪽 OCR1B: pwm 오른쪽 바퀴
}

void turn_left(int speed)
{
	//모터를 좌회전 모드로
	MOTOR_DIRECTION_PORT &= ~(1<< 0 | 1 << 1| 1<< 2 |1<< 3);
	MOTOR_DIRECTION_PORT |= 1<< 2 | 1 << 0;
	
	OCR1A=0;
	OCR1B=speed; //OCR1A: pwm 왼쪽 OCR1B: pwm 오른쪽 바퀴
}

void turn_right(int speed)
{
	//모터를 우회전 모드로
	MOTOR_DIRECTION_PORT &= ~(1<< 0 | 1 << 1| 1<< 2 |1<< 3);
	MOTOR_DIRECTION_PORT |= 1<< 2 | 1 << 0;
	
	OCR1A=speed;
	OCR1B=0; //OCR1A: pwm 왼쪽 OCR1B: pwm 오른쪽 바퀴
}
void stop(void)
{
	MOTOR_DIRECTION_PORT &= ~(1<< 0 | 1 << 1| 1<< 2 |1<< 3);
	MOTOR_DIRECTION_PORT |= 1<< 0 | 1 << 1 | 1 << 2 | 1<< 3; // IN4IN3IN2IN1 1 0 1 0
	
	OCR1A=0;
	OCR1B=0; //OCR1A: pwm 왼쪽 OCR1B: pwm 오른쪽 바퀴
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