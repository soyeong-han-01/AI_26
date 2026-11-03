
#include "fnd.h"
#include "fnd_button.h"

int fnd_main(void);
void init_fnd(void);
void fnd_display(void);
void fnd_sec(void);
void stop_watch(void);

uint32_t ms_count=0; //ms를 재는 카운트 uint32_t: 부호 없는 32비트 정수(정수형 데이터 타입)
uint32_t sec_count=0; //sec를 재는 카운트 uint32_t: unsigned int 같은 뜻.
uint8_t dot_display=0;
uint32_t sw_ms_count=0; //스탑워치용 ms 카운트
uint32_t sw_sec_count=0;  //스탑워치용 sec카운트
/*
 * fnd.c
 *
 * Created: 2026-06-12 오전 10:52:49
 *  Author: kccistc
 */ 

int fnd_main(void)
{
	init_fnd();
	init_button();
	
	static int state = 0; // 0: 분·초 시계, 1: 초 시계, 2: 스톱워치 대기, 3: 스톱워치 동작
	
	while(1)
	{
		//상태별 디스플레이 호출
		if(state == 0)
		{
			fnd_display(); // 분·초 시계 모드
		}
		else if(state == 1)
		{
			fnd_sec();     // 초 시계 모드
		}
		else
		{
			stop_watch();  // 스톱워치 (state 2, 3일 때 동작)
		}
		
		//메인 시계 타이머 카운트 (1ms 주기)
		_delay_ms(1);
		ms_count++;
		if(ms_count >= 1000) //1초가 흐르면
		{
			ms_count = 0;
			sec_count++;
			dot_display = !dot_display; // 1초마다 반전
		}
		
		// 스톱워치 타이머 카운트 (state 3=run 일 때만 흐름)
		if(state == 3)
		{
			sw_ms_count++;
			if(sw_ms_count >= 1000)
			{
				sw_ms_count = 0;
				sw_sec_count++;
			}
		}
		
		// 버튼 0을 눌렀을 때
		if(get_button(BUTTON0, BUTTON0PIN))
		{
			if(state == 0) //분,초 시계 모드라면
			{
				state = 1;       // 초 시계 모드로 변경
				ms_count = 0;    // 초 시계 처음부터 시작하도록 리셋
				sec_count = 0;
			}
			else if(state == 1) //초 시계 모드라면
			{
				state = 2;       // 스톱워치 대기 상태로 변경
			}
			else if(state == 2 || state == 3) //스톱워치 대기 or 스탑워치 run상태라면
			{
				state = 0;       // 분·초 시계 모드로 복귀
			}
		}
		
		//버튼 1을 눌렀을 때
		if(get_button(BUTTON1, BUTTON1PIN))
		{
			if(state == 2)      state = 3; // 스탑워치 대기 상태라면 run모드로
			else if(state == 3) state = 2; // 스탑워치 run모드라면 stop상태로
		}
		
		//버튼 2를 눌렀을 때
		if(get_button(BUTTON2, BUTTON2PIN))
		{
			if(state == 2 && sw_sec_count != 0) //스탑워치 stop모드 & 시간이 흐른 상태라면
			{
				sw_ms_count = 0;  // 기록 리셋 
				sw_sec_count = 0;
			}
			else if(state == 2 && sw_sec_count == 0) //스탑워치 stop모드 & 시간을 재지 않은 상태라면
			{
				state = 3;        //run
			}
		}
	}
	
	return 0;
}

void init_fnd(void)
{
	FND_DATA_DDR=0xff; //출력 모드로 설정한다.
	FND_DIGIT_DDR |= 1<<FND_DIGIT_D1 | 1<<FND_DIGIT_D2|1<<FND_DIGIT_D3|1<<FND_DIGIT_D4;

//FND를 전체 off하는 작업
#if 0 //커먼 애노우드
     FND_DATA_PORT = 0xff;
#else //커면 캐소우드
    FND_DATA_PORT = ~0xff;
#endif
}

