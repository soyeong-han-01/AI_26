/*
 * ds1307_control.c
 *
 * Created: 2026-07-02
 *  Author: kccistc
 */ 

#include "ds1307_control.h"
#include "text_lcd.h"
#include <stdio.h>

volatile uint8_t clock_sub_mode = CLOCK_MODE_NORMAL;
static uint8_t set_target = SET_YEAR;

// 요일 자동 계산 함수
uint8_t calculate_day_of_week(uint16_t y, uint8_t m, uint8_t d) {
	if (m < 3) { m += 12; y--; }
	uint16_t val = (d + (13 * (m + 1)) / 5 + y + y / 4 - y / 100 + y / 400) % 7;
	if (val == 0) return 7; // 토요일
	return val;            // 1:일 ~ 6:금
}

// 화면 출력 및 커서 제어 함수
void display_clock_screen(t_ds1307 *rtc)
{
	char lcd_buff[20];
	
	if (clock_sub_mode == CLOCK_MODE_NORMAL) {
		ds1307_read_time(rtc);
	}
	
	if(rtc->dayofweek < 1 || rtc->dayofweek > 7) rtc->dayofweek = 1;

	// 1번째 줄 출력 (2026-06-27 (SAT))
	sprintf(lcd_buff, "20%02d-%02d-%02d (%s)", rtc->year, rtc->month, rtc->date, DAY_NAMES[rtc->dayofweek]);
	lcd_goto_xy(0, 0);
	lcd_write_string(lcd_buff);
	
	// 2번째 줄 출력 (TIME: ... 또는 SET : ...)
	if (clock_sub_mode == CLOCK_MODE_SET) {
		sprintf(lcd_buff, "SET : %02d:%02d:%02d", rtc->hours, rtc->minutes, rtc->seconds);
	} else {
		sprintf(lcd_buff, "TIME: %02d:%02d:%02d", rtc->hours, rtc->minutes, rtc->seconds);
	}
	lcd_goto_xy(1, 0);
	lcd_write_string(lcd_buff);

	// 세팅 모드일 때 커서 위치 이동 및 깜빡임 켜기
	if (clock_sub_mode == CLOCK_MODE_SET) {
		lcd_write_command(0x0C | (1 << COMMAND_CURSOR_ON_OFF_BIT) | (1 << COMMAND_BLINK_ON_OFF_BIT));
		switch(set_target) {
			case SET_YEAR:  lcd_goto_xy(0, 3);  break;
			case SET_MONTH: lcd_goto_xy(0, 6);  break;
			case SET_DATE:  lcd_goto_xy(0, 9);  break;
			case SET_HOUR:  lcd_goto_xy(1, 7);  break;
			case SET_MIN:   lcd_goto_xy(1, 10); break;
			case SET_SEC:   lcd_goto_xy(1, 13); break;
		}
	} else {
		// 일반 모드에서는 커서 숨김
		lcd_write_command(0x08 | (1 << COMMAND_DISPLAY_ON_OFF_BIT));
	}
}

// 세팅 내부 버튼 이벤트 처리 함수
void handle_clock_setting(t_ds1307 *rtc, uint8_t key_value)
{
	if(key_value == 'N') { // 버튼0: 다음 항목
		set_target = (set_target + 1) % 6;
	}
	else if(key_value == 'U') { // 버튼1: 값 증가
		switch(set_target) {
			case SET_YEAR:  rtc->year = (rtc->year + 1) % 100; break;
			case SET_MONTH: rtc->month = (rtc->month % 12) + 1; break;
			case SET_DATE:  rtc->date = (rtc->date % 31) + 1; break; 
			case SET_HOUR:  rtc->hours = (rtc->hours + 1) % 24; break;
			case SET_MIN:   rtc->minutes = (rtc->minutes + 1) % 60; break;
			case SET_SEC:   rtc->seconds = (rtc->seconds + 1) % 60; break;
		}
	}
	else if(key_value == 'D') { // 버튼2: 값 감소
		switch(set_target) {
			case SET_YEAR:  rtc->year = (rtc->year == 0) ? 99 : rtc->year - 1; break;
			case SET_MONTH: rtc->month = (rtc->month == 1) ? 12 : rtc->month - 1; break;
			case SET_DATE:  rtc->date = (rtc->date == 1) ? 31 : rtc->date - 1; break;
			case SET_HOUR:  rtc->hours = (rtc->hours == 0) ? 23 : rtc->hours - 1; break;
			case SET_MIN:   rtc->minutes = (rtc->minutes == 0) ? 59 : rtc->minutes - 1; break;
			case SET_SEC:   rtc->seconds = (rtc->seconds == 0) ? 59 : rtc->seconds - 1; break;
		}
	}
	
	// 변경값 즉시 반영 출력
	display_clock_screen(rtc);
}
