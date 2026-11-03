/*
 * uart0.c
 *
 * Created: 2026-06-16 오전 9:57:41
 *  Author: kccistc
 */ 
#include "uart0.h"
#include "ds1302.h"

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
void pc_command_processing(t_ds1302 *ds1302); 
void setrtc (t_ds1302 *ds1302);

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

void setrtc (t_ds1302 *ds1302) // 시각보정하는 함수..
{
	extern void init_ds1302(t_ds1302 *ds1302);
	
	ds1302->year= (rx_buff[front][6]-'0')*10+ (rx_buff[front][7] - '0');
	ds1302->month= (rx_buff[front][8] - '0') * 10 + (rx_buff[front][9] - '0');
	ds1302->date= (rx_buff[front][10] - '0') * 10 + (rx_buff[front][11] - '0');
	ds1302->hours= (rx_buff[front][12]- '0')* 10 + (rx_buff[front][13] - '0');
	ds1302->minutes= (rx_buff[front][14]- '0')* 10 + (rx_buff[front][15] - '0');
	ds1302->seconds= (rx_buff[front][16]- '0')* 10 + (rx_buff[front][17] - '0');
	
	init_ds1302(ds1302);
}

void pc_command_processing(t_ds1302 *ds1302)
{
	if (front != rear) // 처리할 문장이 들어왔다면
	{
		// "setrtc" 명령어가 맞는지 확인
		if (strncmp((char *)rx_buff[front], "setrtc", 6) == 0)
		{
			setrtc(ds1302); // 시각 보정 함수 호출
		}
		
		// 명령어 처리가 끝났으니 다음 칸으로 이동
		front = (front + 1) % QUEUE_SIZE;
	}
}
