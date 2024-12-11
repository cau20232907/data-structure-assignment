#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define FILELEN (2739+1)
#define LINELEN 11
#define RELEN 1
#define STATIONLEN 10
#define STATIONRELEN 5
#define MAXTRANSFER 4
#define STATIONINLEN 101
#define FILEPOS "subway_20232907.txt"

typedef struct _single_station {
	struct _line_info* line;
	struct _station_table* station;
} Station;

typedef struct _station_table {
	char* stationName;
	char* stationNum;
	int distance;
	char numOfTransfers;
	struct _single_station transfers[MAXTRANSFER];
} StationTable;

typedef struct _line_info {
	char* lineName;
	struct _station_table* table;
	size_t tablelen;
} Line;

typedef struct _read_line {
	Line* lineinfo;
	size_t len;
} ReadLine;

typedef struct _station_distance {
	struct _single_station stn;
	//struct _single_station* from;
	int distance;
	char from;
} StationDist;

typedef struct _distance_alloc {
	struct _station_distance* stnlist;
	size_t len;
	size_t alloclen;
} Distances;

typedef struct _station_search_result {
	struct _single_station* stn;
	size_t len;
} StationSearchRes;

int calculateDistance(const ReadLine map, const Station start, const Station end);
int reversecomparedist(const StationDist* const p1, const StationDist* const p2);
StationDist getNextStationDistance(const StationDist target);
StationDist getPrevStationDistance(const StationDist target);
void DistanceInitialize(Distances* p, size_t len);
void DistancePush(Distances* p, StationDist to, size_t relen);
void DistanceStationPush(Distances* p, Station stn, int dist, size_t relen);
StationSearchRes searchStationByName(const ReadLine map, const char* const name);
ReadLine makeLine(char* readtxt);
char* readWord(const char* const filename);
void freeset(void** p);

void main() {
	char* in = readWord(FILEPOS), *pos;
	ReadLine line = makeLine(in);
	char start[STATIONINLEN], end[STATIONINLEN];
	StationSearchRes beg = { 0, }, fin = { 0, };
	int res;
	puts("���ۿ��� �������� �Է��� �ּ���.");
	printf("���ۿ�: ");
	fgets(start, sizeof(start) / sizeof(start[0]), stdin);
	printf("������: ");
	fgets(end, sizeof(end) / sizeof(end[0]), stdin);

	for (pos = start; *pos != '\0' && *pos != '\n'; pos++);
	if (*pos == '\n') {
		*pos = '\0';
		beg = searchStationByName(line, start);
	}
	for (pos = end; *pos != '\0' && *pos != '\n'; pos++);
	if (*pos == '\n') {
		*pos = '\0';
		fin = searchStationByName(line, end);
	}
	if (beg.len == 0 || fin.len == 0) {
		if (beg.len == 0) printf("%s ���� �������� �ʽ��ϴ�.\n", start);
		if (fin.len == 0) printf("%s ���� �������� �ʽ��ϴ�.\n", end);
		return;
	}
	for(Station*p=beg.stn;p<beg.stn+beg.len;p++)
		for (Station* q = fin.stn; q < fin.stn + fin.len; q++) {
			res = calculateDistance(line, *p, *q);
			printf("%s %s���� %s %s���� �Ÿ�: %d.%dkm\n", p->line->lineName, p->station->stationName, q->line->lineName, q->station->stationName, res / 10, res % 10);
		}
	
	free(beg.stn);
	free(fin.stn);
	for (Line* p = line.lineinfo; p < line.lineinfo + line.len; p++)
		free(p->table);
	free(line.lineinfo);
	free(in);
}

