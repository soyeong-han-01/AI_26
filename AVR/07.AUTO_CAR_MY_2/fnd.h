/*
 * fnd.h
 *
 * Created: 2026-06-23 오전 10:19:43
 *  Author: kccistc
 */ 

#ifndef FND_H_
#define FND_H_

#include <avr/io.h>
#include <stdint.h>

/* =========================
   FND 데이터(세그먼트) 포트
   ========================= */
#define FND_DATA_DDR  DDRD
#define FND_DATA_PORT PORTD

/* =========================
   FND1 digit 선택 포트 (4자리, PA4~PA7)
   ========================= */
#define FND1_DIGIT_DDR  DDRA
#define FND1_DIGIT_PORT PORTA

/* =========================
   FND2 digit 선택 포트 (4자리, PB0~PB3)
   ========================= */
#define FND2_DIGIT_DDR  DDRB
#define FND2_DIGIT_PORT PORTB

/* =========================
   차량 상태 (FND 표시용)
   ========================= */
#define STOP_STATE     0
#define FORWARD_STATE  1
#define BACKWARD_STATE 2
#define LEFT_STATE     3
#define RIGHT_STATE    4

/* =========================
   FND 표시 모드
   - DISPLAY_DRIVE  : 주행 중 (방향 + 애니메이션)
   - DISPLAY_TIME   : 주행 시간 조회
   - DISPLAY_BACK   : 후진 횟수 조회
   - DISPLAY_LEFT   : 좌회전 횟수 조회
   - DISPLAY_RIGHT  : 우회전 횟수 조회
   ========================= */
typedef enum
{
	DISPLAY_DRIVE = 0,
	DISPLAY_TIME,
	DISPLAY_BACK,
	DISPLAY_LEFT,
	DISPLAY_RIGHT,
	DISPLAY_MODE_COUNT   /* 항목 개수 (순환용) */
} fnd_display_mode_t;

extern volatile uint8_t car_state;
extern volatile uint16_t drive_time;

extern volatile uint16_t back_count;
extern volatile uint16_t left_count;
extern volatile uint16_t right_count;

extern volatile fnd_display_mode_t fnd_display_mode;

void init_fnd(void);
void fnd_scan(void);

void fnd_set_state(uint8_t state);
void fnd_set_drive_time(uint16_t time);

void fnd_add_back(void);
void fnd_add_left(void);
void fnd_add_right(void);

void fnd_reset_counters(void);
void fnd_next_display_mode(void);

#endif