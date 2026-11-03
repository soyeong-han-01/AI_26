/*
 * text_lcd.h
 *
 * Created: 2026-07-01 오후 3:54:40
 *  Author: kccistc
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>  // PORTA PORTB PORTD... IO관련 reg가 들어 있다.
#include <util/delay.h>  // _delay_ms _delay_us 등

#define DDR_DATA DDRD // 데이터 핀의 데이터 방향
#define PORT_DATA PORTD //데이터 핀 연결 포트


#define DDR_CONTROL DDRC //제어 핀의 데이터 방향
#define PORT_CONTROL PORTC //제어 핀 연결 포트


#define RS_PIN 0 //레지스터 선택
#define RW_PIN 1 //읽기쓰기
#define E_PIN 2 //활성화

#define COMMAND_CLEAR_DISPLAY 0x01 //화면 지우는 명령어
#define COMMAND_4_BIT_MODE 0x28 //4비트, 2라인, 5x8폰트

//화면,커서, 커서 깜빡임 제어
#define COMMAND_DISPLAY_ON_OFF_BIT 2
#define COMMAND_CURSOR_ON_OFF_BIT 1
#define COMMAND_BLINK_ON_OFF_BIT 0

void lcd_pulse_enable(void);
void lcd_write_data (uint8_t data);
void lcd_write_command(uint8_t command);
void lcd_clear(void);
void lcd_init(void);
void lcd_write_string(char *string);
void lcd_goto_xy(uint8_t row, uint8_t col);