int calculateDistance(const ReadLine map, const Station start, const Station end) {
	Distances distances[5];
	//���ۿ��� ����ϴ� �׷�, �� �׷쿡 �����ϴ� ����, ������ ����ϴ� �׷�, �� �׷쿡 �����ϴ� ����, �ӽ� ����
	StationDist tmp, target;
	int result = 0;
	char flag;
	for (Distances* p = distances; p < distances + sizeof(distances) / sizeof(distances[0]); p++)
		DistanceInitialize(p, STATIONLEN);

	DistanceStationPush(distances, start, 0, STATIONRELEN);
	for (Station* p = start.station->transfers; p < start.station->transfers + start.station->numOfTransfers; p++)
		DistanceStationPush(distances, *p, 0, STATIONLEN);

	DistanceStationPush(distances + 2, end, 0, STATIONRELEN);
	for (Station* p = end.station->transfers; p < end.station->transfers + end.station->numOfTransfers; p++)
		DistanceStationPush(distances + 2, *p, 0, STATIONLEN);

	for (StationDist* p = distances[0].stnlist; p < distances[0].stnlist + distances[0].len; p++) {
		DistancePush(distances + 4, getNextStationDistance(*p), STATIONRELEN);
		DistancePush(distances + 4, getPrevStationDistance(*p), STATIONRELEN);
	}
	for (StationDist* p = distances[4].stnlist; p < distances[4].stnlist + distances[4].len; p++) {
		flag = 0;

		//����� �� �� �ϳ��� �ִ°�
		for (StationDist* q = distances[1].stnlist; !flag && q < distances[1].stnlist + distances[1].len; q++) {
			if (p->stn.station == q->stn.station) {
				flag = 1;
				if (p->distance < q->distance)
					q->distance = p->distance;
				break;
			}
			for (Station* r = p->stn.station->transfers; r < p->stn.station->transfers + p->stn.station->numOfTransfers; r++) {
				if (r->station == q->stn.station) {
					flag = 1;
					if (p->distance < q->distance)
						q->distance = p->distance;
					break;
				}
			}
		}
		if (!flag) DistancePush(distances + 1, *p, STATIONRELEN);
	}

	distances[4].len = 0;
	for (StationDist* p = distances[2].stnlist; p < distances[2].stnlist + distances[2].len; p++) {
		DistancePush(distances + 4, getNextStationDistance(*p), STATIONRELEN);
		DistancePush(distances + 4, getPrevStationDistance(*p), STATIONRELEN);
	}
	for (StationDist* p = distances[4].stnlist; p < distances[4].stnlist + distances[4].len; p++) {
		flag = 0;

		//����� �� �� �ϳ��� �ִ°�
		for (StationDist* q = distances[3].stnlist; !flag && q < distances[3].stnlist + distances[3].len; q++) {
			if (p->stn.station == q->stn.station) {
				flag = 1;
				if (p->distance < q->distance)
					q->distance = p->distance;
				break;
			}
			for (Station* r = p->stn.station->transfers; r < p->stn.station->transfers + p->stn.station->numOfTransfers; r++) {
				if (r->station == q->stn.station) {
					flag = 1;
					if (p->distance < q->distance)
						q->distance = p->distance;
					break;
				}
			}
		}
		if (!flag) DistancePush(distances + 3, *p, STATIONRELEN);
	}

	//initialize ��

	//���ۿ��� ������ ���� ���
	for (StationDist* q = distances[0].stnlist; q < distances[0].stnlist + distances[0].len; q++) {
		if (end.station == q->stn.station) {
			for (Distances* p = distances; p < distances + sizeof(distances) / sizeof(distances[0]); p++)
				free(p->stnlist);
			return 0;
		}
	}
	while (!result) {
		//���ۿ��� ���
		qsort(distances[1].stnlist, distances[1].len, sizeof(StationDist), reversecomparedist);
		target = distances[1].stnlist[--distances[1].len]; //pop
		//������ �� �Ͱ� ��ġ���� Ȯ��
		for (StationDist* q = distances[2].stnlist; q < distances[2].stnlist + distances[2].len; q++)
			if (target.stn.station == q->stn.station) {
				result = target.distance + q->distance;
				break;
			}

		distances[4].len = 0;
		if (target.from != 1) DistancePush(distances + 4, getNextStationDistance(target), STATIONRELEN); //���� ��
		if (target.from != -1) DistancePush(distances + 4, getPrevStationDistance(target), STATIONRELEN);
		DistancePush(distances, target, STATIONRELEN);
		//ȯ�¿�
		for (Station* p = target.stn.station->transfers; p < target.stn.station->transfers + target.stn.station->numOfTransfers; p++) {
			tmp.stn = *p;
			tmp.from = 0;
			tmp.distance = target.distance;
			DistancePush(distances + 4, getNextStationDistance(tmp), STATIONRELEN); //ȯ�¿��� ������
			DistancePush(distances + 4, getPrevStationDistance(tmp), STATIONRELEN);

			DistancePush(distances, tmp, STATIONLEN);
			//ȯ�¿��� �ϳ��� ������ ��� ��� �߰��Ǳ⿡ ������ �� �Ͱ� ��ġ���� Ȯ���� �ʿ� ����
		}

		for (StationDist* p = distances[4].stnlist; p < distances[4].stnlist + distances[4].len; p++) {
			//���� ��Ͽ� �ִ°�
			flag = 0;
			for (StationDist* q = distances[0].stnlist; q < distances[0].stnlist + distances[0].len; q++) {
				if (p->stn.station == q->stn.station) {
					flag = -1;
					break;
				}
			}
			if (flag) continue;

			//����� �� �� �ϳ��� �ִ°�
			for (StationDist* q = distances[1].stnlist; !flag && q < distances[1].stnlist + distances[1].len; q++) {
				if (p->stn.station == q->stn.station) {
					flag = 1;
					if (p->distance < q->distance)
						q->distance = p->distance;
					break;
				}
				for (Station* r = p->stn.station->transfers; r < p->stn.station->transfers + p->stn.station->numOfTransfers; r++) {
					if (r->station == q->stn.station) {
						flag = 1;
						if (p->distance < q->distance)
							q->distance = p->distance;
						break;
					}
				}
			}
			if (!flag) DistancePush(distances + 1, *p, STATIONRELEN);
		}

		//������ ���
		qsort(distances[3].stnlist, distances[3].len, sizeof(StationDist), reversecomparedist);
		target = distances[3].stnlist[--distances[3].len]; //pop
		//ó������ �� �Ͱ� ��ġ���� Ȯ��
		for (StationDist* q = distances[0].stnlist; q < distances[0].stnlist + distances[0].len; q++)
			if (target.stn.station == q->stn.station) {
				result = target.distance + q->distance;
				break;
			}

		distances[4].len = 0;
		if (target.from != 1) DistancePush(distances + 4, getNextStationDistance(target), STATIONRELEN); //���� ��
		if (target.from != -1) DistancePush(distances + 4, getPrevStationDistance(target), STATIONRELEN);
		DistancePush(distances + 2, target, STATIONRELEN);
		//ȯ�¿�
		for (Station* p = target.stn.station->transfers; p < target.stn.station->transfers + target.stn.station->numOfTransfers; p++) {
			tmp.stn = *p;
			tmp.from = 0;
			tmp.distance = target.distance;
			DistancePush(distances + 4, getNextStationDistance(tmp), STATIONRELEN); //ȯ�¿��� ������
			DistancePush(distances + 4, getPrevStationDistance(tmp), STATIONRELEN);

			DistancePush(distances + 2, tmp, STATIONLEN);
			//ȯ�¿��� �ϳ��� ������ ��� ��� �߰��Ǳ⿡ ������ �� �Ͱ� ��ġ���� Ȯ���� �ʿ� ����
		}

		for (StationDist* p = distances[4].stnlist; p < distances[4].stnlist + distances[4].len; p++) {
			//���� ��Ͽ� �ִ°�
			flag = 0;
			for (StationDist* q = distances[2].stnlist; q < distances[2].stnlist + distances[2].len; q++) {
				if (p->stn.station == q->stn.station) {
					flag = -1;
					break;
				}
			}
			if (flag) continue;

			//����� �� �� �ϳ��� �ִ°�
			for (StationDist* q = distances[3].stnlist; !flag && q < distances[3].stnlist + distances[3].len; q++) {
				if (p->stn.station == q->stn.station) {
					flag = 1;
					if (p->distance < q->distance)
						q->distance = p->distance;
					break;
				}
				for (Station* r = p->stn.station->transfers; r < p->stn.station->transfers + p->stn.station->numOfTransfers; r++) {
					if (r->station == q->stn.station) {
						flag = 1;
						if (p->distance < q->distance)
							q->distance = p->distance;
						break;
					}
				}
			}
			if (!flag) DistancePush(distances + 3, *p, STATIONRELEN);
		}
	}

	for (Distances* p = distances; p < distances + sizeof(distances) / sizeof(distances[0]); p++)
		free(p->stnlist);
	return result;
}

