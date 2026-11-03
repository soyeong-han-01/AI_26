/*
 * led.c
 *
 * Created: 2026-06-10 오후 3:10:22
 *  Author: user
 */ 

#include "led.h"
void init_led(void);
void led_all_on(void);
void led_all_off(void);
void led_shift_left_on(void);
void led_shift_right_on(void);
void led_shift_left_keep_on(void);
void led_shift_right_keep_on(void);
void led_flower_on(void);
void led_flower_off(void);

extern volatile uint32_t msec_count; //volatile 최적화 방지
#define FUNC_SU 6

int func_state=0;

void  (*fp[]) ()=
{
	led_shift_left_on, //func_state=0
	led_shift_right_on,
	led_shift_left_keep_on,
	led_shift_right_keep_on,
	led_flower_on,
	led_flower_off, //func_state=5
};

int led_main(void)
{
	uint8_t led_toggle=0;
	
	init_led();
	led_all_off();
	
	while(1)
	{
#if 1		
            fp[func_state] ();
#else            
			if(msec_count >= 500) //500ms if(msec_count == 500) 쓰는 것보다 좋다?
		{
			msec_count=0;
			led_toggle=!led_toggle;
			if(led_toggle)
			led_all_on();
			else led_all_off();
		} 
#endif		
         //led_shift_left_on();
		//led_shift_right_on();
	}
}

void init_led(void)
{
	DDRA=0xff;  // PORTA 를 출력 모드로 설정
	PORTA=0x00;  // PORTA에 물려있는 led를 all off 	
}

void led_all_on(void)
{
	PORTA=0xff;
}

void led_all_off(void)
{
	PORTA=0x00;
}

void led_flower_on(void)
{
	static int i=0;
	
	if(msec_count >=100)
	{
		msec_count =0;
		if(i==0) PORTA=0b00000000;
		PORTA |=(0b00010000)<<i|(0b00001000)>>i;
		i=(i+1)%4;
		if(i==0) func_state=(func_state+1)% FUNC_SU;
	}
	
}

void led_flower_off(void)
{
	static int i=0;
	
	if(msec_count >=100)
	{
		msec_count =0;
		if(i==0) PORTA=0xff;
		PORTA &=~((0b10000000>>i)|(0b00000001<<i));
		i = (i + 1) % 4;
		if(i==0) func_state=(func_state+1)% FUNC_SU;
	}
	
}

void led_shift_left_keep_on(void)
{
	static int i=0;

	if(msec_count >= 100)
	{
		msec_count=0;
		if(i==0) PORTA=0b00000000;
		PORTA |=1<<i;
		
		if( (i=(i+1)%8)==0) //다음 인덱스 값을 계산
		{
			func_state=(func_state+1)%FUNC_SU; //다음 실행할 func으로 jump
		}
	}
	
	
	/*for(int i=0;i<=8;i++)
	{
		PORTA = ~(0xff<<i);
		_delay_ms(100);
	}*/
}

void led_shift_right_keep_on(void)
{
	static int i=0;
		
	if(msec_count >= 100)
	{
		msec_count=0;
		if(i==0) PORTA=0b00000000;	
		PORTA |=0x80>>i;
		
		if( (i=(i+1)%8)==0) //다음 인덱스 값을 계산
		{
			func_state=(func_state+1)%FUNC_SU; //다음 실행할 func으로 jump
		}
	}
	/*for(int i=0;i<=8;i++)
	{
		PORTA =~(0xff>>i);
		_delay_ms(100);
	}*/
}
void led_shift_left_on(void)
{
#if 1
		static int i=0;
		
		if(msec_count >= 100)
		{
			msec_count=0;
			*(unsigned char *) 0x3B = 1 << i;
			//_delay_ms(30);
			if( (i=(i+1)%8) ==0 ) //다음 인덱스 값을 계산
			{
				func_state=(func_state+1)%FUNC_SU; //다음 실행할 func으로 jump
			} 
		}

#endif

}

void led_shift_right_on(void)
{
	static int i=0;
		
		if(msec_count >= 100)
		{
			msec_count=0;
			*(unsigned char *) 0x3B =0x80 >> i;
			if( (i=(i+1)%8)==0) //다음 인덱스 값을 계산
			{
				func_state=(func_state+1)%FUNC_SU; //다음 실행할 func으로 jump
			}
		}

}