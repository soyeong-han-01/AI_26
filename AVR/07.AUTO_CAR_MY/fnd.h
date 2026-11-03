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


#define FND_DATA_DDR  DDRC
#define FND_DATA_PORT PORTC


#define FND1_DIGIT_DDR  DDRA
#define FND1_DIGIT_PORT PORTA


#define FND2_DIGIT_DDR  DDRB
#define FND2_DIGIT_PORT PORTB



#define STOP_STATE     0
#define FORWARD_STATE  1
#define BACKWARD_STATE 2
#define LEFT_STATE     3
#define RIGHT_STATE    4



// 자동차 모드
#define MANUAL_MODE 0
#define AUTO_MODE   1
#define END_MODE    2

void fnd_reset_record(void);
void fnd_end_view(void);


extern volatile uint8_t car_state;

extern volatile uint16_t drive_time;
extern volatile uint16_t back_count;
extern volatile uint16_t left_count;
extern volatile uint16_t right_count;



void init_fnd(void);
void fnd_scan(void);
void fnd_set_state(uint8_t state);
void fnd_set_drive_time(uint16_t time);
void fnd_end_view(void);
void fnd_end_change(void);


#endif