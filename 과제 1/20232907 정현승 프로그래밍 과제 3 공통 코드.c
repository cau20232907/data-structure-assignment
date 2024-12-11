#include "20232907 정현승 프로그래밍 과제 3 공통 헤더.h"

const char* sgets(int len, int relen, FILE* _Stream) { //할당받은 길이는 return하지 않음: 주어진 길이 외 사용 불가
	//len은 최초할당, relen은 재할당시 길이
	//FILE* _Stream 은 fgets에 그대로 넣을 것이고 이 이외의 활용은 없음(fgets의 매개변수를 이름까지 그대로 가져온 것)
	char* res = NULL, * ptmp = NULL, * pos = NULL; //각각 string 저장, 단어 시작 위치 저장, 현재 커서 저장(in 변수 하나에 계속 입력받을 예정이므로 사용), 포인터 변경 시 등에 활용하는 임시 포인터변수
	size_t res_len = 0; //현재 res의 할당받은 길이
	int tmp = 0;

	//최초할당
	res = (char*)calloc_in(len * sizeof(char));
	if (!res) return NULL;
	res_len += len;

	//입력
	pos = res;
	while (1) {
		fgets(pos, res_len - (pos - res), _Stream);
		pos += strlen(pos); //커서 이동, 첫 번째 NULL문자를 가리키게 됨
		if (*(pos - 1) == '\n') { //문장 끝이 개행문자면 입력이 끝났으므로 return
			*(pos - 1) = '\0'; //문장 끝 개행문자 삭제
			return res;
		}
		else { //아니면 아직 문장을 다 받은 게 아니고 최대 용량에 도달했으므로 realloc 시행
			ptmp = realloc_in(res, res_len + relen * sizeof(char));
			if (ptmp) {
				res = ptmp;
				pos = res + res_len - 1; //res의 위치가 변경될 수 있으니 pos 위치도 변경(길이가 최대로 찬 것이므로 기존 res_len-1(마지막 칸 '\0'부터 다음 입력 시작)을 가리키게 함
				res_len += relen; //realloc 성공시 현재 크기 변경
			}
			else { //realloc 실패시
				freeset(&res);
				return NULL;
			}
		}
	}
}

void* calloc_in(const size_t len) {
	//calloc 기능 + 실패시 재시도
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
	//realloc 기능 + 실패시 재시도
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