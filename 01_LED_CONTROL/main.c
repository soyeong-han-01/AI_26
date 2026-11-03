/*
 * 01.LED_CONTROL.c
 *
 * Created: 2026-06-10 오전 10:20:27
 * Author : user
 */ 

#define F_CPU 16000000UL   // 16MHz
#include <avr/io.h>  // PORTA PORTD 등의 I/O register들이 들어 있다. 
#include <util/delay.h>   // _delay_ms _delay_us 등의 함수 가 들어 있다. 

#include "button.h"

extern void init_led(void);  // init_led함수는 다른 화일에 들어 있다고 compiler 신고 
extern void init_button(void);
extern int get_button(int button_num, int button_pin);
extern void led_all_on(void);
extern void led_all_off(void);
//extern int led_main(void);
extern void led_shift_left_on(void);
extern void led_shift_right_on(void);
extern void led_shift_left_keep_on(void);
extern void led_shift_right_keep_on(void);
extern void led_flower_on(void);
extern void led_flower_off(void);

int main(void)
{
	//int button0_state=0;
	
		init_button();
		init_led();
		int click=0;
		
		while(1)
	{
		if (get_button(BUTTON3, BUTTON3PIN))
		{
			click++;
			if(click==8) 
			{
				click=0;
			}
		}
			switch(click)
			{
				case 0:
				led_all_off();
				break;
				
				case 1:
				led_all_on();
				break;
				
				case 2:
				led_shift_left_on();
				break;
				
				case 3:
				led_shift_right_on();
				break;
				
				case 4:
				led_shift_left_keep_on();
				break;
				
				case 5:
				led_shift_right_keep_on();
				break;
				
				case 6:
				led_flower_on();
				break;
				
				case 7:
				led_flower_off();
				break;
			
			}
	}
}

#if 0
int main(void)
{
	int button0_state=0;   // 초기 상태를 led all off로 출발 하자
	
	init_button();
	init_led();
	
	//led_main();
	
	while (1)
	{
#if 1		
		// button0_state:0 --> led_shift_left_on
		// button0_state:1 --> led_shift_right_on
		if (get_button(BUTTON0, BUTTON0PIN))
		{
			button0_state = !button0_state;
		}  
	  // 반전  0 <--> 1
			if (button0_state)
			led_shift_left_on();
			else led_shift_right_on();
#endif
		
#if 1	
				// toggle  off <--> on
				if (get_button(BUTTON0, BUTTON0PIN))
				{
					button0_state = !button0_state;   // 반전  0 <--> 1
					if (button0_state)
					led_all_on();
					else led_all_off();
				}
#endif		
	}
}
#endif

#if 0
int main(void)
{
	DDRA = 0b11111111;    // PORTA에 LED가 8개 연결 되어 있으므로 all 1(출력)으로 설정 한다.
	                      // DDR(Data Direction Register) 방향 설정
						  // 1: 출력 0:입력 
    while (1) 
    {
		PORTA = 0b11111111;   // all on
		_delay_ms(1000);   // 1초 유지  1000ms : 1초 500ms : 0.5초
		PORTA = 0b00000000;  // all off
		_delay_ms(1000);    // 1초 sleep
    }
}
#endif 
