/*
 * 07.AUTO_CAR_MY.c
 *
 * Created: 2026-06-22 오후 2:32:44
 * Author : kccistc
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "button.h"
#include "led.h"
#include "ultrasonic.h"
#include "pwm.h"
#include "def.h"
#include "fnd.h"

void auto_mode_check(void);
void auto_mode(void);

extern volatile int left_distance;
extern volatile int center_distance;
extern volatile int right_distance;
extern volatile int ultrasonic_check_time;
extern void init_uart1(void);
extern volatile uint8_t bt_data;

volatile int auto_change = 0;

void manual_mode(void)
{
	switch(bt_data)
	{
		case 'F':
		case 'f':
		forward(500); //4us*500= 0.002sec(2ms)
		break;
		case 'B':
		case 'b':
		backward(500);
		break;
		case 'L':
		case 'l':
		turn_left(700); //4us*700=0.0028sec
		break;
		case 'R':
		case 'r':
		turn_right(700);
		break;
		case 'S':
		case 's':
		stop();
		break;
		default:
		break;
	}
}

int main(void)
{
    init_led();
    init_button();
    init_ultrasonic();

    init_timer1_pwm();
    init_motor_driver();
	
	init_uart1();
	init_fnd();

    /* 1ms timer0 */
    TCCR0 |= (1 << CS02) | (1 << CS00);
    TIMSK |= (1 << TOIE0);
    TCNT0 = 6;

    sei();

    led_off();

    while (1)
    {
        ultrasonic_processing();

        auto_mode_check();

        if (auto_change)
        {
            auto_mode();
        }
		else
		{
			manual_mode();
		}
    }
}

/* =========================
   버튼 / LED 제어

   동작 정리
   ---------
   1) 대기 상태(auto_change==0, fnd_display_mode==DISPLAY_DRIVE)에서
      BUTTON0을 누르면 -> 자동주행을 "시작" 한다.
      (이전 주행 기록은 초기화되고 FND는 주행 화면 표시)

   2) 주행 중(auto_change==1)에 BUTTON0을 누르면
      -> 주행을 "정지" 하고 결과조회 모드로 진입한다.
         (첫 화면은 주행시간)

   3) 정지 상태에서 결과조회 모드(주행시간/후진/좌회전/우회전)에
      있을 때 BUTTON0을 누르면 다음 항목으로 순환한다.
      주행시간 -> 후진횟수 -> 좌회전횟수 -> 우회전횟수 -> (대기화면)

   4) 우회전횟수 화면에서 한 번 더 누르면 대기화면(DISPLAY_DRIVE)으로
      돌아가고, 그 상태에서 다시 누르면 1)번처럼 새 주행이 시작된다.
   ========================= */
void auto_mode_check(void)
{
    if (get_button(BUTTON0, BUTTON0PIN))
    {
        if (auto_change == 0 && fnd_display_mode == DISPLAY_DRIVE)
        {
            /* 대기 상태 -> 자동주행 시작 */
            auto_change = 1;
            fnd_reset_counters();      /* 새 주행 시작: 카운터 초기화 */
            fnd_display_mode = DISPLAY_DRIVE;
        }
        else if (auto_change == 1)
        {
            /* 주행 중 -> 정지 + 결과조회 모드 진입 */
            auto_change = 0;
            stop();
            fnd_display_mode = DISPLAY_TIME;
        }
        else
        {
            /* 정지 상태 + 이미 조회모드 -> 항목 순환 (마지막 항목 다음엔 대기화면) */
            fnd_next_display_mode();
        }
    }

    if (auto_change)
        led_on();
    else
        led_off();
}

void auto_mode(void)
{
    /* =========================
       1. 기본: 직진 유지
       ========================= */
    if(center_distance > 20 || center_distance == 0)
    {
        forward(600);
        return;
    }

    /* =========================
       2. 전방 장애물 (코너 포함)
       ========================= */
    if(center_distance > 0 &&
       center_distance <= 20)
    {
        stop();
        _delay_ms(40);

        /* 살짝 후진 (코너 탈출 공간 확보) */
        backward(400);
        _delay_ms(120);

        stop();
        _delay_ms(30);

        /* =========================
           3. 방향 선택 (핵심)
           ========================= */
        if(left_distance > right_distance)
        {
	        turn_left(600);
	        _delay_ms(150);
        }
        else
        {
	        turn_right(600);
	        _delay_ms(150);
        }

        stop();
        _delay_ms(30);

        return;
        }
    /* =========================
       4. 좌우 벽 보정 (부드럽게)
       ========================= */
    if(left_distance > 0 &&
       right_distance > 0)
    {
        int diff = left_distance - right_distance;

        if(diff > 12)
        {
            turn_left(350);
            _delay_ms(25);
            return;
        }
        else if(diff < -12)
        {
            turn_right(350);
            _delay_ms(25);
            return;
        }
    }

    /* =========================
       5. fallback
       ========================= */
    forward(600);
}