int reversecomparedist(const StationDist* const p1, const StationDist* const p2) {
	return p2->distance - p1->distance;
}

StationDist getNextStationDistance(const StationDist target) {
	StationDist result = { 0, };
	if (target.stn.station + 1 == target.stn.line->table + target.stn.line->tablelen) return result;
	result.stn.line = target.stn.line;
	result.stn.station = target.stn.station + 1;
	result.distance = target.distance + result.stn.station->distance - target.stn.station->distance;
	result.from = -1;
	return result;
}

StationDist getPrevStationDistance(const StationDist target) {
	StationDist result = { 0, };
	if (target.stn.station == target.stn.line->table) return result;
	result.stn.line = target.stn.line;
	result.stn.station = target.stn.station - 1;
	result.distance = target.distance + target.stn.station->distance - result.stn.station->distance;
	result.from = 1;
	return result;
}

void DistanceInitialize(Distances* p, size_t len) {
	p->stnlist=calloc(len, sizeof(StationDist));
	if (p->stnlist == NULL) exit(1);
	p->alloclen = len;
	p->len = 0;
}

void DistancePush(Distances* p, StationDist to, size_t relen) {
	if (to.stn.line == NULL) return;
	if (p->len == p->alloclen) {
		StationDist* tmp = realloc(p->stnlist, (p->alloclen + relen) * sizeof(StationDist));
		if (tmp == NULL) {
			free(p->stnlist);
			exit(1);
		}
		p->stnlist = tmp;
		p->alloclen += relen;
	}
	p->stnlist[p->len++] = to;
}

