#include "20232907 ������ ���α׷��� ���� 3 ���� ���.h"
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
		while (*pos == ' ') pos++; //%�� d/f ���̿� ������ ������
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
			if (*end == '.') end++; //�� Ȯ��
			else if (*end == ',') { //���翡���� �Ҽ����� ','�� ��⿡, �̿� ���� ����
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