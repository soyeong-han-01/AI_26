/*
 * ds1302.c
 *
 * Created: 2026-06-26 오후 2:42:23
 *  Author: kccistc
 */ 
#include "ds1302.h"
#include <avr/interrupt.h>

void ds1302_main ( void );
void init_date_time (t_ds1302 *ds1302);
void init_gpio_ds1302 ( void );
void init_ddr_ds1302 ( void );
void init_ds1302 (t_ds1302 *ds1302);
void tx_ds1302(uint8_t data);
void rx_ds1302(uint8_t *pdata);
void clock_ds1302 ( void );
uint8_t dec2bcd (uint8_t data);
uint8_t bcd2dec(uint8_t data);
void write_ds1302 ( uint8_t addr , uint8_t data );
uint8_t read_ds1302(uint8_t addr);


void read_time_ds1302 (t_ds1302 *ds1302);
void read_date_ds1302(t_ds1302 *ds1302);

//숙제 부분
extern void init_uart0(void);
extern void pc_command_processing(t_ds1302 *ds1302);


void ds1302_main ( void )
{
	// 지역변수 선언
	t_ds1302 ds1302; 
	
	init_date_time(&ds1302);
	init_ddr_ds1302();
	init_gpio_ds1302(); // all low로 설정
	init_ds1302(&ds1302); //한번 빌드하고 다시 끄기.
	
	init_uart0();
	sei();
	
	while(1)
	{
		pc_command_processing(&ds1302);
		// 1. read time
		read_time_ds1302(&ds1302);
		// 2. read date
		read_date_ds1302(&ds1302);
		// 3. printf date & time
		printf("DATE: %d-%02d-%02d\n",ds1302.year,ds1302.month,ds1302.date);
		printf("TIME: %02d-%02d-%02d\n",ds1302.hours,ds1302.minutes,ds1302.seconds);
		// 4. delay_ms( 1000 )
		_delay_ms(1000);


		// 4. delay_ms( 1000 )
	}
}

//포인터를 넘겨서 처리할수 있도록
void read_time_ds1302 (t_ds1302 *ds1302)
{
	ds1302->seconds = read_ds1302(ADDR_SECONDS);
	ds1302->minutes = read_ds1302(ADDR_MINUTES);
	ds1302->hours = read_ds1302(ADDR_HOUR);
}
void read_date_ds1302(t_ds1302 *ds1302)
{
	ds1302->date = read_ds1302(ADDR_DATE);
	ds1302->month = read_ds1302(ADDR_MONTH);
	ds1302->dayofweek = read_ds1302(ADDR_DAYOFWEEK);
	ds1302->year = read_ds1302(ADDR_YEAR);
}

uint8_t read_ds1302(uint8_t addr)
{
	uint8_t data8bit = 0; // 1bit씩 읽어서 담을 변수
	//1. CE HIGH
	DS1302_RST_PORT |= 1 << DS1302_RST ;
	// 2. ADDR 전송
	tx_ds1302(addr+1); //read addr
	// 3. data 읽어들임
	rx_ds1302(&data8bit);
	// 4. CE low
	DS1302_RST_PORT &= ~( 1 << DS1302_RST );
	//5. 읽은 값 return ( bcd to dec )
	return bcd2dec(data8bit);
}

void rx_ds1302(uint8_t *pdata)
{
	uint8_t temp = 0;
	
	// 1. input모드로 설정
	DS1302_DAT_DDR &= ~( 1 << DS1302_DAT ); //read mode

	// lsb부터 차례로 입력
	for ( int i = 0; i < 8; i++)
	{
		if (DS1302_DAT_PIN & ( 1 << DS1302_DAT ))
		{
			temp |= 1 << i; //1의 조건만 set
		}
		
		if ( i != 7) //마지막 bit를 읽을 때는 clk을 보내지 않는다.
		clock_ds1302();
	}
	*pdata = temp;
}
void init_ds1302 (t_ds1302 *ds1302)
{
	write_ds1302(ADDR_SECONDS, ds1302->seconds);
	write_ds1302(ADDR_MINUTES, ds1302->minutes);
	write_ds1302(ADDR_HOUR, ds1302->hours);
	write_ds1302(ADDR_DATE, ds1302->date);
	write_ds1302(ADDR_MONTH, ds1302->month);
	write_ds1302(ADDR_DAYOFWEEK, ds1302->dayofweek);
	write_ds1302(ADDR_YEAR, ds1302->year);
}

// dec ---->  bcd
// 예 ) 25
//   dec           bcd
// 00011001      0010 0101
uint8_t dec2bcd (uint8_t data)
{
	uint8_t high, low;
	
	high = (data/10) << 4; //high nibble 에 위치한다.
	low = data % 10;
	
	return(high+low);
}

// 1. 입력 bcd
// 예 )26
// 7654 3210
// 0010 0110
//   2   6
// X10   X1
uint8_t bcd2dec (uint8_t data)
{
	uint8_t high, low;
	
	low = data & 0x0f;
	high = (data  >> 4 ) * 10;
	
	return (high + low);
}

void write_ds1302 ( uint8_t addr , uint8_t data )
{
	// 1. CE low ---> high
	DS1302_RST_PORT |= 1 << DS1302_RST ;
	// 2. ADDRESS 전송
	tx_ds1302(addr);
	// 3. DATA 전송
	tx_ds1302(dec2bcd(data));
	// 4. CE high ---> low
	DS1302_RST_PORT &= ~( 1 << DS1302_RST );
}

void tx_ds1302(uint8_t data)
{
	// 1. 출력모드로 설정
	DS1302_DAT_DDR |= 1 << DS1302_DAT; //write mode
	// 예 )0x80
	//  M       L
	//  1000 0000
	
	for ( int i = 0; i < 8; i++)
	{
		if (data & ( 1 << i))
		DS1302_DAT_PORT |= 1 <<DS1302_DAT; //1
		else DS1302_DAT_PORT &= ~(1 <<DS1302_DAT); //0
		
		clock_ds1302();
	}
}

void clock_ds1302 ( void )
{
	// 1. low -> high -> low
	DS1302_CLK_PORT &= ~( 1 << DS1302_CLK );
	DS1302_CLK_PORT |= 1 << DS1302_CLK;
	DS1302_CLK_PORT &= ~( 1 << DS1302_CLK );
}
void init_ddr_ds1302 ( void )
{
	DS1302_CLK_DDR &= ~ ( 1 << DS1302_CLK | 1 << DS1302_DAT | 1 << DS1302_RST );
	DS1302_CLK_DDR |= 1 << DS1302_CLK | 1 << DS1302_DAT | 1 << DS1302_RST; // 출력 mode로 설정
}
void init_gpio_ds1302 ( void )
{
	DS1302_CLK_PORT &= ~ ( 1 << DS1302_CLK | 1 << DS1302_DAT | 1 << DS1302_RST );
	_delay_ms(2);
}
void init_date_time (t_ds1302 *ds1302)
{
	ds1302->year = 26;
	ds1302->month = 6;
	ds1302->date = 26;
	ds1302->dayofweek = 6; //Friday
	ds1302->hours = 15;
	ds1302->minutes = 19;
	ds1302->seconds = 00;
}