/*
 * cal.h
 *
 * Created: 2026-07-01 오후 7:35:12
 *  Author: kccistc
 */ 

#ifndef CAL_H_
#define CAL_H_

#include <stdint.h>

// 괄호 수식 검증 및 연산 본체 함수 원형
int check_brackets(char *str);
int32_t do_calculation(char *expr);

#endif /* CAL_H_ */
