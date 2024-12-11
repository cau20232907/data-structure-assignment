#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

typedef struct _dictionary {
	char* en;
	char* ko;
} Word;

typedef struct _dictionary_read_result {
	struct _dictionary* dictionary;
	char* allocpt;
	size_t numOfWords;
} ReadDict;

void dictionarySortCheck(const Word* dictionary, size_t len);
void dictionaryInsertionsort(Word* dictionary, size_t len);
void dictionaryQuicksort(Word* dictionary, size_t len);
void dictionaryHeapsort(Word* dictionary, size_t len);
void dictionaryHeapsortAdjusting(Word* dictionary, size_t rootIndex, size_t len);

int dictionaryCompareFunctionForInnersort(Word* p1, Word* p2);
ReadDict saveWordByArray(char* dicttxt);
ReadDict readWord(const char* const filename);
Word* search(const ReadDict dictionary, const char* const target);
char* trim(char* str);
void freeReadDict(ReadDict* p);
void freeset(void**);

#define FILEPOS "D:/OneDrive - 중앙대학교/2학년 1학기/자료구조/randdict.TXT"

void main() {
	ReadDict dictionary = { 0, };
	Word* tempArray = NULL;
	clock_t startc, endc;
	double insertionSortc, quickSortc, heapSortc;

	//1번 문제 start
	dictionary = readWord(FILEPOS);
	if (dictionary.numOfWords == 0) {
		puts("파일을 읽을 메모리가 부족합니다.");
		return;
	}

	tempArray = malloc(sizeof(Word) * dictionary.numOfWords);
	if (tempArray == NULL) {
		freeReadDict(&dictionary);
		exit(1);
	}

	//단순 정렬(1번)
	memcpy(tempArray, dictionary.dictionary, sizeof(Word) * dictionary.numOfWords);
	startc = clock();
	dictionaryInsertionsort(tempArray, dictionary.numOfWords);
	endc = clock();
	dictionarySortCheck(tempArray, dictionary.numOfWords);
	insertionSortc = (double)(endc - startc) / CLOCKS_PER_SEC;

	//단순 정렬(1번)
	memcpy(tempArray, dictionary.dictionary, sizeof(Word) * dictionary.numOfWords);
	startc = clock();
	dictionaryQuicksort(tempArray, dictionary.numOfWords);
	endc = clock();
	dictionarySortCheck(tempArray, dictionary.numOfWords);
	quickSortc = (double)(endc - startc) / CLOCKS_PER_SEC;

	//단순 정렬(1번)
	memcpy(tempArray, dictionary.dictionary, sizeof(Word) * dictionary.numOfWords);
	startc = clock();
	dictionaryHeapsort(tempArray, dictionary.numOfWords);
	endc = clock();
	dictionarySortCheck(tempArray, dictionary.numOfWords);
	heapSortc = (double)(endc - startc) / CLOCKS_PER_SEC;


	printf("삽입 정렬: %.4f초\n퀵 정렬: %.4f초\n힙 정렬: %.4f초", insertionSortc, quickSortc, heapSortc);

	free(tempArray);
	freeReadDict(&dictionary);
}

int dictionaryCompareFunctionForInnersort(Word* p1, Word* p2) {
	return strcmp(p1->en, p2->en);
}

#define dictionaryCompareFunction(word1, word2) (strcmp((word1)->en, (word2)->en) < 0)

void dictionarySortCheck(const Word* dictionary, size_t len) {
	for (const Word* p = dictionary + 1; p < dictionary + len; p++)
		if (!dictionaryCompareFunction(p - 1, p)) {
			printf("정렬 실패: index %lld 단어: %s, index %lld 단어:%s", p - dictionary - 1, (p - 1)->en, p - dictionary, p->en);
			system("pause");
			exit(1);
		}
}

void dictionaryInsertionsort(Word* dictionary, size_t len) {
	for (Word* p = dictionary + 1, *r, tmp; p < dictionary + len; p++) {
		tmp = *p;
		for (r = p; r > dictionary && dictionaryCompareFunction(&tmp, r - 1); r--)
			*r = *(r - 1);
		*r = tmp;
	}
}

void dictionaryQuicksort(Word* dictionary, size_t len) {
	typedef struct _sortSettings {
		Word* start;
		Word* end;
	} quicksortThread;
	Word* pivot, * low, * right, * high, tmp;
	quicksortThread* sortQueue, * ps;
	sortQueue = calloc((len + 1) / 3, sizeof(quicksortThread)); //둘씩 떨어져나가는 게 공간 복잡도 최악(1개는 계산하지 않기 때문), pivot은 별도이므로 /3, 2/pivot/2/pivot/.../pivot/2로 나뉘는 게 최악이므로 /3 전 +1 진행(pivot 수가 하나 적을 수 있음)
	if (sortQueue == NULL) exit(1);
	ps = sortQueue;
	ps->start = dictionary;
	ps->end = dictionary + len;
	while (ps >= sortQueue) {
		low = pivot = ps->start;
		high = right = ps->end;
		ps--;
		do low++;
		while (low < right && !dictionaryCompareFunction(pivot, low));
		do high--;
		while (high > pivot && dictionaryCompareFunction(pivot, high));
		while (low < high) {
			tmp = *low;
			*low = *high;
			*high = tmp;
			do low++;
			while (low < right && !dictionaryCompareFunction(pivot, low));
			do high--;
			while (high > pivot && dictionaryCompareFunction(pivot, high));
		}
		tmp = *high;
		*high = *pivot;
		*pivot = tmp;

		if (high - pivot > 1) {
			ps++;
			ps->start = pivot;
			ps->end = high;
		}
		if (right - low > 1) {
			ps++;
			ps->start = low;
			ps->end = right;
		}
	}
}

