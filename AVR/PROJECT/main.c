/*
 * PROJECT.c
 *
 * Created: 2026-07-01 오전 10:47:29
 * Author : kccistc
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>

#include "queue.h"
#include "keypad.h"
#include "button.h"   
#include "text_lcd.h"
#include "cal.h"
#include "ds1307.h"
#include "ds1307_control.h" // 🌟 분할 제어 헤더 장착!

#define MODE_CLOCK 0
#define MODE_CALC  1
#define INACTIVE_TIMEOUT_COUNT  300  // 200ms * 300 = 60,000ms (1분)

char expr_buf[80] = {0,};
int buf_index = 0;

volatile uint32_t keypad_counter = 0;
volatile uint8_t current_mode = MODE_CLOCK; // 대화면 모드 플래그

void init_timer0(void)
{
	TCCR0 |= (1 << CS02) | (1 << CS00); 
	TIMSK |= (1 << TOIE0);              
	TCNT0 = 6;                          
}

ISR(TIMER0_OVF_vect)
{
	uint8_t keydata = 0;
	TCNT0 = 6;
	if (++keypad_counter >= 60)
	{
		keypad_counter = 0;
		
		// 1) 화면 전환 버튼(버튼4 - PC7)은 모드 상관없이 항상 최우선 동작
		if (get_button(BUTTON4, BUTTON4PIN))
		{
			keydata = 'M'; // 모드 토글 신호
		}
		// 2) 계산기 모드(MODE_CALC)일 때 버튼 0~3번의 원래 역할 지정
		else if (current_mode == MODE_CALC)
		{
			if (get_button(BUTTON0, BUTTON0PIN))      keydata = 'C'; // 계산기 리셋
			else if (get_button(BUTTON1, BUTTON1PIN)) keydata = 'B'; // 백스페이스
			else if (get_button(BUTTON2, BUTTON2PIN)) keydata = '('; // 괄호 열기
			else if (get_button(BUTTON3, BUTTON3PIN)) keydata = ')'; // 괄호 닫기
		}
		// 3) 시계 모드(MODE_CLOCK)류 일 때 버튼 0~3번의 세팅 역할 지정
		else
		{
			if (get_button(BUTTON3, BUTTON3PIN))      keydata = 'S'; // 세팅 모드 진입/완료
			else if (get_button(BUTTON0, BUTTON0PIN)) keydata = 'N'; // 세팅 항목 변경(Next)
			else if (get_button(BUTTON1, BUTTON1PIN)) keydata = 'U'; // 값 증가(Up)
			else if (get_button(BUTTON2, BUTTON2PIN)) keydata = 'D'; // 값 감소(Down)
		}
		
		// 4) 독립 버튼이 아무것도 안 눌렸다면 4x4 키패드 스캔 기동
		if(keydata == 0)
		{
			keydata = keypad_scan();
		}
		
		// 5) 최종 포착된 글자가 있다면 원형 큐에 안착
		if(keydata != 0)
		{
			insert_queue(keydata);
		}
	}
}

void refresh_lcd_display(void)
{
	lcd_clear();
	lcd_goto_xy(0, 0);
	
	// 만약 입력된 수식의 길이가 16글자 이하 인 경우 -> 그냥 처음부터 다 보여줌
	if (buf_index <= 16)
	{
		lcd_write_string(expr_buf);
	}
	// 만약 16글자를 넘어간 경우 -> 현재 커서(맨 뒤)를 기준으로 앞의 16글자만 잘라서 보여줌
	else
	{
		// 시작 주소를 맨 뒤에서 16글자 앞으로 당겨서 지정
		char *scroll_start = &expr_buf[buf_index - 16];
		lcd_write_string(scroll_start);
	}
}

int main(void)
{
	uint8_t key_value;
	char res_str[20];
	uint32_t clock_timer = 0;
	uint32_t calc_idle_counter = 0; // 🌟 [추가] 계산기 무입력 시간 측정용 카운터
	
	t_ds1307 my_rtc;
	
	i2c_init();
	init_timer0();
	init_keypad();
	init_button();
	lcd_init();
	
	ds1307_read_time(&my_rtc);
	
	sei();

	while (1)
	{
		// 1. 시계/세팅 모드 화면 갱신 스케줄러 (약 200ms 주기 박자)
		if (current_mode == MODE_CLOCK)
		{
			if (++clock_timer >= 20000)
			{
				clock_timer = 0;
				display_clock_screen(&my_rtc);
			}
		}
		// 🌟 [추가] 계산기 모드일 때 무입력 타이머 카운팅 조율
		else if (current_mode == MODE_CALC)
		{
			if (++clock_timer >= 20000) // 시계와 동일한 약 200ms 주기로 동작
			{
				clock_timer = 0;
				calc_idle_counter++; // 입력이 없으므로 카운트 증가
				
				// 1분(300 카운트) 동안 반응이 없으면 자동 복귀 실행
				if (calc_idle_counter >= INACTIVE_TIMEOUT_COUNT)
				{
					calc_idle_counter = 0;
					clock_sub_mode = CLOCK_MODE_NORMAL; // 시계 일반 화면 지정
					current_mode = MODE_CLOCK;          // 시계 모드로 강제 변경
					ds1307_read_time(&my_rtc);          // RTC 시간 동기화
					lcd_clear();
					display_clock_screen(&my_rtc);
					continue;
				}
			}
		}

		// 2. 입력 이벤트 핸들러
		if(queue_empty() != TRUE)
		{
			key_value = read_queue();
			if(key_value == ' ') continue;
			
			// 🌟 [중요] 버튼이든 키패드든 아무 키나 눌려 큐에서 읽히면 카운터 리셋!
			calc_idle_counter = 0;
			
			// [M] 버튼 4번: 메인 화면 모드 전환
			if(key_value == 'M')
			{
				lcd_clear();
				if(current_mode == MODE_CALC) {
					current_mode = MODE_CLOCK;
					clock_sub_mode = CLOCK_MODE_NORMAL;
					ds1307_read_time(&my_rtc);
					} else {
					current_mode = MODE_CALC;
					if(buf_index == 0) {
						lcd_goto_xy(0, 0);
						lcd_write_string("Calc Ready...");
						} else {
						refresh_lcd_display();
					}
				}
				continue;
			}
			
			// [S] 버튼 3번: 시계 세팅 모드 제어
			if(key_value == 'S' && current_mode == MODE_CLOCK)
			{
				if(clock_sub_mode == CLOCK_MODE_NORMAL) {
					clock_sub_mode = CLOCK_MODE_SET;
					} else {
					my_rtc.dayofweek = calculate_day_of_week(2000 + my_rtc.year, my_rtc.month, my_rtc.date);
					ds1307_write_time(&my_rtc);
					clock_sub_mode = CLOCK_MODE_NORMAL;
				}
				lcd_clear();
				display_clock_screen(&my_rtc);
				continue;
			}
			
			// 3. 서브 모드 분기 처리 규칙
			if (current_mode == MODE_CLOCK && clock_sub_mode == CLOCK_MODE_SET)
			{
				handle_clock_setting(&my_rtc, key_value);
			}
			else if (current_mode == MODE_CALC)
			{
				// 정수형 계산기 코어 로직 (%ld 형식 복구 유지상태)
				if(key_value == 'C')
				{
					memset(expr_buf, 0, sizeof(expr_buf));
					buf_index = 0;
					lcd_clear();
					lcd_goto_xy(0, 0);
					lcd_write_string("Cleared.");
				}
				else if(key_value == 'B')
				{
					if(buf_index > 0)
					{
						expr_buf[--buf_index] = '\0';
						refresh_lcd_display();
					}
				}
				else if(key_value == '=')
				{
					expr_buf[buf_index] = '\0';
					
					if(check_brackets(expr_buf))
					{
						int32_t ans = do_calculation(expr_buf);
						sprintf(res_str, "%ld", ans);          
						
						lcd_goto_xy(1, 0);
						lcd_write_string("Ans:");
						lcd_write_string(res_str);
					}
					else
					{
						lcd_goto_xy(1, 0);
						lcd_write_string("Error: Bracket");
					}
					
					memset(expr_buf, 0, sizeof(expr_buf));
					buf_index = 0;
				}
				else
				{
					if(key_value != 'N' && key_value != 'U' && key_value != 'D' && key_value != 'S' && key_value != 'M') {
						if(buf_index < (int)sizeof(expr_buf) - 1)
						{
							expr_buf[buf_index++] = key_value;
							refresh_lcd_display();
						}
					}
				}
			}
		}
	}
	return 0;
}