//분,초 시계 함수
void fnd_display(void)
{                      
	
#if 0                 //  0    1   2    3    4    5    6    7    8    9   .
	uint8_t fnd_font[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xd8,0x80,0x98,0x7f}; //common 애노드
#else
                      //  0    1   2    3    4    5    6    7    8    9   .
    uint8_t fnd_font[]={~0xc0,~0xf9,~0xa4,~0xb0,~0x99,~0x92,~0x82,~0xd8,~0x80,~0x98,~0x7f}; //common 캐소드
#endif

   static int digit_select=0; //자리수 선택
   
   switch(digit_select){
	   
	   case 0: //1단위 초의 일의 자리수
#if 0
            FND_DIGIT_PORT=0x80; //커먼 애노드
#else
            FND_DIGIT_PORT=~0x80; //커먼 캐소드
#endif	   
	        FND_DATA_PORT=fnd_font[sec_count%10]; //0~9
	   break;
	   
	   case 1: //10단위: 초의 십의 자리수
	   #if 0
	        FND_DIGIT_PORT=0x40; //커먼 애노드
	   #else
	        FND_DIGIT_PORT=~0x40; //커먼 캐소드
	   #endif
	        FND_DATA_PORT=fnd_font[sec_count/10%6]; //0~59
	   break;
	   
	   case 2: //100단위 분의 일의 자리수
	   	   #if 0
	   	   FND_DIGIT_PORT=0x20; //커먼 애노드
	   	   #else
	   	   FND_DIGIT_PORT=~0x20; //커먼 캐소드
	   	   #endif
	   	   FND_DATA_PORT=fnd_font[sec_count/60%10];
	   break;
	   
	   case 3: //1000단위 분의 십의 자리수
	   	   #if 0
	   	   FND_DIGIT_PORT=0x10; //커먼 애노드
	   	   #else
	   	   FND_DIGIT_PORT=~0x10; //커먼 캐소드
	   	   #endif
	   	   FND_DATA_PORT=fnd_font[sec_count/600%6]; //0~59
	   break;
	   
   }
   digit_select=(digit_select+1)%4; //표시할 다음 자릿수.
}

//초 시계 함수
void fnd_sec(void) 
{
	// 기존 폰트 배열 (0~9)
	uint8_t fnd_font[]={~0xc0,~0xf9,~0xa4,~0xb0,~0x99,~0x92,~0x82,~0xd8,~0x80,~0x98,~0x7f};
	
	// 써클 패턴 배열
	uint8_t circle1[] = { 0x08, 0x04, 0x02, 0x01 }; //d->c->b->a 오른쪽 구역
	uint8_t circle2[] = { 0x01, 0x20, 0x10, 0x08 }; //a->f->e->d 왼쪽 구역
	
	static int digit_select = 0;
	static int circle_index = 0;
	uint8_t display_data; // 점과 숫자를 같이 나타내기 위해 만든 변수
	
	circle_index = (sec_count) % 8;

	switch(digit_select)
	{
		case 0: // 초 일의 자리
		FND_DIGIT_PORT = ~0x80;
		FND_DATA_PORT = fnd_font[sec_count % 10];
		break;
		
		case 1: // 초 십의 자리
		FND_DIGIT_PORT = ~0x40;
		
		display_data = fnd_font[(sec_count / 10) % 6];
		
		if(dot_display)
		{
			//점 표시 추가
			display_data |= ~0x7f;
		}
		FND_DATA_PORT = display_data;
		break;
		
		case 2: // 분 십의 자리 (서클 패턴-왼쪽)
		if (circle_index < 4)
		{
			FND_DIGIT_PORT = ~0x10;
			FND_DATA_PORT = circle2[circle_index];
		}
		break;
		
		case 3: // 분 일의 자리 (서클 패턴-오른쪽)
		if (circle_index >= 4)
		{
			FND_DIGIT_PORT = ~0x20;
			FND_DATA_PORT = circle1[circle_index - 4];
		}
		break;
	}
	
	digit_select = (digit_select + 1) % 4;
}

void stop_watch(void) //스탑워치 화면
{
	uint8_t fnd_font[]={~0xc0,~0xf9,~0xa4,~0xb0,~0x99,~0x92,~0x82,~0xd8,~0x80,~0x98,~0x7f}; //common 캐소드
	static int digit_select=0; //자리수 선택
	
	   switch(digit_select){
		   
		   case 0: //1단위 초의 일의 자리수
		   FND_DIGIT_PORT=~0x80; //커먼 캐소드
		   FND_DATA_PORT=fnd_font[sw_sec_count%10]; //0~9
		   break;
		   
		   case 1: //10단위: 초의 십의 자리수
		   FND_DIGIT_PORT=~0x40; //커먼 캐소드
		   FND_DATA_PORT=fnd_font[sw_sec_count/10%6]; //0~59
		   break;
		   
		   case 2: //100단위 분의 일의 자리수
		   FND_DIGIT_PORT=~0x20; //커먼 캐소드
		   FND_DATA_PORT=fnd_font[sw_sec_count/60%10];
		   break;
		   
		   case 3: //1000단위 분의 십의 자리수
		   FND_DIGIT_PORT=~0x10; //커먼 캐소드
		   FND_DATA_PORT=fnd_font[sw_sec_count/600%6]; //0~59
		   break;
		   
	   }
	   digit_select=(digit_select+1)%4; //표시할 다음 자릿수.
}