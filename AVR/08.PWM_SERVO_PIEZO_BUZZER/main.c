/*
 * 08.PWM_SERVO_PIEZO_BUZZER.c
 *
 * Created: 2026-06-25 오전 9:46:21
 * Author : kccistc
 */ 

#if 1 //piezo buzzer
#define F_CPU	16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "button.h"

extern void Music_Player(int *tone, int *Beats);
extern void init_speaker(void);
extern void Beep(int  repeat);
extern void Siren(int repeat);
extern void RRR(void);
extern void power_on_melody(int repeat);
extern void open_buzzer(int repeat);

extern const int Elise_Tune[];
extern const int Elise_Beats[];

extern int School_Bell_Tune[];
extern const int School_Bell_Beats[];

extern int GrandFa_Tune[];
extern const int GrandFa_Beats[];

extern int durm_waching_stop_melody_Tune[];
extern const int durm_waching_stop_melody_Beats[];

// PE3 (OC3A) PWM 출력 사용.
// 16bit Timer/Counter
// OCR3A값이 같아지면 Low 출력.
#if 1
int main(void)
{
	init_speaker();
	init_button();
	while(1)
	{
		if(get_button(BUTTON0,BUTTON0PIN))
		{
			power_on_melody(1);
		}
		if(get_button(BUTTON1,BUTTON1PIN))
		{
			open_buzzer(1);
		}
		if(get_button(BUTTON3,BUTTON3PIN))
		{
			Music_Player(durm_waching_stop_melody_Tune, durm_waching_stop_melody_Beats);
		}
	}	
}
#endif
#if 0
int main(void)
{
	init_speaker();
	while(1)
	{
/*
		OCR3A=1702;   // 삐~뽀 레 
		_delay_ms(1000);
		OCR3A=1431;   // 파  
		_delay_ms(1000);
*/

		//power_on_melody(3);
//		 RRR();
 //		_delay_ms(3000);
	//Siren(5);
		//_delay_ms(3000);

        Music_Player(Elise_Tune, Elise_Beats);
	}
}
#endif
#endif
#if 0 //서브모터
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/io.h>
extern int servo_motor_main(void);


int main(void)
{
    servo_motor_main();
    while (1) 
    {
		
    }
}
#endif