void DistanceStationPush(Distances* p, Station stn, int dist, size_t relen) {
	StationDist to;
	to.stn = stn;
	to.distance = dist;
	to.from = 0;
	if (stn.line == NULL) return;
	if (p->len == p->alloclen) {
		StationDist* tmp = realloc(p->stnlist, (p->alloclen + relen) * sizeof(StationDist));
		if (tmp == NULL) {
			free(p->stnlist);
			exit(1);
		}
		p->stnlist = tmp;
		p->alloclen += relen;
	}
	p->stnlist[p->len++] = to;
}

StationSearchRes searchStationByName(const ReadLine map, const char* const name) {
	StationSearchRes result = { 0, };
	Station* tmp;
	char flag = 0;
	for (const Line* p = map.lineinfo; p < map.lineinfo + map.len; p++)
		for (const StationTable* q = p->table; q < p->table + p->tablelen; q++)
			if (!strcmp(name, q->stationName)) {
				flag = 0;
				for (const Station* r = result.stn; r < result.stn + result.len; r++) {
					for (const Station* s = q->transfers; s < q->transfers + q->numOfTransfers; s++)
						if (r->station == s->station) {
							flag = 1;
							break;
						}
					if (flag) break;
				}
				if (flag) continue;


				if (result.stn == NULL) {
					tmp = malloc(sizeof(Station));
					if (tmp == NULL) exit(1);
				}
				else {
					tmp = realloc(result.stn, (result.len + 1) * sizeof(Station));
					if (tmp == NULL) {
						free(result.stn);
						exit(1);
					}
				}
				result.stn = tmp;
				tmp[result.len].line = p;
				tmp[result.len].station = q;
				result.len++;
			}
	return result;
}

