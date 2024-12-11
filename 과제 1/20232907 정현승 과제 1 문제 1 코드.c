#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

#define STRMAXLEN 5 //Max string length including '\0'
#define try_falied "Memory problem: fix failed... Now exiting program..."

void main() {
	int size[3] = { 0, }; //a, b, c
	void*** ptr = calloc((sizeof(size) / sizeof(size[0])), sizeof(void**));

	//Used to get number in array
	int intin = 1;
	double doublein = 1.0;
	if (ptr == NULL) {
		puts(try_falied);
		return;
	}

	//a, b, c input
	printf("a, b, c? ");
	for (int* p = size; p < size + sizeof(size) / sizeof(size[0]); p++)
		scanf("%d", p);

	//alloc
	ptr[0] = calloc((size[0] + size[1] + size[2]), sizeof(void*));
	//할당은 한 번에
	if (ptr[0] == NULL) {
		puts(try_falied);
		free(ptr);
		return;
	}
	//한번에 받은 할당을 크기에 맞추어 나눔
	for (int i = 0; i < sizeof(size) / sizeof(size[0]) - 1; i++)
		ptr[i + 1] = ptr[i] + size[i];

	//할당은 한 번에(int형 2차원 배열, double형 2차원 배열, char* 2차원 배열을 저장할 수 있는 공간
	ptr[0][0] = calloc(size[0] * size[1] * sizeof(int) + size[1] * size[2] * sizeof(double)
		+ size[2] * size[0] * sizeof(char*), 1);
	if (ptr[0] == NULL) {
		puts(try_falied);
		free(ptr[0]);
		free(ptr);
		return;
	}
	/*
	한번에 받은 할당을 크기에 맞추어 나누고, 남는 공간을 다음 배열(ptr[1])에 넘김
	위에서 할당을 한 번에 받고 이를 나누었기에 가능(2차원 배열 3개가 전부 연속하고 있기에 가능함)
	그러니까 ptr[0][size[0]] == ptr[1][0]이기에 가능함
	할당을 받는 데 많은 시간이 소요된다고 들었고, 이렇게 짜는 게 생각보다 코드가 복잡하지 않아 이렇게 진행함
	*/
	for (int** p = (int**)(ptr[0]); p < (int**)(ptr[0]) + size[0]; p++)
		*(p + 1) = *p + size[1];

	for (double** p = (double**)(ptr[1]); p < (double**)(ptr[1]) + size[1]; p++)
		*(p + 1) = *p + size[2];

	//(char*)의 2차원 배열, 즉 (char*)**임, 이해를 위해 띄어쓰기를 진행함
	for (char* ** p = (char* **)(ptr[2]); p < (char* **)(ptr[2]) + size[2] - 1; p++)
		*(p + 1) = *p + size[0];
	//ptr[2][size[2]]는 할당받지 않은 공간으로 접근 시 에러가 발생하므로 조건문에 -1을 넣어 이를 방지
	
	//input
	//위에서처럼 어차피 2차원 배열도 연결되어 있으므로 반복문 하나로 2차원 배열의 입력을 받을 수 있음
	for (int* p = (int*)ptr[0][0]; p < (int*)ptr[0][0] + size[0] * size[1]; p++)
		*p = intin++;

	for (double* p = (double*)ptr[1][0]; p < (double*)ptr[1][0] + size[1] * size[2]; p++, doublein += 0.1)
		*p = doublein;

	intin = 1;
	**((char* **)ptr[2]) = calloc(size[2] * size[0], sizeof(char*) * STRMAXLEN);
	if (**((char* **)ptr[2]) == NULL) {
		puts(try_falied);
		free(ptr[0][0]);
		free(ptr[0]);
		free(ptr);
		return;
	}
	sprintf(**((char* **)ptr[2]), "Hi%d", intin++);
	for (char* * p = (char* *)ptr[2][0] + 1; p < (char* *)ptr[2][0] + size[2] * size[0]; p++) {
		*p = *(p - 1) + STRMAXLEN;
		sprintf(*p, "Hi%d", intin++);
	}

	for (int** p1 = (int**)(ptr[0]); p1 < (int**)(ptr[0]) + size[0]; p1++) {
		for (int* p2 = *p1; p2 < *p1 + size[1]; p2++) printf("%d ", *p2);
		putchar('\n');
	}
	putchar('\n');

	for (double** p1 = (double**)(ptr[1]); p1 < (double**)(ptr[1]) + size[1]; p1++) {
		for (double* p2 = *p1; p2 < *p1 + size[2]; p2++) printf("%.1lf ", *p2);
		putchar('\n');
	}
	putchar('\n');

	for (char* ** p1 = (char* **)(ptr[2]); p1 < (char* **)(ptr[2]) + size[2]; p1++) {
		for (char* * p2 = *p1; p2 < *p1 + size[0]; p2++) printf("%s ", *p2);
		putchar('\n');
	}
	
	//free
	free(**((char* **)ptr[2]));
	free(ptr[0][0]);
	free(ptr[0]);
	free(ptr);
}
/*
for (int i = 0, across, down; i < sizeof(size) / sizeof(size[0]); i++) {
	across = size[i];
	if (i + 1 == sizeof(size) / sizeof(size[0])) down = size[0];
	else down = size[i + 1];

	for (int j = 1; j < down; j++) {
		ptr[i][j] = (int*)(ptr[i][j - 1]) + across;
	}
}
ptr[1][0] = calloc(size[1] * size[2], sizeof(double));
ptr[2][0] = calloc(size[2] * size[0], sizeof(char*));
*//*
2 3 2
1 2 3
4 5 6
1.0 1.1
1.2 1.3
1.4 1.5
hi1 hi2
hi3 hi4
*/