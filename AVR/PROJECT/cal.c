/*
 * cal.c
 *
 * Created: 2026-07-01 오후 7:36:22
 *  Author: kccistc
 */ 

#include "cal.h"
#include <stdio.h>
#include <stdlib.h>

// 연산자 우선순위를 반환하는 함수 (숫자가 클수록 높은 우선순위)
int get_priority(char op)
{
	if (op == '+' || op == '-') return 1;
	if (op == '*' || op == '/') return 2;
	return 0; // 괄호 '(' 등은 가장 낮게 설정
}

// 스택에 쌓인 데이터를 가지고 실제 연산을 수행하는 함수
void compute(int32_t *num_stack, int *num_top, char *op_stack, int *op_top)
{
	if (*num_top < 2 || *op_top < 1) return; // 피연산자 2개, 연산자 1개가 없으면 리턴
	
	int32_t b = num_stack[--(*num_top)]; // 우측 피연산자
	int32_t a = num_stack[--(*num_top)]; // 좌측 피연산자
	char op = op_stack[--(*op_top)];    // 연산자
	
	int32_t res = 0;
	if (op == '+')      res = a + b;
	else if (op == '-') res = a - b;
	else if (op == '*') res = a * b;
	else if (op == '/') {
		if (b == 0) res = 0; // 0 분모 예외 처리
		else        res = a / b;
	}
	
	num_stack[(*num_top)++] = res; // 계산 결과를 다시 숫자 스택에 push
}

// 괄호 누락 체크 (기존 함수 유지)
int check_brackets(char *str)
{
	int bracket_cnt = 0;
	for (int i = 0; str[i] != '\0'; i++)
	{
		if (str[i] == '(') bracket_cnt++;
		else if (str[i] == ')')
		{
			bracket_cnt--;
			if (bracket_cnt < 0) return 0;
		}
	}
	return (bracket_cnt == 0);
}

// 🌟 [새로운 스택 기반 계산 함수] 이 함수 하나로 모든 것을 해결합니다!
int32_t do_calculation(char *expr)
{
	int32_t num_stack[40] = {0};
	char op_stack[40] = {0};
	int num_top = 0;
	int op_top = 0;
	
	char *p = expr;
	
	while (*p != '\0' && *p != '=')
	{
		if (*p == ' ') {
			p++;
			continue;
		}
		
		// 1. 숫자라면 값 읽어서 숫자 스택에 push
		if (*p >= '0' && *p <= '9') {
			num_stack[num_top++] = strtol(p, &p, 10);
		}
		// 2. 열기 괄호 '(' 라면 연산자 스택에 무조건 push
		else if (*p == '(') {
			op_stack[op_top++] = *p;
			p++;
		}
		// 3. 닫기 괄호 ')' 라면 '('를 만날 때까지 스택을 정산(compute)
		else if (*p == ')') {
			while (op_top > 0 && op_stack[op_top - 1] != '(') {
				compute(num_stack, &num_top, op_stack, &op_top);
			}
			if (op_top > 0 && op_stack[op_top - 1] == '(') {
				op_top--; // '(' 자체를 스택에서 제거
			}
			p++;
		}
		// 4. 사칙 연산자라면 우선순위 비교 후 스택 제어
		else if (*p == '+' || *p == '-' || *p == '*' || *p == '/') {
			// 현재 연산자보다 스택 제일 위에 있는 연산자의 우선순위가 높거나 같다면 먼저 계산함!
			while (op_top > 0 && get_priority(op_stack[op_top - 1]) >= get_priority(*p)) {
				compute(num_stack, &num_top, op_stack, &op_top);
			}
			op_stack[op_top++] = *p; // 현재 연산자를 스택에 push
			p++;
		}
		else {
			p++;
		}
	}
	
	// 수식이 끝났으므로 스택에 남은 연산자들을 최종 정산
	while (op_top > 0) {
		compute(num_stack, &num_top, op_stack, &op_top);
	}
	
	// 최종 결과는 숫자 스택의 가장 첫 번째에 남아있음
	if (num_top > 0) return num_stack[0];
	return 0;
}

