/*
 * pwm.c
 *
 * Created: 2026-06-18 오후 2:23:04
 *  Author: kccistc
 */ 
#define F_CPU 16000000UL
#include <util/delay.h>
#include "button.h"

void init_timer1_pwm(void);
int servo_motor_main(void);


//16비트 3번 타이머/카운터를 사용
/*
pwm 출력 신호
=============
PB5 : OC1A : servo 모터
PB6 : OC1B : not use

*/

void init_timer1_pwm(void)
{
	//모드14 : 16비트 고속 pwm 모드 사용 하겠다. timer1(327p 14-5)
	TCCR1A |= 1<<WGM11;
	TCCR1B |= 1<< WGM13 | 1 << WGM12;
	
	//비반전 모드 top : ICR1 비교일치 값(pwm) 지정 OCR1A p350 표15-7
	//비교 일치 발생시 OCR1 출력핀은 LOW로 바뀌고 BOTTOM에서 HIGH로 바뀐다.
	TCCR1A |= 1<<COM1A1;
		
	//---------분주비 설정 ----------
	// 8분주 하는 이유 16Mhz가지고 50Hz의 주파수를 바로 만들지 못한다.
	// 16000000/8 --> 2MHz
	// T=1/f 1/16000000Hz --> 0.0000000625sec*8-->0.0000005sec(0.5usec)
	// T=1/f 1/2000000Hz ==>0.0000005sec(0.5usec)
	//16비트까지 카운트할 수 있는 최대 0xffff(65535)
	//0.0000005sec * 65535 ==> 0.0327675 (32.7675ms 마다 타이머 인터럽트가 뜬다)
	
	// 20ms 길이(duty)를 갖는 것을 만든다고 한다면
	// 0.0000005sec * 40000개 ==> 0.02sec (20ms)
	TCCR1B &= ~(1 << CS12 | 1<< CS11 | 1 << CS10);
	TCCR1B |= 1 << CS11; //분주비 8
	
	//top값 설정
	ICR1 = 40000-1; // 0.0000005sec * 40000개 ==> 20ms
	

}

int servo_motor_main(void)
{
	//servo motor가 PB5를 출력으로 설정
	DDRB |= 1<<5;
	init_timer1_pwm();
	
	//0(1ms) --> 90(1.5ms) --> 180(2ms) 최대 5000
	while(1)
	{
		//1ms: 2000개 count 
		//40000(20ms) / 20 --> 1ms당 2000개다.
		OCR1A=1600;
		_delay_ms(1000);
		
		//1.5ms: 3000개 count
		OCR1A=3300;
		_delay_ms(1000);
		
		//2ms: 4000개 count
		OCR1A=5000;
		_delay_ms(1000);
	}
	return 0;
}