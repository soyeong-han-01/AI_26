/*
 * ultrasonic.c
 *
 * Created: 2026-06-17 오후 1:33:03
 *  Author: kccistc
 */ 

#include "ultrasonic.h"

extern volatile int ultrasonic_check_time;

volatile int ultrasonic_distance=0;
volatile char scm[50];

void init_ultrasonic(void);
void make_trigger(void);
void ultrasonic_processing(void);

//p278 표 12-3
//INT4 : PE4 외부 INT4 초음파 센서 상승/하강 엣지 둘다 이곳으로 들어온다.
//결국 상승엣지 때 1번 들어오고 하강엣지 때 1번씩 들어 온다.
//0x000A

ISR(INT4_vect)
{
	// 상승엣지
	if (ECHO_PORT & (1<<ECHO_PIN))
	{
		TCNT1 = 0;
	}
	else //하강엣지
	{
		// TCNT1 : Timer Counter 1
		// 예) TCNT1이 10이 들어있다고 가정,
		// 15.625 KHz의 1주기: 64us
		// 0.000064sec *10개 ==> 0.00064us(640us)
		// 640us / 58(1cm 이동하는데 소요시간) : 11cm
		// 1sec = 1000000us
		
		ultrasonic_distance=TCNT1 * 1000000.0 * 1024 / F_CPU;
		//---- 소요 시간을 cm로 환산
		sprintf(scm, "dis: %dcm\n",ultrasonic_distance/58); //cm로 환산. //58 공기중에서 1cm를 왕복하는데 걸리는 시간
		
	}
}

void init_ultrasonic(void)
{
	TRIG_DDR |= 1<<TRIG_PIN; //output 모드로 설정
	ECHO_DDR &= ~( 1<<ECHO_PIN ); //input 모드로 설정
	
	//p289 표 12-6 p288 그림12-8 참조
	// 0 1: 상승 엣지 하강엣지 둘 다 INT를 띄우도록 요청 한다.
	EICRB |= 0 << ISC41 | 1 << ISC40;
	
	//16비트 타이머/카운터 1번을 사용 하기로 하자
	//타이머 int를 사용하지 않는다
	// 16비트 타이머1 16비트로 표시할 수 있는 최댓값 65535(max): 0xffff
	//16MHz/1024 분주 : 16000000Hz/1024 --> 15625Hz --> 15.625 KHz
	// 1주기(1개의 필수 소요시간) : T=1/f 1/15625 --> 0.000064sec ==> 64us
	// 분주비 1024설정 p318 표14-1
	TCCR1B |= 1<<CS12 | 1<<CS10; //1024 분주
	
	//---- EINT4 설정 -------
	//p287 그림 12-6
	EIMSK |= 1<< INT4; //외부 INT4(ECHO핀) 
}

void make_trigger(void)
{
	TRIG_PORT &= ~(1 << TRIG_PIN); //일단 low로 만든다.
	_delay_ms(1);
	TRIG_PORT |= (1 << TRIG_PIN); //high로 만든다.
	_delay_us(15); //규격에는 10us인데 reduancy로 15us
	TRIG_PORT &= ~(1 << TRIG_PIN); //low로 만든다.
}

void ultrasonic_processing(void)
{
	if (ultrasonic_check_time >= 1000) //1초
	{
		ultrasonic_check_time=0;
		printf("%s", scm);
		make_trigger();
	}
	
}