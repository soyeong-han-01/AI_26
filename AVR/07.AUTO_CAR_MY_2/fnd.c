/*
 * fnd.c
 *
 * Created: 2026-06-23 오전 10:20:42
 *  Author: kccistc
 */ 
#include "fnd.h"


// Common Cathode (숫자 0~9)
uint8_t fnd_num[]=
{~0xc0,~0xf9,~0xa4,~0xb0,~0x99,~0x92,~0x82,~0xf8,~0x80,~0x90};

// 직진 애니메이션 (a->b->c->d->e->f->g 순서로 한 칸씩 회전)
uint8_t forward_anim[] =
{
	0x01, // a
	0x02, // b
	0x04, // c
	0x08, // d
	0x10, // e
	0x20, // f
	0x40  // g
};

/* =========================
   상태/라벨 문자 세그먼트 값
   (Common Cathode, 1=ON 기준)
   소문자 느낌으로 통일: f(직진) b(후진) l(좌회전) r(우회전)
   ========================= */
#define FND_f     0x70  // f  : 직진(Forward)
#define FND_b     0x7C  // b  : 후진(Backward)
#define FND_l     0x30  // l  : 좌회전(Left)
#define FND_r     0x50  // r  : 우회전(Right)
#define FND_S     0x6D  // S  : 정지(Stop) - 숫자 5와 동일한 모양
#define FND_t     0x78  // t  : 주행시간(time) 조회 라벨
#define FND_OFF   0x00  // 모든 세그먼트 OFF


volatile uint8_t car_state = STOP_STATE;

volatile uint16_t drive_time = 0;

volatile uint16_t back_count = 0;
volatile uint16_t left_count = 0;
volatile uint16_t right_count = 0;

/* 현재 FND 표시 모드 (주행중 / 결과조회 항목) */
volatile fnd_display_mode_t fnd_display_mode = DISPLAY_DRIVE;

static uint8_t digit = 0;


void init_fnd(void)
{
	FND_DATA_DDR = 0xff; //출력모드 설정

	FND1_DIGIT_DDR |=
	(1<<4)|(1<<5)|(1<<6)|(1<<7);

	FND2_DIGIT_DDR |=
	(1<<0)|(1<<1)|(1<<2)|(1<<3);

	FND_DATA_PORT = 0x00;

	/* digit 선택 포트는 active-low 이므로 평소엔 모두 1(=OFF) */
	FND1_DIGIT_PORT |= (1<<4)|(1<<5)|(1<<6)|(1<<7);
	FND2_DIGIT_PORT |= (1<<0)|(1<<1)|(1<<2)|(1<<3);
}


/* 차량 상태 변경 + 카운트 (이전 상태와 달라질 때만 카운트) */
void fnd_set_state(uint8_t state)
{
	if(car_state != state)
	{
		if(state == BACKWARD_STATE)
			back_count++;
		else if(state == LEFT_STATE)
			left_count++;
		else if(state == RIGHT_STATE)
			right_count++;
	}

	car_state = state;
}


void fnd_set_drive_time(uint16_t time)
{
	drive_time = time;
}


void fnd_add_back(void)
{
	back_count++;
}


void fnd_add_left(void)
{
	left_count++;
}


void fnd_add_right(void)
{
	right_count++;
}


/* 주행 종료 시 호출: 결과 조회 누적치는 유지하고 싶다면 이 함수는
   호출하지 않고, 새 주행을 0부터 다시 세고 싶을 때만 호출한다. */
void fnd_reset_counters(void)
{
	drive_time = 0;
	back_count = 0;
	left_count = 0;
	right_count = 0;
}


/* 결과 조회 모드를 한 단계 순환시킨다.
   DISPLAY_TIME -> DISPLAY_BACK -> DISPLAY_LEFT -> DISPLAY_RIGHT -> DISPLAY_DRIVE
   (DISPLAY_DRIVE로 돌아오면 다음 버튼 입력 시 다시 자동주행을 시작할 수 있다) */
void fnd_next_display_mode(void)
{
	if(fnd_display_mode == DISPLAY_DRIVE)
	{
		fnd_display_mode = DISPLAY_TIME;
	}
	else if(fnd_display_mode == DISPLAY_RIGHT)
	{
		/* 마지막 조회 항목 다음엔 주행 대기 화면으로 복귀 */
		fnd_display_mode = DISPLAY_DRIVE;
	}
	else
	{
		fnd_display_mode++;
	}
}


