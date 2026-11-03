/*
 * text_lcd.c
 *
 * Created: 2026-07-01 오후 3:58:03
 *  Author: kccistc
 */ 
#include "text_lcd.h"

//하강 엣지에서 동작
void lcd_pulse_enable(void)
{
	PORT_CONTROL |= 1<<E_PIN; //하이에서
	_delay_ms(1);
	PORT_CONTROL &= ~(1 << E_PIN); //로우로
	_delay_ms(1);
}

//문자 출력 함수
void lcd_write_data (uint8_t data)
{
	PORT_CONTROL |= 1 << RS_PIN; //데이터를 담고있는 레지스터 선택은 1
	PORT_DATA = data & 0xF0;  //상위 4비트
	lcd_pulse_enable();
	PORT_DATA = (data << 4) & 0xF0; //하위 4비트
	lcd_pulse_enable();
	_delay_ms(2);
}

//명령어 실행
void lcd_write_command(uint8_t command)
{
	PORT_CONTROL &= ~(1 << RS_PIN); //명령을 담고있는 레지스터 선택은 0
	PORT_DATA = command & 0xF0;  //상위 4비트
	lcd_pulse_enable();
	PORT_DATA = (command << 4) & 0xF0; //하위 4비트 & 1111 0000
	lcd_pulse_enable();
	
	_delay_ms(2);
}

//lcd 클리어
void lcd_clear(void)
{
	lcd_write_command(COMMAND_CLEAR_DISPLAY);
	_delay_ms(2);
}

void lcd_init(void)
{
	_delay_ms(50);
	
	//연결 핀을 출력으로 설정
	DDR_DATA |= 0xF0;
	PORT_DATA = 0x00;
	DDR_CONTROL |= 1 << RS_PIN | 1 << RW_PIN | 1 << E_PIN;
	
	//RW핀으로 low를 출력하여 쓰기 전용으로 사용
	PORT_CONTROL &= ~(1 << RW_PIN);
	
	lcd_write_command(0x02); //4비트 모드 추가 명령
	lcd_write_command(COMMAND_4_BIT_MODE); //4비트 모드
	
	//display on/off control
	//화면 on, 커서 off, 커서 깜빡임 off
	uint8_t command = 0x08 | (1 << COMMAND_DISPLAY_ON_OFF_BIT);
	lcd_write_command(command);
	
	lcd_clear(); //화면 지움
	
	//ENTRY MODE SET
	//출력 후 커서를 오른쪽으로 옮김, 즉 DDRAM의 주소가 증가하며 화면 이동은 없음.
	lcd_write_command(0x06);
	
	
}

void lcd_write_string(char *string)
{
	uint8_t i;
	for (i=0; string[i]; i++)
	lcd_write_data(string[i]);
}

void lcd_goto_xy(uint8_t row, uint8_t col)
{
	col %= 16; //[0 15]
	row %= 2; //[0 1]
	
	//첫째 라인 시작 주소는 0x00, 둘째 라인 시작 주소는 0x40
	uint8_t address = (0x40 * row) + col;
	uint8_t command = 0x80 + address;
	
	lcd_write_command(command); //커서 이동
}