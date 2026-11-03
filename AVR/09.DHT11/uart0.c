/*
 * uart0.c
 *
 * Created: 2026-06-16 오전 9:57:41
 *  Author: kccistc
 */ 
#include "uart0.h"

extern int func_state;
extern void  (*fp[])(void);

/*
1.전송 속도: 9600bps 
2.스타트, 스탑 비트 설정
3. RX(수신) : 인터럽트로 설정
4. 
*/
void init_uart0(void);
void UART0_transmit(uint8_t data);
void pc_command_processing(void);

//p278 표 12-3
//pc로 부터 1바이트가 들어오면 자동적으로 이곳으로 진입 한다.
//예) led all on\n 이면 11번 들어온다
ISR(USART0_RX_vect)
{
	volatile uint8_t data;
	volatile static int i=0;
	
	data = UDR0; //UDR0의 내용이 data에 복사된 후 UDR0의 내용은 빈 상태로...
	
	if(data == '\n' || data == '\r')
	{
		
		if(((rear + 1) % QUEUE_SIZE) == front)
		{
			return; // 큐 full 상태
		}
		rx_buff[rear][i] = '\0'; // 문장의 끝에 NULL 집어넣기
		i = 0;                  //다음 string값을 저장 하기 위하여 i를 0으로 만든다.
		rear = (rear + 1) % QUEUE_SIZE;
	}
	else
	{

		if(((rear + 1) % QUEUE_SIZE) == front)
		{
			return; // 큐 full 상태
		}
		rx_buff[rear][i++] = data;
	}
}

void init_uart0(void)
{
	//1.전송속도: 9600bps
	UBRR0H= 0x00; //Uart Baud Rate 레지스터 0번 high바이트
	UBRR0L = 207; //9600bps 표 9-9
	UCSR0A |= 1 << U2X0; //2배속 설정(sampling 8)
	//UART0를 송신, 수신이 다 가능하고 RX 인터럽트가 가능하도록 설정 한다.
	UCSR0B |= 1 << RXEN0 | 1 << TXEN0 | 1 << RXCIE0;
}

//UART0 로 1바이트를 전송 하는 함수
void UART0_transmit(uint8_t data)
{
	while(!(UCSR0A & 1 << UDRE0)) //데이터가 송신중이면 송신이 끝날 때까지 기다림
	; //No operation
	UDR0 = data; //HW 전송 레지스터에 data를 송신한다.
}

void pc_command_processing(void)
{
	if (front != rear) //data가 rx_buff에 존재하는지 check
	{
		printf("%s", rx_buff[front]); //printf("%s", rx_buff[front][0]) 같은 의미
		if (strncmp((char *)rx_buff[front],"led_shift_left_on", strlen("led_shift_left_on"))==0)
		{
			func_state=0;
		}
		else if (strncmp((char *)rx_buff[front],"led_shift_right_on", strlen("led_shift_right_on"))==0)
		{
			func_state=1;
		}
		else if (strncmp((char *)rx_buff[front],"led_shift_left_keep_on", strlen("led_shift_left_keep_on"))==0)
		{
			func_state=2;
		}
		else if(strncmp((char *)rx_buff[front],"led_shift_right_keep_on",strlen("led_shift_right_keep_on"))==0)
		{
			func_state=3;
		}
		else if(strncmp((char *)rx_buff[front],"led_flower_on",strlen("led_flower_on"))==0)
		{
			func_state=4;
		}
		else if(strncmp((char *)rx_buff[front],"led_flower_off",strlen("led_flower_off"))==0)
		{
			func_state=5;
		}
		front = (front+1)%QUEUE_SIZE;
	}
	fp[func_state]();
}