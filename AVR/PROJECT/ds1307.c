/*
 * ds1307.c
 *
 * Created: 2026-07-01 오후 4:38:38
 *  Author: kccistc
 */ 
#include "ds1307.h"

void i2c_init(void)
{
	// 1. [교재 변환] 우리의 스타일대로 핀 방향 설정 (PD0, PD1 출력 모드)
	DDRD |= (1 << I2C_SCL);
	DDRD |= (1 << I2C_SDA);
	
	// 2. TWI 통신 속도(SCLK) 설정 레지스터
	// 교재의 TWBR = 32 설정을 그대로 유지합니다.
	TWBR = 32;
	
	// TWI 상태 레지스터의 하위 2비트(분주비)를 0으로 만들어서 1분주로 고정
	TWSR &= ~((1 << TWPS1) | (1 << TWPS0));
}

void i2c_start(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); 
	while (!(TWCR & (1 << TWINT)));
}

void i2c_transmit(uint8_t data)
{
	TWDR=data;
	TWCR= (1 << TWINT) | (1 << TWEN)| (1 << TWEA);
	while(!(TWCR & (1 << TWINT))); //전송 완료 대기
}

uint8_t i2c_receive_ack(void)
{
	TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));  //수신완료 대기
	return TWDR;
}
uint8_t i2c_receive_nack(void)
{
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	return TWDR;
}
void i2c_stop(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

uint8_t dec2bcd(uint8_t dec) 
{ 
	return ((dec / 10) << 4) | (dec % 10); 
}
uint8_t bcd2dec(uint8_t bcd) 
{ 
	return ((bcd >> 4) * 10) + (bcd & 0x0F); 
}

void ds1307_write_time(t_ds1307 *rtc)
{
	i2c_start();
	i2c_transmit(DS1307_ADDR << 1); // 0xD0 전송 (쓰기 모드 상문 오픈)
	i2c_transmit(0x00);             // 내부 메모리 시작 번지를 0번지(초)로 고정
	
	// 구조체 변수를 BCD로 압축해서 줄줄이 쓰기
	i2c_transmit(dec2bcd(rtc->seconds));
	i2c_transmit(dec2bcd(rtc->minutes));
	i2c_transmit(dec2bcd(rtc->hours));
	i2c_transmit(dec2bcd(rtc->dayofweek));
	i2c_transmit(dec2bcd(rtc->date));
	i2c_transmit(dec2bcd(rtc->month));
	i2c_transmit(dec2bcd(rtc->year));
	i2c_stop();
}

void ds1307_read_time(t_ds1307 *rtc)
{
	i2c_start();
	i2c_transmit(DS1307_ADDR << 1); // 0xD0
	i2c_transmit(0x00);
	i2c_stop();
	
	i2c_start();
	i2c_transmit((DS1307_ADDR << 1) | 1); // 0xD1 전송 (읽기 모드 오픈)
	
	// 연속 버스트 읽기 실행 (마지막 연도만 nack 마무리 규칙 준수!)
	rtc->seconds   = bcd2dec(i2c_receive_ack());
	rtc->minutes   = bcd2dec(i2c_receive_ack());
	rtc->hours  = bcd2dec(i2c_receive_ack());
	rtc->dayofweek  = bcd2dec(i2c_receive_ack());
	rtc->date  = bcd2dec(i2c_receive_ack());
	rtc->month = bcd2dec(i2c_receive_ack());
	rtc->year  = bcd2dec(i2c_receive_nack());
	i2c_stop();
}