#include "fnd.h"


// Common Cathode
uint8_t fnd_num[] =
{
	0x3f,
	0x06,
	0x5b,
	0x4f,
	0x66,
	0x6d,
	0x7d,
	0x07,
	0x7f,
	0x6f
};


// Common Cathode 문자
#define FND_F 0x71
#define FND_b 0x7C
#define FND_L 0x38
#define FND_r 0x50
#define FND_S 0x6D



volatile uint8_t car_state = STOP_STATE;


volatile uint16_t drive_time = 0;

volatile uint16_t back_count = 0;
volatile uint16_t left_count = 0;
volatile uint16_t right_count = 0;
volatile uint8_t end_page=0;


static uint8_t digit = 0;





void init_fnd(void)
{

	FND_DATA_DDR = 0xff;
	FND1_DIGIT_DDR |=
	(1<<4)|(1<<5)|(1<<6)|(1<<7);
	FND2_DIGIT_DDR |=
	(1<<0)|(1<<1)|(1<<2)|(1<<3);
	FND_DATA_PORT = 0;
	FND1_DIGIT_PORT = 0xff;
	FND2_DIGIT_PORT = 0xff;
}

void fnd_set_state(uint8_t state)
{

	if(car_state != state)
	{
		switch(state)
		{

			case BACKWARD_STATE:
			back_count++;
			break;

			case LEFT_STATE:
			left_count++;
			break;

			case RIGHT_STATE:
			right_count++;
			break;
		}
	}
	car_state = state;
}








void fnd_set_drive_time(uint16_t time)
{

	drive_time=time;

}







void fnd_reset_record(void)
{

	drive_time=0;

	back_count=0;

	left_count=0;

	right_count=0;

}









void fnd_scan(void)
{

	uint8_t data;
	// 전체 OFF
	FND1_DIGIT_PORT=0xff;
	FND2_DIGIT_PORT=0xff;

	switch(digit)
	{

		// FND1 상태 표시
		case 0:
		switch(car_state)
		{
			case FORWARD_STATE:
			data=FND_F;
			break;

			case BACKWARD_STATE:
			data=FND_b;
			break;

			case LEFT_STATE:
			data=FND_L;
			break;

			case RIGHT_STATE:
			data=FND_r;
			break;

			default:
			data=FND_S;
			break;
		}
		FND_DATA_PORT=data;
		FND1_DIGIT_PORT=~(1<<4);
		break;

		// FND2 시간 표시

		case 1:
		FND_DATA_PORT =
		fnd_num[(drive_time/1000)%10];
		FND2_DIGIT_PORT=~(1<<0);
		break;

		case 2:
		FND_DATA_PORT =
		fnd_num[(drive_time/100)%10];
		FND2_DIGIT_PORT=~(1<<1);
		break;

		case 3:
		FND_DATA_PORT =
		fnd_num[(drive_time/10)%10];
		FND2_DIGIT_PORT=~(1<<2);
		break;

		case 4:
		FND_DATA_PORT =
		fnd_num[drive_time%10];
		FND2_DIGIT_PORT=~(1<<3);
		break;
	}
	digit++;
	if(digit>=5)
	digit=0;
}













// 종료 화면
void fnd_end_view(void)
{
	FND1_DIGIT_PORT=0xff;
	FND2_DIGIT_PORT=0xff;
	switch(end_page)
	{
		case 0:
		FND_DATA_PORT =
		fnd_num[(drive_time/1000)%10];
		FND2_DIGIT_PORT=~(1<<0);
		break;

		case 1:
		FND_DATA_PORT =
		fnd_num[back_count%10];
		FND2_DIGIT_PORT=~(1<<0);
		break;

		case 2:
		FND_DATA_PORT =
		fnd_num[left_count%10];
		FND2_DIGIT_PORT=~(1<<0);
		break;

		case 3:
		FND_DATA_PORT =
		fnd_num[right_count%10];
		FND2_DIGIT_PORT=~(1<<0);
		break;
	}
}

void fnd_end_change(void)
{

	end_page++;


	if(end_page>=4)

	end_page=0;

}