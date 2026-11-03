#include "keypad.h"

void init_keypad(void);
uint8_t keypad_scan(void);
uint8_t get_button(int row, int col);

void init_keypad(void)
{
	KEYPAD_DDR = 0x0f; //row: 입력 col: 출력
	KEYPAD_PORT = 0xff;
}

uint8_t keypad_scan(void)
{
	uint8_t data=0; //key값 저장 변수
	
	for(int row=0; row<4; row++)
	{
		for(int col=0; col < 4; col++)
		{
			data = get_button(row,col);
			if(data) //if data >=1
			return data;
		}
	}
}

uint8_t get_button(int row, int col)
{
	uint8_t keypad_char[4][4] = {
	{' ','0','=','+'},	//반대됨
	{'7','8','9','*'},
	{'4','5','6','-'},
	{'1','2','3','/'}
	};
	
	//active low로 동작
	static int8_t prev_state[4][4]=
	{
		{1,1,1,1}, //초기 버튼은 눌려지지 않은 상태로 한다.
		{1,1,1,1},
		{1,1,1,1},
		{1,1,1,1}
	};
	
	int8_t current_state=1;
	KEYPAD_PORT=0xff;
	KEYPAD_PORT &= ~(1<<3-col); //COL4가 PA0에 연결되어 있기 때문에/ 해당 col에 전류를 흘린다.
	
	for(int delay=0; delay < 20; delay++) //keypad check를 위한 delay
	;                                     //0.625us*20=1.25us
	 current_state = (KEYPAD_PIN & (1 << (row+4))) >> (row+4);
	 /*예) row: 0 PA4에 연결
	 76543210
	 11101111 KEYPAD_PIN & (1 << (row+4))
	 00001110 >> (row+4) 
	 */
	if (current_state == 0 && prev_state[row][col]==1) //처음 눌려진 상태
	{
		prev_state[row][col]=0;
		return 0; //아직은 완전히 눌러지지 않음
	}
	else if (current_state == 1 && prev_state[row][col]==0)
	{   
		//이전에 버튼이 눌려지고 현재는 버튼을 뗀 상태이면 버튼을 1번 눌렀다 뗀 것으로 인정.
		prev_state[row][col]=1; //prev_state 초기화
		printf("key: %c\n",keypad_char[row][col]);
		return keypad_char[row][col];
	}
	return 0;
}
//ISR 내부의 ++keypad_counter >= 60를 통해서 60ms마다 스캔하므로 _delay가 필요없다???