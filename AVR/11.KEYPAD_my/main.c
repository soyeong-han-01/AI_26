/*
 * 11.KEYPAD_my.c
 *
 * Created: 2026-06-29 오후 4:42:49
 * Author : kccistc
 */ 

#define F_CPU 16000000UL //이걸 먼저해야함
#include <avr/io.h> //포트,디디알 등등
#include <util/delay.h>
#include <avr/interrupt.h> //sei 등 함수
#include <stdio.h>
#include <string.h>

#include "queue.h"
#include "uart0.h"
#include "keypad.h"
#include "cal.h"

extern void init_uart0(void);
extern uint8_t keypad_scan(void);
extern void init_keypad(void);
extern void UART0_transmit(uint8_t data);

FILE OUTPUT = FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE);

volatile uint32_t keypad_counter=0;
/*
ISR(Interrupt Service Routine) : 인터럽트 처리 함수 ISR로 시작
TIMER_OVR_VECT : Timer 0 OVERflow INT가 발생이 되면 이곳으로 진입 함
250개의 펄스를 count하면 이곳으로 자동 진입한다.
ISR은 가능한 짧게 작성 한다.
*/
ISR(TIMER0_OVF_vect)
{
	volatile uint8_t keydata=0;
	
	TCNT0 = 6; //TCNT0 6~256 : 250개 펄스 count하기 위해
	if (++keypad_counter >= 60) //60ms마다 체크
	{
		keypad_counter=0;
		if(keydata=keypad_scan()) //keypad를 check해서  눌려 진것이 있으면
		{
			insert_queue(keydata);  //circular queue에 저장한다.
		}
		
	}
}

int main(void)
{
	uint8_t key_value;
	char expr_buf[80]={0,}; // 한 글자씩 들어오는 키 값을 붙여 저장할 배열
	int buf_index=0;
	
	init_timer0();
	init_uart0();
	init_keypad();

	
	stdout = &OUTPUT; //printf가 동작 할 수 있도록 stdout을 설정.
	sei(); //전역 대문 인터럽트 허용

    while (1) 
    {
		if(queue_empty()!=TRUE) //비어있지 않다면
		{
			 key_value=read_queue();
			 
			 if(key_value == ' ' || key_value == 0)
			 {
				 continue;
			 }
			 
			 if(key_value == '=')
			 {
				  //=를 만났다면 지금까지 모아둔 배열 끝에 공백 붙이기
				  expr_buf[buf_index++]='=';
				  expr_buf[buf_index]= '\0';
				  
				   printf("\n");
				  
				  calculation(expr_buf);
				  memset(expr_buf, 0, sizeof(expr_buf));
				  buf_index=0;
			 }
			 else //숫자나 사칙연산자를 만났다면
			 {
				 if(buf_index < 75)
				 {
					expr_buf[buf_index++]=key_value; 
				 }
			 }
		}
    }
}

/*
1.타이머 0을 초기화 한다.
  AVR에서 8bit timer 0/2 중에서 0번을 초기화한다.
  임데이드 쪽에서 가장 신경을 써야 할 부분이 초기화 하는 것이다.
  초기화가 잘못 되면 이후가 다 꼬인다.
2. 8비트 가지고 1ms를 측정하는 타이머/카운터를 만들고자 한다.
2-1 분주비 설정(64분주)
    16000000hz/64 ==> 250,000hz
2-2 1주기가 잡아 먹는 시간 계산
    T=1/f= 1/250,000 ==> 0.000004 sec (4us) ==> 0.004ms
2-3 8비트 가지고 카운트 하는 시간을 계산(8비트 타이머 OV(OVerflow))
    0.004ms x 256개 ==> 0.001024 sec (1.024ms)
	o.oo4ms x 250개 ==> 0.001sec(1ms)
*/
void init_timer0(void)
{
   TCNT0 = 6; //TCNT 6~256 : 250개 펄스를 카운트하기 위해. (타이머카운터 0번)
   TCCR0 &= ~(1 << CS02 | 1 << CS01 | 1 << CS00); //0분주 기존 분주비 비트(CS02,CS01,CS00을 0으로 청소) 타이머 카운터 컨트롤 레지스터 -> 분주비 설정
   TCCR0 |= 1 << CS02 | 0 << CS01 | 0 << CS00; //64분주
   TIMSK |= 1 << TOIE0; //타이머 인터럽트 마스크 레지스터 (켤지/끌지 결정하는 스위치 보드)
   //sei(); // 전역(대문) 인터럽트 허용
}

