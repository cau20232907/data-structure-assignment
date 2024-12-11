#include "20232907 정현승 프로그래밍 과제 3 공통 헤더.h"
#include <ctype.h>
#include <stdarg.h>

int my_scanf(const char* const args, ...);

void main() {
	int a, b;
	float c, d;
	my_scanf("%d%f", &a, &c);
	my_scanf("% f % d", &d, &b);
	printf("%d\n%d\n%f\n%f", a, b, c, d);
}

int my_scanf(const char* const args, ...) {
	static char* input = NULL;
	static char* inputpos = NULL;
	int res = 0;
	const char* pos = args;
	char* end = NULL;
	char* tempstr = NULL;
	void* toput = NULL;
	va_list ap;

	tempstr = strchr(args,'%');
	while (tempstr != NULL) {
		res++;
		tempstr = strchr(tempstr + 1, '%');
	}
	
	va_start(ap, res);
	for (int i = 0; i < res; i++) {
		while (*pos != '%') pos++;
		pos++;
		while (*pos == ' ') pos++; //%와 d/f 사이에 공백은 무시함
		switch (*pos) {
		case 'd':
			if (input == NULL) {
				input = sgets(10, 3, stdin);
				if (input == NULL) {
					return 0;
				}
				inputpos = input;
			}
			while (!isdigit(*inputpos) && *inputpos != '\0' && *inputpos != '+' && *inputpos != '-') inputpos++;
			while (*inputpos == '\0') {
				if (*inputpos == '\0') freeset(&input);
				input = sgets(10, 3, stdin);
				if (input == NULL) {
					return 0;
				}
				inputpos = input;
				while (!isdigit(*inputpos) && *inputpos != '\0' && *inputpos != '+' && *inputpos != '-') inputpos++;
			}
			end = inputpos;
			if (isdigit(*end) || *end == '+' || *end == '-') end++;
			while (isdigit(*end)) end++;
			tempstr = calloc(end - inputpos + 1, sizeof(char));
			if (tempstr == NULL) return 0;
			strncpy(tempstr, inputpos, end - inputpos);

			toput = va_arg(ap, int*);
			*(int*)toput = atoi(tempstr);
			freeset(&tempstr);
			inputpos = end;

			break;
		case 'f':
			if (input == NULL) {
				input = sgets(10, 3, stdin);
				if (input == NULL) {
					return 0;
				}
				inputpos = input;
			}
			while (!isdigit(*inputpos) && *inputpos != '\0' && *inputpos != '+' && *inputpos != '-') inputpos++;
			while (*inputpos == '\0') {
				if (*inputpos == '\0') freeset(&input);
				input = sgets(10, 3, stdin);
				if (input == NULL) {
					return 0;
				}
				inputpos = input;
				while (!isdigit(*inputpos) && *inputpos != '\0' && *inputpos != '+' && *inputpos != '-') inputpos++;
			}
			end = inputpos;
			if (isdigit(*end) || *end == '+' || *end == '-') end++;
			while (isdigit(*end)) end++;
			if (*end == '.') end++; //점 확인
			else if (*end == ',') { //서양에서는 소수점을 ','로 찍기에, 이에 따른 보정
				*end = '.';
				end++;
			}
			while (isdigit(*end)) end++;
			tempstr = calloc(end - inputpos + 1, sizeof(char));
			if (tempstr == NULL) return 0;
			strncpy(tempstr, inputpos, end - inputpos);

			toput = va_arg(ap, float*);
			*(float*)toput = atof(tempstr);
			freeset(&tempstr);
			inputpos = end;

			break;
		}
	}

	va_end(ap);
	return res;
}