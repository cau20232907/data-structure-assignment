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

typedef struct _dictionary_read_result_tree {
	struct _dictionary_tree* dictionary;
	size_t numOfNodes;
} ReadDictTree;

typedef struct _dictionary_tree {
	struct _dictionary dictionary;
	struct _dictionary_tree* leftword;
	struct _dictionary_tree* rightword;
} WordNode;

#define dictionaryCompareFunction(word1, word2) (strcmp((word1)->en, (word2)->en) < 0)
#define FILELEN (1303699+1)
#define WORDLEN 48406
#define RELEN 1
#define FILEPOS "D:/OneDrive - 중앙대학교/2학년 1학기/자료구조/randdict.TXT"

ReadDictTree saveWordByTree(char* dicttxt);
void insertTree(WordNode* root, WordNode* target);
size_t treeDeep(WordNode* root);
ReadDict saveWordByArray(char* dicttxt);
void dictionaryQuicksort(Word* dictionary, size_t len);
ReadDictTree saveWordByTreeFromArray(ReadDict readResult);
ReadDictTree saveNonrootWord(Word* start, size_t len);
char* readWord(const char* const filename);
ReadDictTree search(WordNode* root, char* target);
void freeTree(WordNode* p);
char* trim(char* str);
void freeReadDict(ReadDict* p);
void freeset(void** p);

void main() {
	char in[51] = { 0, }; //검색
	char* str1 = readWord(FILEPOS), * str2 = readWord(FILEPOS);
	ReadDictTree assign1 = saveWordByTree(str1), assign2 = saveWordByTreeFromArray(saveWordByArray(str2));
	ReadDictTree searchres1, searchres2;

	printf("사전 파일을 모두 읽었습니다. 단순 트리 제작(과제 1)에는 %lld개, 개선 트리 제작(과제 2)에는 %lld개의 단어가 있습니다.\n", assign1.numOfNodes, assign2.numOfNodes);
	printf("단순 트리 제작(과제 1)의 트리 높이는 %lld, 개선 트리 제작(과제 2)의 트리 높이는 %lld입니다.\n", treeDeep(assign1.dictionary), treeDeep(assign2.dictionary));
	printf("트리의 노드 수는 단순 트리 제작(과제 1)에는 %lld개, 개선 트리 제작(과제 2)에는 %lld개 입니다.\n", assign1.numOfNodes, assign2.numOfNodes);

	while (!feof(stdin)) {
		printf("단어를 입력하세요: ");
		fgets(in, sizeof(in) / sizeof(in[0]), stdin);
		in[strlen(in) - 1] = '\0';
		searchres1 = search(assign1.dictionary, in);
		searchres2 = search(assign2.dictionary, in);
		/*
		if (!(searchres1.dictionary == NULL && searchres2.dictionary == NULL) &&
			(strcmp(searchres1.dictionary->dictionary.en, searchres2.dictionary->dictionary.en) ||
			strcmp(searchres1.dictionary->dictionary.ko, searchres2.dictionary->dictionary.ko))) {
			system("pause");
			exit(1);
		}*/
		if (searchres1.dictionary == NULL)
			puts("입력한 단어가 사전에 없습니다.");
		else
			printf("%s\t(단순 트리(과제 1) 레벨: %lld, 개선 트리(과제 1) 레벨: %lld)\n",
				searchres1.dictionary->dictionary.ko, searchres1.numOfNodes, searchres2.numOfNodes);
	}

	freeTree(assign1.dictionary);
	freeTree(assign2.dictionary);
	free(str1);
	free(str2);
}

ReadDictTree saveWordByTree(char* dicttxt) {
	WordNode* target;
	char* tmp;
	ReadDictTree result;
	result.numOfNodes = 0;
	result.dictionary = malloc(sizeof(WordNode));
	if (result.dictionary == NULL) {
		free(dicttxt);
		exit(1);
	}

	result.dictionary->dictionary.en = trim(strtok(dicttxt, ":"));
	result.dictionary->dictionary.ko = trim(strtok(NULL, "\n"));
	result.dictionary->leftword = NULL;
	result.dictionary->rightword = NULL;
	result.numOfNodes++;

	tmp = trim(strtok(NULL, ":"));
	while (tmp != NULL && *tmp != '\0') {
		target = malloc(sizeof(WordNode));
		if (target == NULL) {
			freeTree(result.dictionary);
			free(dicttxt);
			exit(1);
		}
		target->dictionary.en = tmp;
		target->dictionary.ko = trim(strtok(NULL, "\n"));
		//insert
		insertTree(result.dictionary, target);

		result.numOfNodes++;
		tmp = trim(strtok(NULL, ":"));
	}
	return result;
}

