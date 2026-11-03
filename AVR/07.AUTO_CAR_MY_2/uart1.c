/*
 * uart1.c
 *
 * Created: 2026-06-22 오전 10:38:35
 *  Author: kccistc
 */ 

//bt로부터 1byte가 들어오면 자동적으로 이곳으로 진입.
#include "uart1.h"

volatile uint8_t bt_data;
void UART1_transmit(uint8_t data);
// extern void UART0_transmit(uint8_t data);

void init_uart1(void);

ISR(USART1_RX_vect)
{
	
	
	bt_data = UDR1; //UDR1의 내용이 data에 복사된 후 UDR1의 내용은 빈 상태로...
	UART1_transmit(bt_data); // bt로 부터 들어온 바이트를 확인하기 위해 컴포트로 출력
	

}

void init_uart1(void)
{
	//1.전송속도: 9600bps
	UBRR1H= 0x00; //Uart Baud Rate 레지스터 0번 high바이트
	UBRR1L = 207; //9600bps 표 9-9
	UCSR1A |= 1 << U2X1; //2배속 설정(sampling 8)
	//UART1를 송신, 수신이 다 가능하고 RX 인터럽트가 가능하도록 설정 한다.
	UCSR1B |= 1 << RXEN1 | 1 << TXEN1 | 1 << RXCIE1;
}

//UART1 로 1바이트를 전송 하는 함수
void UART1_transmit(uint8_t bt_data)
{
	while(!(UCSR1A & 1 << UDRE1)) //데이터가 송신중이면 송신이 끝날 때까지 기다림
	; //No operation
	UDR1 = bt_data; //HW 전송 레지스터에 data를 송신한다.
}