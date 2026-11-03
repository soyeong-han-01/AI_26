/*
 * ds1307.h
 *
 * Created: 2026-07-01 오후 4:39:18
 *  Author: kccistc
 */ 

#ifndef DS1307_H_    // 🌟 중복 방지 가드 시작
#define DS1307_H_

#define F_CPU 16000000UL  // 16MHz
#include <avr/io.h>  
#include <util/delay.h>  
#include <stdio.h>
#include <string.h>

#define I2C_SCL 0  // PD0
#define I2C_SDA 1  // PD1
#define DS1307_ADDR 0x68 // DS1307 고유 7비트 주소

// 구조체 정의 (중복 방지 가드 덕분에 이제 에러가 나지 않습니다)
typedef struct _ds1307
{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t dayofweek; // (1:일, 2:월, 3:화, 4:수, 5:목, 6:금, 7:토)
	uint8_t date;
	uint8_t month;
	uint8_t year;
} t_ds1307;

// 요일 이름을 담은 전역 매핑 배열
static const char* DAY_NAMES[] = {
	"ERR", "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"
};

// 함수 프로토타입 선언
void i2c_init(void);
void i2c_start(void);
void i2c_transmit(uint8_t data);
uint8_t i2c_receive_ack(void);
uint8_t i2c_receive_nack(void);
void i2c_stop(void);
uint8_t dec2bcd(uint8_t dec);
uint8_t bcd2dec(uint8_t bcd);
void ds1307_write_time(t_ds1307 *rtc);
void ds1307_read_time(t_ds1307 *rtc);

#endif /* DS1307_H_ */ // 🌟 중복 방지 가드 끝
