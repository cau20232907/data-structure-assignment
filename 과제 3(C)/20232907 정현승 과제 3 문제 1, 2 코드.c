#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

typedef enum _datastructure {
	array, LinkedList
} DataStructure;

typedef struct _dictionary {
	char* en;
	char* ko;
} Word;

typedef struct _dictionary_linked_list {
	struct _dictionary dictionary;
	struct _dictionary_linked_list* next;
} WordLinkedList;

typedef struct _dictionary_read_result {
	union {
		struct _dictionary_linked_list* LinkedListDictionaryStart;
		struct _dictionary* arrayDictionary;
	} dictionary;
	char* allocpt;
	size_t numOfWords;
	DataStructure dictionaryType;
} ReadDict;

ReadDict saveWordByArray(char* dicttxt);
ReadDict saveWordByLinkedList(char* dicttxt);
ReadDict readWord(const char* const filename, const DataStructure savetype);
Word* search(const ReadDict dictionary, const char* const target);
char* trim(char* str);
void freeReadDict(ReadDict* p);
void freeWordLinkedList(WordLinkedList* p);
void freeset(void**);

#define MAXWORDLEN 50 //�ܾ� �ϳ��� �ִ� ����(sgets�� �̹����� �������� �ʾ���)
#define FILEPOS "D:/OneDrive - �߾Ӵ��б�/2�г� 1�б�/�ڷᱸ��/randdict.TXT"

void main() {
	ReadDict dictionary = { 0, };
	char input[MAXWORDLEN + 1] = { 0, };
	Word* searchres = NULL;
	clock_t startc, endc;
	double linkedlistc, arraysortc;

	//2�� ���� start
	startc = clock();
	dictionary = readWord(FILEPOS, array);
	if (dictionary.numOfWords == 0) {
		puts("������ ���� �޸𸮰� �����մϴ�.");
		return;
	}
	endc = clock();
	arraysortc = (double)(endc - startc) / CLOCKS_PER_SEC;
	freeReadDict(&dictionary);

	startc = clock();
	dictionary = readWord(FILEPOS, LinkedList);
	if (dictionary.numOfWords == 0) {
		puts("������ ���� �޸𸮰� �����մϴ�.");
		return;
	}
	endc = clock();
	linkedlistc = (double)(endc - startc) / CLOCKS_PER_SEC;

	printf("�迭�� ���� �� ����: %.4f�� �ҿ�\n���Ḯ��Ʈ�� ������ ����: %.4f�� �ҿ�\n", arraysortc, linkedlistc);
	//2�� ���� end

	if (dictionary.numOfWords == 0) {
		puts("������ ���� �޸𸮰� �����մϴ�.");
		return;
	}

	while (!feof(stdin)) {
		printf(">> ");
		fgets(input, sizeof(input) / sizeof(input[0]), stdin);
		searchres = search(dictionary, trim(input));
		if (searchres) puts(searchres->ko);
		else puts("�ش��ϴ� �ܾ �����ϴ�.");
	}
	freeReadDict(&dictionary);
}

#define FILELEN (1303699+1)
#define WORDLEN 48406
#define RELEN 1

ReadDict saveWordByArray(char* dicttxt) {
	ReadDict result = { 0, };
	Word* cur = NULL;
	char* tmp;
	size_t alloclen = WORDLEN;

	result.dictionaryType = array;
	result.allocpt = dicttxt;
	//ù ��° ������ �ڵ尡 �ణ �ٸ��� ��
	result.dictionary.arrayDictionary = calloc(alloclen, sizeof(Word));
	if (result.dictionary.arrayDictionary == NULL) {
		freeset(&result.allocpt);
		return result;
	}
	cur = result.dictionary.arrayDictionary;

	tmp = trim(strtok(result.allocpt, ":"));
	while (tmp != NULL && *tmp != '\0') {
		if (cur == result.dictionary.arrayDictionary + alloclen) { //�߰� �뷮 �ʿ�
			Word* wtmp = realloc(result.dictionary.arrayDictionary, alloclen + RELEN);
			if (wtmp == NULL) {
				freeset(&result.allocpt);
				freeset(&result.dictionary.arrayDictionary);
				result.numOfWords = 0;
				return result;
			}
			result.dictionary.arrayDictionary = wtmp;
			cur = result.dictionary.arrayDictionary + alloclen;
			alloclen += RELEN;
		}

		cur->en = tmp;
		cur->ko = trim(strtok(NULL, "\n"));
		result.numOfWords++;
		cur++;
		tmp = trim(strtok(NULL, ":"));
	}

	for (Word* p = result.dictionary.arrayDictionary, *q, *min, tmp; p < result.dictionary.arrayDictionary + result.numOfWords - 1; p++) {
		for (min = p, q = p + 1; q < result.dictionary.arrayDictionary + result.numOfWords; q++)
			if (strcmp(min->en, q->en) < 0)
				min = q;
		//swap
		tmp = *min;
		*min = *p;
		*p = tmp;
	}

	return result;
}

