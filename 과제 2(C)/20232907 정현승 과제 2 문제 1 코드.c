#include "20232907 ������ ���α׷��� ���� 3 ���� ���.h"

typedef struct my_string {
	char* string;
	size_t strlen;
	size_t alloclen;
} new_string;

new_string new_gets(int len, int relen, FILE* _Stream); //����ñ��� ���� ���������� ������ �Է¹޴� �Լ�
new_string create_str(char* obj);
size_t new_strlen(new_string str);
void new_strcpy(new_string* dest, const new_string src);
void new_strcat(new_string* dest, const new_string src);
char* new_str(new_string obj);
void freeset_newstring(new_string* p);

void main() {
	new_string a = { 0 }, b = { 0 }, c = { 0 }, d = { 0 };
	a = create_str("Hello world");
	b = new_gets(10, 10, stdin);
	new_strcpy(&c, a); // a�� ������ c�� �����Ѵ�.
	new_strcpy(&d, a); // a�� ������ d�� �����Ѵ�.
	new_strcat(&d, b); // b�� ������ d�� ���δ�.
	printf("%s %s %d %d\n", new_str(c), new_str(d), new_strlen(c), new_strlen(d));
	freeset_newstring(&a);
	freeset_newstring(&b);
	freeset_newstring(&c);
	freeset_newstring(&d);
}

new_string new_gets(int len, int relen, FILE* _Stream) { //�Ҵ���� ���̴� return���� ����: �־��� ���� �� ��� �Ұ�
	//len�� �����Ҵ�, relen�� ���Ҵ�� ����
	//FILE* _Stream �� fgets�� �״�� ���� ���̰� �� �̿��� Ȱ���� ����(fgets�� �Ű������� �̸����� �״�� ������ ��)
	char* ptmp = NULL, * pos = NULL; //���� string ����, �ܾ� ���� ��ġ ����, ���� Ŀ�� ����(in ���� �ϳ��� ��� �Է¹��� �����̹Ƿ� ���), ������ ���� �� � Ȱ���ϴ� �ӽ� �����ͺ���
	new_string res = {0};
	int tmp = 0;

	//�����Ҵ�
	res.string = (char*)calloc_in(len * sizeof(char));
	if (!res.string) return res;
	res.alloclen += len;

	//�Է�
	pos = res.string;
	while (1) {
		fgets(pos, res.alloclen - (pos - res.string), _Stream);
		tmp = strlen(pos);
		res.strlen += tmp;
		pos += tmp; //Ŀ�� �̵�, ù ��° NULL���ڸ� ����Ű�� ��
		if (*(pos - 1) == '\n') { //���� ���� ���๮�ڸ� �Է��� �������Ƿ� return
			*(pos - 1) = '\0'; //���� �� ���๮�� ����
			return res;
		}
		else { //�ƴϸ� ���� ������ �� ���� �� �ƴϰ� �ִ� �뷮�� ���������Ƿ� realloc ����
			ptmp = realloc_in(res.string, res.alloclen + relen * sizeof(char));
			if (ptmp) {
				res.string = ptmp;
				pos = res.string + res.alloclen - 1; //res�� ��ġ�� ����� �� ������ pos ��ġ�� ����(���̰� �ִ�� �� ���̹Ƿ� ���� res_len-1(������ ĭ '\0'���� ���� �Է� ����)�� ����Ű�� ��
				res.alloclen += relen; //realloc ������ ���� ũ�� ����
			}
			else { //realloc ���н�
				freeset_newstring(&res);
				return res;
			}
		}
	}
}

new_string create_str(char* obj) {
	new_string src = { 0, }, res = { 0, };
	src.string = obj;
	src.strlen = strlen(obj);
	new_strcpy(&res, src);
	return res;
}

size_t new_strlen(new_string str) {
	return str.strlen;
}

void new_strcpy(new_string* dest, const new_string src) {
	freeset_newstring(dest);
	dest->alloclen = (src.strlen + 1) * sizeof(char);
	dest->string = calloc_in(dest->alloclen);
	if (dest->string == NULL) {
		dest->alloclen = 0;
		dest->strlen = 0;
		return;
	}
	memcpy(dest->string, src.string, dest->alloclen);
	dest->strlen = src.strlen;
}

void new_strcat(new_string* dest, const new_string src) {
	char* tmp = NULL;
	if (dest->string == NULL) {
		new_strcpy(dest, src);
		return;
	}
	else if (dest->alloclen - 1 < src.strlen + dest->strlen) {
		dest->alloclen = (src.strlen + dest->strlen + 1) * sizeof(char);
		tmp = realloc_in(dest->string, dest->alloclen);
		if (tmp == NULL) {
			freeset_newstring(dest);
			return;
		}
		else {
			dest->string = tmp;
		}
	}
	memcpy(dest->string + dest->strlen, src.string, src.strlen + 1);
	dest->strlen += src.strlen;
}

char* new_str(new_string obj) {
	return obj.string;
}

void freeset_newstring(new_string* p) {
	if (p->string != NULL) {
		freeset(&p->string);
	}
	p->strlen = 0;
	p->alloclen = 0;
}