/*
 * cal.c
 *
 * Created: 2026-06-29 오후 4:44:50
 *  Author: kccistc
 */ 
/*
 * cal.c
 */ 
#include "cal.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void calculation(char *expr)
{
	int32_t numbers[40] = {0};
	char operators[40] = {0};
	int num_cnt = 0, op_cnt = 0;
	
	while(*expr != '\0' && *expr != '=')
	{
		//숫자 라면
		if (*expr >= '0' && *expr <= '9')
		{
			numbers[num_cnt++] = strtol(expr, &expr, 10);
		}
		// 연산자라면
		else if(*expr == '+' || *expr == '-' || *expr == '*' || *expr == '/')
		{
			operators[op_cnt++] = *expr;
			expr++;
		}
		else //그 외 다른것이 들어왔을 경우
		{
			expr++;
		}
	}
	
	if(num_cnt == 0) return;
	
	//곱셈 나눗셈 먼저
	for(int i = 0; i < op_cnt; i++)
	{
		if(operators[i] == '*' || operators[i] == '/') 
		{
			if(operators[i] == '*')
			{
				numbers[i] = numbers[i] * numbers[i+1]; 
			}
			else if (operators[i] == '/')
			{
				if(numbers[i+1] == 0)
				{
					printf("Error: 0으로 나눌 수 없습니다.\n");
					return;
				}
				numbers[i] = numbers[i] / numbers[i+1];
			}
			
			// 연산 완료 후 사용된 우측 피연산자와 연산자를 앞으로 한 칸씩 댕김
			for (int j = i + 1; j < num_cnt - 1; j++)   numbers[j] = numbers[j + 1];
			for (int j = i; j < op_cnt - 1; j++)         operators[j] = operators[j + 1];
			
			num_cnt--;
			op_cnt--;
			i--;
		}
	}
	
	//더하기,빼기
	int32_t final_result = numbers[0]; //최종 값
	for (int i = 0; i < op_cnt; i++)
	{
		if(operators[i] == '+')        final_result += numbers[i+1];
		else if (operators[i] == '-')  final_result -= numbers[i+1];
	}
	printf("Result: %d\n", final_result);
}