ReadDict saveWordByLinkedList(char* dicttxt) {
	ReadDict result = { 0, };
	WordLinkedList* cur = NULL;
	WordLinkedList* prev = NULL;
	char* tmp;

	result.dictionaryType = LinkedList;
	result.allocpt = dicttxt;
	//ù ��° ������ �ڵ尡 �ణ �ٸ��� ��
	cur = malloc(sizeof(WordLinkedList));
	if (cur == NULL) {
		freeset(&result.allocpt);
		return result;
	}
	cur->dictionary.en = trim(strtok(result.allocpt, ":"));
	if (*cur->dictionary.en == NULL) {
		freeset(&cur);
		return result;
	}
	cur->dictionary.ko = trim(strtok(NULL, "\n"));
	cur->next = NULL;
	result.dictionary.LinkedListDictionaryStart = cur;
	result.numOfWords++;

	tmp = strtok(NULL, ":");
	while (tmp != NULL) {
		cur = malloc(sizeof(WordLinkedList));
		if (cur == NULL) {
			freeWordLinkedList(result.dictionary.LinkedListDictionaryStart);
			result.dictionary.LinkedListDictionaryStart = NULL;
			freeset(&result.allocpt);
			result.numOfWords = 0;
			return result;
		}
		cur->dictionary.en = trim(tmp);
		if (*cur->dictionary.en == '\0') { //���� ���� �Ϸ�
			freeset(&cur);
			return result;
		}
		cur->dictionary.ko = trim(strtok(NULL, "\n"));

		//����, ���� �� �տ� ������ Ȯ��
		if (strcmp(result.dictionary.LinkedListDictionaryStart->dictionary.en, cur->dictionary.en) > 0) {
			cur->next = result.dictionary.LinkedListDictionaryStart;
			result.dictionary.LinkedListDictionaryStart = cur;
		}
		//���� ��ġ ã��
		else {
			for (prev = result.dictionary.LinkedListDictionaryStart;
				prev->next != NULL && strcmp(prev->next->dictionary.en, cur->dictionary.en) < 0;
				prev = prev->next);
			cur->next = prev->next;
			prev->next = cur;
		}

		result.numOfWords++;

		tmp = strtok(NULL, ":");
	}

	return result;
}

ReadDict readWord(const char* const filename, const DataStructure savetype) {
	const ReadDict emptyResult = { 0, };
	const ReadDict(*saveProcess[2])(char*) = { saveWordByArray, saveWordByLinkedList };
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
		if (alloclen - (pos - str) == 1) { //���Ҵ�
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
		while (*pos) pos++; //'\0' ��ġ�� �ű��
	}
	//������ ������ ��� ����
	fclose(dictfile);

	return saveProcess[savetype](str);
}

Word* search(const ReadDict dictionary, const char* const target) {
	if (dictionary.dictionaryType == LinkedList) {
		WordLinkedList* cur = dictionary.dictionary.LinkedListDictionaryStart;
		while (cur != NULL && strcmp(cur->dictionary.en, target))
			cur = cur->next;
		if (cur == NULL) return NULL;
		else return &cur->dictionary;
	}
	else {
		size_t rangeStart = 0;
		size_t rangeEnd = dictionary.numOfWords - 1;
		int cmpres = 0;
		size_t pos = 0;
		while (rangeStart < rangeEnd) {
			pos = (rangeEnd - rangeStart) / 2 + rangeStart; //�����÷� ���
			cmpres = strcmp(dictionary.dictionary.arrayDictionary[pos].en, target);
			if (cmpres > 0) rangeStart = pos + 1;
			else if (cmpres < 0) rangeEnd = pos - 1;
			else return dictionary.dictionary.arrayDictionary + pos;
		}
		//rangeStart==rangeEnd
		if (!strcmp(dictionary.dictionary.arrayDictionary[rangeStart].en, target))
			return dictionary.dictionary.arrayDictionary + rangeStart;
		else return NULL;
	}
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
	//�� ���ڿ��̶� ��ȯ�ؾ� ������ ���� �� ����
}

void freeReadDict(ReadDict* p) {
	if (p->dictionaryType == LinkedList) {
		freeWordLinkedList(p->dictionary.LinkedListDictionaryStart);
		p->dictionary.LinkedListDictionaryStart = NULL;
	}
	else freeset(&p->dictionary.arrayDictionary);
	freeset(&p->allocpt);
	p->numOfWords = 0;
}

void freeWordLinkedList(WordLinkedList* p) {
	WordLinkedList* next = p->next;
	while (p) {
		next = p->next;
		free(p);
		p = next;
	}
}

void freeset(void** p) {
	if (*p != NULL) {
		free(*p);
		*p = NULL;
	}
}