#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifndef realloc_try
#define realloc_try 10 //malloc 재시도 횟수(malloc에 실패했을 때 사용)
#endif // !realloc_try
#ifndef try_message //재시도 메지시가 없으면
#define try_falied "Memory problem: fix failed... Now exiting program..."
#endif // !try_message

//공통 함수
extern const char* sgets(int len, int relen, FILE* _Stream); //개행시까지 거의 무제한으로 문장을 입력받는 함수
extern void* calloc_in(const size_t len); //sgets 함수에서 배열 공간 최초 할당할 때 사용
extern void* realloc_in(void* to, const size_t len); //sgets 함수에서 배열 공간 연장할 때 사용
extern void freeset(void** p); //동적할당 해제에 사용