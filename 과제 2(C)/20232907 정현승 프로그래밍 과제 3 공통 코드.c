#include "20232907 ������ ���α׷��� ���� 3 ���� ���.h"

const char* sgets(int len, int relen, FILE* _Stream) { //�Ҵ���� ���̴� return���� ����: �־��� ���� �� ��� �Ұ�
	//len�� �����Ҵ�, relen�� ���Ҵ�� ����
	//FILE* _Stream �� fgets�� �״�� ���� ���̰� �� �̿��� Ȱ���� ����(fgets�� �Ű������� �̸����� �״�� ������ ��)
	char* res = NULL, * ptmp = NULL, * pos = NULL; //���� string ����, �ܾ� ���� ��ġ ����, ���� Ŀ�� ����(in ���� �ϳ��� ��� �Է¹��� �����̹Ƿ� ���), ������ ���� �� � Ȱ���ϴ� �ӽ� �����ͺ���
	size_t res_len = 0; //���� res�� �Ҵ���� ����
	int tmp = 0;

	//�����Ҵ�
	res = (char*)calloc_in(len * sizeof(char));
	if (!res) return NULL;
	res_len += len;

	//�Է�
	pos = res;
	while (1) {
		fgets(pos, res_len - (pos - res), _Stream);
		pos += strlen(pos); //Ŀ�� �̵�, ù ��° NULL���ڸ� ����Ű�� ��
		if (*(pos - 1) == '\n') { //���� ���� ���๮�ڸ� �Է��� �������Ƿ� return
			*(pos - 1) = '\0'; //���� �� ���๮�� ����
			return res;
		}
		else { //�ƴϸ� ���� ������ �� ���� �� �ƴϰ� �ִ� �뷮�� ���������Ƿ� realloc ����
			ptmp = realloc_in(res, res_len + relen * sizeof(char));
			if (ptmp) {
				res = ptmp;
				pos = res + res_len - 1; //res�� ��ġ�� ����� �� ������ pos ��ġ�� ����(���̰� �ִ�� �� ���̹Ƿ� ���� res_len-1(������ ĭ '\0'���� ���� �Է� ����)�� ����Ű�� ��
				res_len += relen; //realloc ������ ���� ũ�� ����
			}
			else { //realloc ���н�
				freeset(&res);
				return NULL;
			}
		}
	}
}

void* calloc_in(const size_t len) {
	//calloc ��� + ���н� ��õ�
	unsigned int cnt = 0;
	void* tmp = NULL;
	while (!tmp && cnt < realloc_try) {
		cnt++;
		tmp = malloc(len);
	}
	if (tmp) {
		memset(tmp, 0, len);
		return tmp;
	}
	else {
		puts(try_falied);
		return NULL;
	}
}

void* realloc_in(void* to, const size_t len) {
	//realloc ��� + ���н� ��õ�
	unsigned int cnt = 0;
	void* tmp = NULL;
	while (!tmp && cnt < realloc_try) {
		cnt++;
		tmp = realloc(to, len);
	}
	if (tmp) return tmp;
	else {
		puts(try_falied);
		return NULL;
	}
}

void freeset(void** p) {
	if (*p != NULL) {
		free(*p);
		*p = NULL;
	}
}