void dictionaryHeapsort(Word* dictionary, size_t len) {
	Word* const tree = dictionary - 1;
	for (size_t i = len / 2; i > 0; i--)
		dictionaryHeapsortAdjusting(tree, i, len);
	for (Word* p = tree + len, tmp; p > dictionary; p--) {
		tmp = *dictionary;
		*dictionary = *p;
		*p = tmp;
		dictionaryHeapsortAdjusting(tree, 1, p - dictionary);
	}
}

void dictionaryHeapsortAdjusting(Word* dictionary, size_t rootIndex, size_t len) {
	Word rootVal = dictionary[rootIndex];
	size_t childIndex = rootIndex * 2;
	while (childIndex <= len) {
		if (childIndex < len && dictionaryCompareFunction(dictionary + childIndex, dictionary + childIndex + 1)) childIndex++;
		if (!dictionaryCompareFunction(&rootVal, dictionary + childIndex))
			break;
		dictionary[childIndex / 2] = dictionary[childIndex];
		childIndex *= 2;
	}
	dictionary[childIndex / 2] = rootVal;
}

#define FILELEN (1303699+1)
#define WORDLEN 48406
#define RELEN 1

ReadDict saveWordByArray(char* dicttxt) {
	ReadDict result = { 0, };
	Word* cur = NULL;
	char* tmp;
	size_t alloclen = WORDLEN;

	result.allocpt = dicttxt;
	//첫 번째 저장은 코드가 약간 다르니 뺌
	result.dictionary = calloc(alloclen, sizeof(Word));
	if (result.dictionary == NULL) {
		freeset(&result.allocpt);
		return result;
	}
	cur = result.dictionary;

	tmp = trim(strtok(result.allocpt, ":"));
	while (tmp != NULL && *tmp != '\0') {
		if (cur == result.dictionary + alloclen) { //추가 용량 필요
			Word* wtmp = realloc(result.dictionary, alloclen + RELEN);
			if (wtmp == NULL) {
				freeset(&result.allocpt);
				freeset(&result.dictionary);
				result.numOfWords = 0;
				return result;
			}
			result.dictionary = wtmp;
			cur = result.dictionary + alloclen;
			alloclen += RELEN;
		}

		cur->en = tmp;
		cur->ko = trim(strtok(NULL, "\n"));
		result.numOfWords++;
		cur++;
		tmp = trim(strtok(NULL, ":"));
	}

	return result;
}

ReadDict readWord(const char* const filename) {
	const ReadDict emptyResult = { 0, };
	char* str = NULL;
	size_t alloclen = FILELEN;
	FILE* dictfile;
	char* pos = NULL;

	str = calloc(alloclen, sizeof(char));
	if (str == NULL) return emptyResult;

	dictfile = fopen(filename, "rt");
	if (dictfile == NULL) {
		freeset(&str);
		return emptyResult;
	}

	pos = str;
	while (!feof(dictfile)) {
		if (alloclen - (pos - str) == 1) { //재할당
			char* tmp = realloc(str, (alloclen + RELEN) * sizeof(char));
			if (tmp == NULL) {
				fclose(dictfile);
				freeset(&str);
				return emptyResult;
			}
			str = tmp;
			pos = str + alloclen - 1;
			alloclen += RELEN;
		}
		fgets(pos, alloclen - (pos - str), dictfile);
		while (*pos) pos++; //'\0' 위치로 옮기기
	}
	//파일의 내용을 모두 읽음
	fclose(dictfile);

	return saveWordByArray(str);
}

Word* search(const ReadDict dictionary, const char* const target) {
	size_t rangeStart = 0;
	size_t rangeEnd = dictionary.numOfWords - 1;
	int cmpres = 0;
	size_t pos = 0;
	while (rangeStart < rangeEnd) {
		pos = (rangeEnd - rangeStart) / 2 + rangeStart; //오버플로 고려
		cmpres = strcmp(dictionary.dictionary[pos].en, target);
		if (cmpres > 0) rangeStart = pos + 1;
		else if (cmpres < 0) rangeEnd = pos - 1;
		else return dictionary.dictionary + pos;
	}
	//rangeStart==rangeEnd
	if (!strcmp(dictionary.dictionary[rangeStart].en, target))
		return dictionary.dictionary + rangeStart;
	else return NULL;
}

char* trim(char* str) {
	char* res = NULL;
	if (str == NULL) return NULL;
	while (*str != '\0' && (*str == ' ' || *str == '\n' || *str == '\t')) { //trim
		*str = '\0';
		str++;
	}
	res = str;
	str = res + strlen(res) - 1;
	while ((*str == ' ' || *str == '\n' || *str == '\t') && str >= res) { //trim
		*str = '\0';
		str--;
	}
	return res;
	//빈 문자열이라도 반환해야 에러를 막을 수 있음
}

void freeReadDict(ReadDict* p) {
	freeset(&p->dictionary);
	freeset(&p->allocpt);
	p->numOfWords = 0;
}

void freeset(void** p) {
	if (*p != NULL) {
		free(*p);
		*p = NULL;
	}
}