/*
 * ds1307_control.h
 *
 * Created: 2026-07-02 오전 10:20:00
 *  Author: kccistc
 */ 

#include "ds1307.h"

// 시계 관련 상태 모드 정의
#define CLOCK_MODE_NORMAL 0
#define CLOCK_MODE_SET    1

// 세팅 위치 정의
#define SET_YEAR   0
#define SET_MONTH  1
#define SET_DATE   2
#define SET_HOUR   3
#define SET_MIN    4
#define SET_SEC    5

// 외부에서 참조할 현재 시계 세부 모드 변수
extern volatile uint8_t clock_sub_mode;

// 함수 선언
void display_clock_screen(t_ds1307 *rtc);
void handle_clock_setting(t_ds1307 *rtc, uint8_t key_value);
uint8_t calculate_day_of_week(uint16_t y, uint8_t m, uint8_t d);