ReadLine makeLine(char* readtxt) {
	ReadLine result;
	size_t stnTableAlloclen, lineAlloclen;
	char* stn = readtxt, * stnpt;
	char flag = 1;
	StationTable* stnTable = NULL, * stnTablept = NULL, * q;
	Line* line = calloc(LINELEN, sizeof(Line)), * linept, * p;
	if (line == NULL) exit(1);
	linept = line;
	result.lineinfo = line;
	lineAlloclen = LINELEN;
	strtok(readtxt, "\n");
	stn = strtok(NULL, "\n");
	while (stn) {
		for (stnpt = stn; *stnpt != ','; stnpt++);
		*stnpt++ = '\0';
		if (flag || strcmp(linept->lineName, stn)) {
			if (!flag) {
				linept->tablelen = stnTablept - stnTable;
				linept++;
				if (linept - line == lineAlloclen) {
					//���Ҵ�
					result.lineinfo = realloc(line, (lineAlloclen + RELEN) * sizeof(Line));
					if (result.lineinfo == NULL) {
						while (linept >= line) {
							free(linept->table);
							linept--;
						}
						free(line);
						exit(1);
					}
					else {
						line = result.lineinfo;
						linept = line + lineAlloclen;
						lineAlloclen += RELEN;
					}
				}
			}

			//�뼱�� �ֱ�
			linept->lineName = stn;
			stnTable = calloc(STATIONLEN, sizeof(StationTable));
			if (stnTable == NULL) {
				while (linept > line) {
					linept--;
					free(linept->table);
				}
				free(line);
				exit(1);
			}
			else {
				linept->table = stnTable;
				stnTablept = stnTable;
				stnTableAlloclen = STATIONLEN;
			}
		}
		else if (stnTablept - stnTable == stnTableAlloclen) {
			//���Ҵ�
			linept->table = realloc(stnTable, (stnTableAlloclen + STATIONRELEN) * sizeof(StationTable));
			if (linept->table == NULL) {
				while (linept >= line) {
					free(linept->table);
					linept--;
				}
				free(line);
				exit(1);
			}
			else {
				stnTable = linept->table;
				stnTablept = stnTable + stnTableAlloclen;
				stnTableAlloclen += STATIONRELEN;
			}
		}
		stn = stnpt;

		while (*stnpt != ',') stnpt++;
		stnTablept->stationNum = stn;
		*stnpt++ = '\0';
		stn = stnpt;

		while (*stnpt != ',') stnpt++;
		stnTablept->stationName = stn;
		*stnpt++ = '\0';
		stn = stnpt;

		while (*stnpt != '.'&& *stnpt != ',') stnpt++;
		if (*stnpt == ',') flag = 0;
		else flag = 1;
		
		*stnpt++ = '\0';
		stnTablept->distance = atoi(stn) * 10;
		stn = stnpt;
		if (flag) {
			while (*stnpt != ',') stnpt++;
			*stnpt++ = '\0';
			stnTablept->distance += atoi(stn);
			stn = stnpt;
		}

		stnTablept->numOfTransfers = 0;
		while (*stn != ',' && *stn != '\0') {
			while (*stnpt != ',' && *stnpt != '\0') stnpt++;
			if (*stnpt == ',') *stnpt++ = '\0';
			flag = 0;

			for (p = line; p < linept && !flag; p++)
				for (q = p->table; q < p->table + p->tablelen && !flag; q++)
					if (!strcmp(stn, q->stationNum)) {
						stnTablept->transfers[stnTablept->numOfTransfers].line = p;
						stnTablept->transfers[stnTablept->numOfTransfers].station = q;
						stnTablept->numOfTransfers++;
						q->transfers[q->numOfTransfers].line = linept;
						q->transfers[q->numOfTransfers].station = stnTablept;
						q->numOfTransfers++;
						flag = 1;
					}
			stn = stnpt;
		}
		stnTablept++;
		stn = strtok(NULL, "\n");
		flag = 0;
	}
	linept->tablelen = stnTablept - stnTable;
	result.len = linept - line + 1;
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
		if (alloclen - (pos - str) == 1) { //���Ҵ�
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
		while (*pos) pos++; //'\0' ��ġ�� �ű��
	}
	//������ ������ ��� ����
	fclose(dictfile);

	return str;
}

void freeset(void** p) {
	if (*p != NULL) {
		free(*p);
		*p = NULL;
	}
}