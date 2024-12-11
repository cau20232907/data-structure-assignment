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
	//�Ҵ��� �� ����
	if (ptr[0] == NULL) {
		puts(try_falied);
		free(ptr);
		return;
	}
	//�ѹ��� ���� �Ҵ��� ũ�⿡ ���߾� ����
	for (int i = 0; i < sizeof(size) / sizeof(size[0]) - 1; i++)
		ptr[i + 1] = ptr[i] + size[i];

	//�Ҵ��� �� ����(int�� 2���� �迭, double�� 2���� �迭, char* 2���� �迭�� ������ �� �ִ� ����
	ptr[0][0] = calloc(size[0] * size[1] * sizeof(int) + size[1] * size[2] * sizeof(double)
		+ size[2] * size[0] * sizeof(char*), 1);
	if (ptr[0] == NULL) {
		puts(try_falied);
		free(ptr[0]);
		free(ptr);
		return;
	}
	/*
	�ѹ��� ���� �Ҵ��� ũ�⿡ ���߾� ������, ���� ������ ���� �迭(ptr[1])�� �ѱ�
	������ �Ҵ��� �� ���� �ް� �̸� �������⿡ ����(2���� �迭 3���� ���� �����ϰ� �ֱ⿡ ������)
	�׷��ϱ� ptr[0][size[0]] == ptr[1][0]�̱⿡ ������
	�Ҵ��� �޴� �� ���� �ð��� �ҿ�ȴٰ� �����, �̷��� ¥�� �� �������� �ڵ尡 �������� �ʾ� �̷��� ������
	*/
	for (int** p = (int**)(ptr[0]); p < (int**)(ptr[0]) + size[0]; p++)
		*(p + 1) = *p + size[1];

	for (double** p = (double**)(ptr[1]); p < (double**)(ptr[1]) + size[1]; p++)
		*(p + 1) = *p + size[2];

	//(char*)�� 2���� �迭, �� (char*)**��, ���ظ� ���� ���⸦ ������
	for (char* ** p = (char* **)(ptr[2]); p < (char* **)(ptr[2]) + size[2] - 1; p++)
		*(p + 1) = *p + size[0];
	//ptr[2][size[2]]�� �Ҵ���� ���� �������� ���� �� ������ �߻��ϹǷ� ���ǹ��� -1�� �־� �̸� ����
	
	//input
	//������ó�� ������ 2���� �迭�� ����Ǿ� �����Ƿ� �ݺ��� �ϳ��� 2���� �迭�� �Է��� ���� �� ����
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