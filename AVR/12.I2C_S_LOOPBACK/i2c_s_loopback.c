/*
 * i2c_s_loopback.c
 *
 * Created: 2026-06-30 오전 10:52:53
 *  Author: kccistc
 */ 
/*
"ABC" 문자열을 slave로 전송하고  slave가 돌려보낸 문자열을 수신하여 UART로 출력하는
loop_back 테스트 프로그램을 작성 한다.
atmega128a 2개를 연결 한다. 1개는 master 1개는 slave로 동작 하도록 한다.

I2C address: 0x60 

H/W 조건
============
1. I2C 연결
PD0(SCL) -- 10K pull-up 저항 -- VCC
PD1(SDA) -- 10K pull-up 저항 -- VCC
두 atmega의 GND 라인은 공통으로 연결 한다. VCC 끼리 연결 안해도 됨.

2. 속도
F_CPU: 16MHz I2C 속도: 100KHz UART: 9600bps

3. Handshaking
ACK / NACK 의 의미
신호   SDA(데이터 라인) slave 입장에서의 의미
===========================================
ACK   LoW             다음 바이트도 계속 보낸다. 정상적으로 수신 의미도 있다.
NACK  High            이제 그만 보내라 전송 끝이다.
*/
#define F_CPU 16000000UL //이걸 먼저해야함
#include <avr/io.h> //포트,디디알 등등
#include <util/delay.h>
#include <avr/interrupt.h> //sei 등 함수
#include <stdio.h>

//I2C 설정
#define SLAVE_ADDR 0x50 //7bit slave 주소

/*
TWS 상태 코드 (SLAVE) tx는 폴링/rx는 인터럽트로 짜준다.
-----------------------------------------------------------
TWSR(TWI Status Register): 7번~3번 bits(5bit를 참조) : I2C 통신의 전송 상태나 오류를 나타 낸다.
아래의 코드를 수신하면 ISR(TWI_vect) 상태로 분기 된다.
*/
#define TWSR_SR_SLA_ACK 0x60 //SLAVE 리시브 슬레이브 어드레스 ack : SLAVE ADDR+W 전송 후 ACK 전송
#define TWSR_SR_DATA_ACK 0x80 //SLAVE Receive: 슬레이브 데이타 수신 ack 전송
#define TWSR_SR_STOP 0xA0 //스탑 감지


#define TWSR_ST_SLA_ACK 0xA8 //SLAVE가 전송: SlaveAddr+R 수신 ACk 전송
#define TWSR_ST_DATA_ACK 0xB8 // 데이터 전송 + ACK 수신
#define TWSR_ST_DATA_NACK 0xC0 // 데이터 전송 + NACK 수신
#define TWSR_ST_LAST_DATA 0xC8 // 마지막 데이터 전송+ ACK 수신

int i2c_slave_main(void); //slave 빼던가 아무렇게...

/*
ATmega128a TWI(I2C) 관련 register
TWCR:TWI Control Register
    --각 비트의 역할--
	TWINT: 인터럽트 플래그 (1 이면 clear (일반 flag와 다르다.)) I2C작업이 끝나면 set
	TWEA: ACK enable (1: 자동으로 ACK 전송, 0: NACK 전송)
	TWSTA: START 신호 발생 (마스터 전용, 슬레이브는 사용 안함)
	TWSTO: STOP 신호 발생 (마스터 전용, 슬레이브는 사용 안함)
	TWEN: TWI(I2C) Enable (I2C 하드웨어를 활성화)
	TWIE: Interrupt Enable (TWI(I2C) interrupt를 활성화)
TWSR: TWI Status Register (상위 5bit == 상태 코드, 하위 2bit: 분주비)
TWBR: TWI Bit Rate Register
TWDR: TWI Data Register
TWAR: TWI Address Register ( Slave 모드에서 사용: 나의 어드레스를 설정)
*/

#define RX_SIZE 8
volatile uint8_t rx_buff[RX_SIZE]; //volatile; 컴파일러 최적화 방지
volatile int rx_len=0; //수신된 바이트 수
volatile int tx_index=0; //송신 인덱스 (버퍼에서 뭐~)

/////////////////////////////
//TWI interrupt 서비스 루틴
/////////////////////////////
ISR(TWI_vect)
{
		uint8_t st=TWSR & 0xf8; //7번부터 3번비트 값을 취한다.
	
	switch(st){
		//-----slave 수신 단계-------
		case TWSR_SR_SLA_ACK: //SLAVE 리시브 슬레이브 어드레스 ack : SLAVE ADDR+W 전송 후 ACK 전송
		   rx_len=0;
		   TWCR = 1 << TWINT | 1 << TWEA | 1 << TWEN | 1 << TWIE; //ACK 자동전송
		   break;
		
		case TWSR_SR_DATA_ACK: //SLAVE Receive: 슬레이브 데이타 수신 ack 전송
		   if(rx_len < RX_SIZE){
		      rx_buff[rx_len++]=TWDR;
		   }
		   TWCR = 1 << TWINT | 1 << TWEA | 1 << TWEN | 1 << TWIE; //ACK 자동전송
		break;
		
		case TWSR_SR_STOP: //스탑 감지
		   tx_index=0;  //고친 부분
		   TWCR = 1 << TWINT | 1 << TWEA | 1 << TWEN | 1 << TWIE; //읽기 모드 대기
		break;
		
		
		//-----slave 전송 단계-------
		case TWSR_ST_SLA_ACK: //SLAVE가 전송: SlaveAddr+R 수신 ACk 전송
		   tx_index=0; //첫번째 바이트 수신 준비
		//break;
		case TWSR_ST_DATA_ACK: // 데이터 전송 + ACK 수신
		   if(tx_index < rx_len) {
			   TWDR = rx_buff[tx_index++];
		   }
		   else {
			   TWDR = 0xff; //버퍼 이상 발생 : dummy
		   }
		// 마지막 바이트이면 TWEA=0으로 NACK 준비 (더 이상 보낼게 없다.)
		if (tx_index >= rx_len) {
		TWCR = 1 << TWINT | 1 << TWEN | 1 << TWIE; //nack 준비
		}
		else {TWCR = 1 << TWINT | 1 << TWEA | 1 << TWEN | 1 << TWIE;} //ACK 준비
	   
		   break;
		
		
		case TWSR_ST_DATA_NACK: //nack 받으면 송신 종료
		case TWSR_ST_LAST_DATA: // 마지막 데이터 전송+ ACK @@점검
		   tx_index=0;
           TWCR = 1 << TWINT | 1 << TWEA | 1 << TWEN | 1 << TWIE;
		break;
		default: //에러 복구 TWINT clear, ACK 재활성화 한다.
		  TWCR = 1 << TWINT | 1 << TWEA | 1 << TWEN | 1 << TWIE;
		break;
	}	
}


//TWI(I2C) driver
void init_slave_i2c(void)
{
	DDRD |= 1 << 0 | 1 << 1; //추가
	
	TWAR= (SLAVE_ADDR << 1);
	TWCR = 1 << TWEA | 1 << TWEN | 1 << TWIE; //ack 활성화 int 활성화
	
}


int i2c_slave_main(void)
{
	init_slave_i2c();
	sei();
	
	while(1)
	{

	}
}