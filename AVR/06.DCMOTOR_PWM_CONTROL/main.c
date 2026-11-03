/*
 * 05.ULTRASONIC.c
 *
 * Created: 2026-06-16 오전 9:51:17
 * Author : kccistc
 */ 

#define F_CPU 16000000UL //이걸 먼저해야함
#include <avr/io.h> //포트,디디알 등등
#include <util/delay.h>
#include <avr/interrupt.h> //sei 등 함수
#include <stdio.h>

extern int led_main(void);
extern void init_uart0(void);
extern void UART0_transmit(uint8_t data);
extern void pc_command_processing(void);
extern void init_ultrasonic(void);
extern void ultrasonic_processing(void);

extern void init_timer3_pwm(void);
extern void init_motor_driver(void);
extern void init_button(void);
extern void dcmotor_pwm_control_main(void);

FILE OUTPUT = FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE);

volatile uint32_t msec_count=0;
volatile int ultrasonic_check_time=0;
/*
ISR(Interrupt Service Routine) : 인터럽트 처리 함수 ISR로 시작
TIMER_OVR_VECT : Timer 0 OVERflow INT가 발생이 되면 이곳으로 진입 함
250개의 펄스를 count하면 이곳으로 자동 진입한다.
ISR은 가능한 짧게 작성 한다.
*/
ISR(TIMER0_OVF_vect)
{
	TCNT0 = 6; //TCNT0 6~256 : 250개 펄스 count하기 위해
	msec_count++; //1ms count
	//ultrasonic_check_time++;
}

int main(void)
{
	init_led();
	init_timer0();
	init_uart0();
	init_button();
	
	init_motor_driver();
	init_timer3_pwm();
	
	
	//init_ultrasonic();
	
	stdout = &OUTPUT; //printf가 동작 할 수 있도록 stdout을 설정.
	sei();
	
	dcmotor_pwm_control_main();
	
	//led_main();
    while (1) 
    {
		//pc_command_processing();
		ultrasonic_processing();
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