/* 주행 모드로 돌아갈 때 호출 (자동/수동 모드 진입 시) */
static void fnd_set_drive_display(void)
{
	fnd_display_mode = DISPLAY_DRIVE;
}


void fnd_scan(void)
{
	uint8_t data = 0;
	uint16_t value = 0;     // 조회 모드에서 표시할 숫자값
	uint8_t label = FND_OFF; // 조회 모드 좌측 라벨 문자

	/* 모두 OFF (digit 선택 포트 active-low -> 1이 OFF) */
	FND1_DIGIT_PORT |= (1<<4)|(1<<5)|(1<<6)|(1<<7);
	FND2_DIGIT_PORT |= (1<<0)|(1<<1)|(1<<2)|(1<<3);

	// 세그먼트 OFF
	FND_DATA_PORT = 0x00;

	/* =========================
	   결과 조회 모드일 때 표시할 값/라벨 결정
	   ========================= */
	switch(fnd_display_mode)
	{
		case DISPLAY_TIME:
			value = drive_time;
			label = FND_t;
			break;

		case DISPLAY_BACK:
			value = back_count;
			label = FND_b;
			break;

		case DISPLAY_LEFT:
			value = left_count;
			label = FND_l;
			break;

		case DISPLAY_RIGHT:
			value = right_count;
			label = FND_r;
			break;

		default:
			value = 0;
			label = FND_OFF;
			break;
	}

	switch(digit)
	{
		/* ---- FND1 : 자리 0 (상태 / 조회 라벨) ---- */
		case 0:

			if(fnd_display_mode == DISPLAY_DRIVE)
			{
				switch(car_state)
				{
					case FORWARD_STATE:  data = FND_f; break;
					case BACKWARD_STATE: data = FND_b; break;
					case LEFT_STATE:     data = FND_l; break;
					case RIGHT_STATE:    data = FND_r; break;
					default:             data = FND_S; break;
				}
			}
			else
			{
				data = label;
			}

			FND_DATA_PORT = data;
			FND1_DIGIT_PORT &= ~(1<<4);
			break;

		/* ---- FND1 : 자리 1~3 (주행중 애니메이션 / 조회모드에선 OFF) ---- */
		case 1:

			if(fnd_display_mode == DISPLAY_DRIVE && car_state != STOP_STATE)
			{
				FND_DATA_PORT = forward_anim[(drive_time) % 7];
			}
			FND1_DIGIT_PORT &= ~(1<<5);
			break;

		case 2:

			if(fnd_display_mode == DISPLAY_DRIVE && car_state != STOP_STATE)
			{
				FND_DATA_PORT = forward_anim[(drive_time + 1) % 7];
			}
			FND1_DIGIT_PORT &= ~(1<<6);
			break;

		case 3:

			if(fnd_display_mode == DISPLAY_DRIVE && car_state != STOP_STATE)
			{
				FND_DATA_PORT = forward_anim[(drive_time + 2) % 7];
			}
			FND1_DIGIT_PORT &= ~(1<<7);
			break;

		/* ---- FND2 : 자리 4~7 (4자리 숫자) ---- */
		case 4: // 천의 자리
			if(fnd_display_mode == DISPLAY_DRIVE)
				FND_DATA_PORT = fnd_num[(drive_time / 1000) % 10];
			else
				FND_DATA_PORT = fnd_num[(value / 1000) % 10];
			FND2_DIGIT_PORT &= ~(1<<0);
			break;

		case 5: // 백의 자리
			if(fnd_display_mode == DISPLAY_DRIVE)
				FND_DATA_PORT = fnd_num[(drive_time / 100) % 10];
			else
				FND_DATA_PORT = fnd_num[(value / 100) % 10];
			FND2_DIGIT_PORT &= ~(1<<1);
			break;

		case 6: // 십의 자리
			if(fnd_display_mode == DISPLAY_DRIVE)
				FND_DATA_PORT = fnd_num[(drive_time / 10) % 10];
			else
				FND_DATA_PORT = fnd_num[(value / 10) % 10];
			FND2_DIGIT_PORT &= ~(1<<2);
			break;

		case 7: // 일의 자리
			if(fnd_display_mode == DISPLAY_DRIVE)
				FND_DATA_PORT = fnd_num[drive_time % 10];
			else
				FND_DATA_PORT = fnd_num[value % 10];
			FND2_DIGIT_PORT &= ~(1<<3);
			break;
	}

	digit++;
	if(digit >= 8)
		digit = 0;
}