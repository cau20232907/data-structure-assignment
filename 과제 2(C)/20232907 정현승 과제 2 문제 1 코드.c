#include "20232907 정현승 프로그래밍 과제 3 공통 헤더.h"

typedef struct my_string {
	char* string;
	size_t strlen;
	size_t alloclen;
} new_string;

new_string new_gets(int len, int relen, FILE* _Stream); //개행시까지 거의 무제한으로 문장을 입력받는 함수
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
	new_strcpy(&c, a); // a의 내용을 c에 복사한다.
	new_strcpy(&d, a); // a의 내용을 d에 복사한다.
	new_strcat(&d, b); // b의 내용을 d에 붙인다.
	printf("%s %s %d %d\n", new_str(c), new_str(d), new_strlen(c), new_strlen(d));
	freeset_newstring(&a);
	freeset_newstring(&b);
	freeset_newstring(&c);
	freeset_newstring(&d);
}

new_string new_gets(int len, int relen, FILE* _Stream) { //할당받은 길이는 return하지 않음: 주어진 길이 외 사용 불가
	//len은 최초할당, relen은 재할당시 길이
	//FILE* _Stream 은 fgets에 그대로 넣을 것이고 이 이외의 활용은 없음(fgets의 매개변수를 이름까지 그대로 가져온 것)
	char* ptmp = NULL, * pos = NULL; //각각 string 저장, 단어 시작 위치 저장, 현재 커서 저장(in 변수 하나에 계속 입력받을 예정이므로 사용), 포인터 변경 시 등에 활용하는 임시 포인터변수
	new_string res = {0};
	int tmp = 0;

	//최초할당
	res.string = (char*)calloc_in(len * sizeof(char));
	if (!res.string) return res;
	res.alloclen += len;

	//입력
	pos = res.string;
	while (1) {
		fgets(pos, res.alloclen - (pos - res.string), _Stream);
		tmp = strlen(pos);
		res.strlen += tmp;
		pos += tmp; //커서 이동, 첫 번째 NULL문자를 가리키게 됨
		if (*(pos - 1) == '\n') { //문장 끝이 개행문자면 입력이 끝났으므로 return
			*(pos - 1) = '\0'; //문장 끝 개행문자 삭제
			return res;
		}
		else { //아니면 아직 문장을 다 받은 게 아니고 최대 용량에 도달했으므로 realloc 시행
			ptmp = realloc_in(res.string, res.alloclen + relen * sizeof(char));
			if (ptmp) {
				res.string = ptmp;
				pos = res.string + res.alloclen - 1; //res의 위치가 변경될 수 있으니 pos 위치도 변경(길이가 최대로 찬 것이므로 기존 res_len-1(마지막 칸 '\0'부터 다음 입력 시작)을 가리키게 함
				res.alloclen += relen; //realloc 성공시 현재 크기 변경
			}
			else { //realloc 실패시
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