void insertTree(WordNode* root, WordNode* target) {
	target->leftword = NULL;
	target->rightword = NULL;
	while (1) {
		if (dictionaryCompareFunction(&root->dictionary, &target->dictionary)) {
			if (root->rightword == NULL) {
				root->rightword = target;
				return;
			}
			else root = root->rightword;
		}
		else {
			if (root->leftword == NULL) {
				root->leftword = target;
				return;
			}
			else root = root->leftword;
		}
	}
}

size_t treeDeep(WordNode* root) {
	size_t left = root->leftword == NULL ? 0 : treeDeep(root->leftword);
	size_t right = root->rightword == NULL ? 0 : treeDeep(root->rightword);
	if (left > right) return left + 1;
	else return right + 1;
}

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
		exit(1);
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
				exit(1);
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
	dictionaryQuicksort(result.dictionary, result.numOfWords);
	return result;
}

void dictionaryQuicksort(Word* dictionary, size_t len) {
	typedef struct _sortSettings {
		Word* start;
		Word* end;
	} quicksortThread;
	Word* pivot, * low, * right, * high, tmp;
	quicksortThread* sortQueue, * ps;
	sortQueue = calloc((len + 1) / 3, sizeof(quicksortThread));
	//둘씩 떨어져나가는 게 공간 복잡도 최악(1개는 계산하지 않기 때문)
	//pivot은 별도이므로 /3, 2/pivot/2/pivot/.../pivot/2로 나뉘는 게 최악이므로 /3 전 +1 진행(pivot 수가 하나 적을 수 있음)
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

ReadDictTree saveWordByTreeFromArray(ReadDict readResult) {
	return saveNonrootWord(readResult.dictionary, readResult.numOfWords);
}

ReadDictTree saveNonrootWord(Word* start, size_t len) {
	size_t mid = len / 2;
	ReadDictTree result = { 0, },tmp;
	result.dictionary = malloc(sizeof(WordNode));
	if (result.dictionary == NULL) exit(1);
	result.numOfNodes = 1;
	result.dictionary->dictionary = start[mid];
	if (mid) {
		tmp = saveNonrootWord(start, mid);
		result.dictionary->leftword = tmp.dictionary;
		result.numOfNodes += tmp.numOfNodes;
	}
	else result.dictionary->leftword = NULL;
	//2개 남으면 this, left로 들어감
	if (len > 2) {
		tmp = saveNonrootWord(start + mid + 1, len - mid - 1);
		result.dictionary->rightword = tmp.dictionary;
		result.numOfNodes += tmp.numOfNodes;
	}
	else result.dictionary->rightword = NULL;
	return result;
}

char* readWord(const char* const filename) {
	char* str = NULL;
	size_t alloclen = FILELEN;
	FILE* dictfile;
	char* pos = NULL;

	str = calloc(alloclen, sizeof(char));
	if (str == NULL) exit(1);

	dictfile = fopen(filename, "rt");
	if (dictfile == NULL) {
		freeset(&str);
		exit(1);
	}

	pos = str;
	while (!feof(dictfile)) {
		if (alloclen - (pos - str) == 1) { //재할당
			char* tmp = realloc(str, (alloclen + RELEN) * sizeof(char));
			if (tmp == NULL) {
				fclose(dictfile);
				freeset(&str);
				exit(1);
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

	return str;
}

ReadDictTree search(WordNode* root, char* target) {
	ReadDictTree result;
	result.dictionary = NULL;
	result.numOfNodes = 0;
	int cmpres;
	while (result.dictionary == NULL && root != NULL) {
		cmpres = strcmp(root->dictionary.en, target);
		if (cmpres < 0) root = root->rightword;
		else if (cmpres > 0) root = root->leftword;
		else result.dictionary = root;
		result.numOfNodes++;
	}
	return result;
}

void freeTree(WordNode* p) {
	if (p->leftword != NULL) freeTree(p->leftword);
	if (p->rightword != NULL) freeTree(p->rightword);
	free(p);
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