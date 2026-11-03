/*
 * fnd.c
 *
 * Created: 2026-06-18 오후 7:00:38
 *  Author: kccistc
 */ 

#include "fnd.h"

void init_fnd(void)
{
	FND_DATA_DDR = 0xff; //데이터 포트 전체 출력 설정
	FND_DIGIT_DDR |= (1 << FND_DIGIT_D1) | (1 << FND_DIGIT_D2) | (1 << FND_DIGIT_D3) | (1 << FND_DIGIT_D4);
	FND_DATA_PORT = ~0xff;
}

void fnd_washing_display(int total_sec)
{
	uint8_t fnd_font[] = { ~0xc0, ~0xf9, ~0xa4, ~0xb0, ~0x99, ~0x92, ~0x82, ~0xd8, ~0x80, ~0x98 };
	static int digit_select = 0;

	int display_val = 0;
	
    //1분 미만으로 남았을 때
	if (total_sec < 60)
	{
		switch(digit_select)
		{
			case 0: /* 초 일의 자리 */
			FND_DIGIT_PORT = ~0x80;
			FND_DATA_PORT = fnd_font[total_sec % 10];
			break;
			case 1: /* 초 십의 자리 */
			FND_DIGIT_PORT = ~0x40;
			FND_DATA_PORT = fnd_font[(total_sec / 10) % 6];
			break;
			case 2: // 1분 미만이므로 분 일의 자리는 끈 상태 유지
			FND_DIGIT_PORT = 0xff;
			break;
			case 3: // 1분 미만이므로 분 십의 자리는 끈 상태 유지
			FND_DIGIT_PORT = 0xff;
			break;
		}
	}
	//60초 이상 남았을 때는 정상적으로 화면에 출력
	else
	{
		int min = total_sec / 60;
		int sec = total_sec % 60;
		
		switch(digit_select)
		{
			case 0: //초 일의 자리
			FND_DIGIT_PORT = ~0x80;
			FND_DATA_PORT = fnd_font[sec % 10];
			break;
			case 1: //초 십의 자리
			FND_DIGIT_PORT = ~0x40;
			FND_DATA_PORT = fnd_font[(sec / 10) % 6];
			break;
			case 2: //분 일의 자리
			FND_DIGIT_PORT = ~0x20;
			FND_DATA_PORT = fnd_font[min % 10];
			break;
			case 3: //분 십의 자리
			FND_DIGIT_PORT = ~0x10;
			FND_DATA_PORT = fnd_font[(min / 10) % 6];
			break;
		}
	}

	digit_select = (digit_select + 1) % 4; //다음 자릿수 선택
}
