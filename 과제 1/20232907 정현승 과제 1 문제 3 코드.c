#include "20232907 정현승 프로그래밍 과제 3 공통 헤더.h"

#define POLYNOMIAL_ALLOCLEN 3

typedef struct _monomial {
	double coefficient;
	int degree;
} Monomial;

typedef struct _polynomial_nonzeroonly {
	Monomial *polynomial;
	size_t nomiallen;
	size_t alloclen;
} Polynomial;

#define EMPTY_POLYNOMIAL (Monomial*) 0xFF //참조할 일은 없음, 에러코드를 담을 데가 없어서 쓰는 것

void print_polynomial(Polynomial obj, FILE* _Stream);
Polynomial add_polynomial(Polynomial obj1, Polynomial obj2);
Polynomial multiply_polynomial(Polynomial obj1, Polynomial obj2);
Polynomial append_polynomial(Polynomial obj, int relen, double coefficient, int degree);
Polynomial realloc_Polynomial(Polynomial obj, size_t len);
Polynomial polynomial_simplify(Polynomial obj);
int polynomial_sortbydegree(Monomial* p1, Monomial* p2);
void free_polynomial(Polynomial* obj, size_t len);

void main() {
	char* in = NULL;
	char* tmp[2] = { 0, };
	Polynomial polymonial[4] = { {0,} };
	Polynomial ptmp = { 0, };
	//순서대로 다항식 1, 다항식 2, 덧셈 결과, 뺄셈 결과

	for (Polynomial* p = polymonial; p < polymonial + 2; p++) {
		printf("%d번 다항식: ", p - polymonial + 1);
		in = sgets(14, 3, stdin);
		if (in == NULL) {
			free_polynomial(polymonial, sizeof(polymonial) / sizeof(polymonial[0]));
			return;
		}
		tmp[0] = strtok(in, " \t\n/^Xx");
		tmp[1] = strtok(NULL, " \t\n/^Xx");
		while (tmp[0] != NULL) {
			if (atof(tmp[0]) != 0.0) {
				ptmp = append_polynomial(*p, POLYNOMIAL_ALLOCLEN,
					atof(tmp[0]), tmp[1] != NULL ? atoi(tmp[1]) : 0);
				if (ptmp.polynomial == NULL) {
					free_polynomial(polymonial, sizeof(polymonial) / sizeof(polymonial[0]));
					return;
				}
				else *p = ptmp;
			}
			tmp[0] = strtok(NULL, " \t\n/^Xx");
			tmp[1] = strtok(NULL, " \t\n/^Xx");
		}
		free(in);

		//정리
		if (p->nomiallen == 0) {
			p->polynomial = EMPTY_POLYNOMIAL;
		}
		else {
			ptmp = polynomial_simplify(*p);
			if (ptmp.polynomial == NULL) {
				free_polynomial(polymonial, sizeof(polymonial) / sizeof(polymonial[0]));
				return;
			}
			else *p = ptmp;
		}
	}


	polymonial[2] = add_polynomial(polymonial[0], polymonial[1]);
	polymonial[3] = multiply_polynomial(polymonial[0], polymonial[1]);
	if (polymonial[2].polynomial == NULL || polymonial[3].polynomial == NULL) {
		free_polynomial(polymonial, sizeof(polymonial) / sizeof(polymonial[0]));
		return;
	}

	printf("1 + 2 = ");
	print_polynomial(polymonial[2], stdout);

	printf("1 * 2 = ");
	print_polynomial(polymonial[3], stdout);

	free_polynomial(polymonial, sizeof(polymonial) / sizeof(polymonial[0]));
}

void print_polynomial(Polynomial obj, FILE* _Stream) {
	if (obj.alloclen == 0 || obj.nomiallen == 0) {
		fputs("0\n", _Stream);
		return;
	}
	else {
		fprintf(_Stream, "%.0lf", obj.polynomial[0].coefficient);
		if (obj.polynomial[0].degree != 0) //상수일 때를 대비해 분리
			fprintf(_Stream, "x^%d", obj.polynomial[0].degree);
		for (size_t i = 1; i < obj.nomiallen; i++) { //'+'를 제대로 붙이기 위해 분리
			fprintf(_Stream, "%+.0lf", obj.polynomial[i].coefficient);
			if (obj.polynomial[i].degree != 0)
				fprintf(_Stream, "x^%d", obj.polynomial[i].degree);
		}
	}
	fputc('\n', _Stream); //줄변경용
}

Polynomial add_polynomial(Polynomial obj1, Polynomial obj2) {
	Polynomial res = { 0, }, tmp;
	size_t p1 = 0, p2 = 0;
	while (p1 < obj1.nomiallen && p2 < obj2.nomiallen) {
		if (obj1.polynomial[p1].degree > obj2.polynomial[p2].degree) {
			tmp = append_polynomial(res, POLYNOMIAL_ALLOCLEN, obj1.polynomial[p1].coefficient, obj1.polynomial[p1].degree);
			p1++;
		}
		else if (obj1.polynomial[p1].degree < obj2.polynomial[p2].degree) {
			tmp = append_polynomial(res, POLYNOMIAL_ALLOCLEN, obj2.polynomial[p2].coefficient, obj2.polynomial[p2].degree);
			p2++;
		}
		else if (obj1.polynomial[p1].coefficient + obj2.polynomial[p2].coefficient != 0.0){
				tmp = append_polynomial(res, POLYNOMIAL_ALLOCLEN,
					obj1.polynomial[p1].coefficient + obj2.polynomial[p2].coefficient, obj1.polynomial[p1].degree);
			p1++;
			p2++;
		}
		else {
			p1++;
			p2++;
			continue; //아래 tmp 충돌 방지
		}
		
		if (tmp.polynomial != NULL) res = tmp;
		else {
			free_polynomial(&res, 1);
			return res;
		}
	}
	while (p1 < obj1.nomiallen) {
		tmp = append_polynomial(res, POLYNOMIAL_ALLOCLEN, obj1.polynomial[p1].coefficient, obj1.polynomial[p1].degree);
		p1++;

		if (tmp.polynomial != NULL) res = tmp;
		else {
			free_polynomial(&res, 1);
			return res;
		}
	}
	while (p2 < obj2.nomiallen) {
		tmp = append_polynomial(res, POLYNOMIAL_ALLOCLEN, obj2.polynomial[p2].coefficient, obj2.polynomial[p2].degree);
		p2++;

		if (tmp.polynomial != NULL) res = tmp;
		else {
			free_polynomial(&res, 1);
			return res;
		}
	}
	if (res.nomiallen == 0)
		res.polynomial = EMPTY_POLYNOMIAL;
	return res;
}

Polynomial multiply_polynomial(Polynomial obj1, Polynomial obj2) {
	Polynomial res = { 0, }, tmp;
	size_t p1 = 0, p2 = 0;
	
	//죄다 쓰고 정렬하는 방식(O(n^2) + 1/2 * O(n^2) + O(nlogn))
	for (p1 = 0; p1 < obj1.nomiallen; p1++) {
		for (p2 = 0; p2 < obj2.nomiallen; p2++) {
			tmp = append_polynomial(res, POLYNOMIAL_ALLOCLEN,
				obj1.polynomial[p1].coefficient * obj2.polynomial[p2].coefficient, obj1.polynomial[p1].degree + obj2.polynomial[p2].degree);

			if (tmp.polynomial != NULL) res = tmp;
			else {
				free_polynomial(&res, 1);
				return res;
			}
		}
	}
	if (res.nomiallen == 0) {
		res.polynomial = EMPTY_POLYNOMIAL;
		return res;
	}
	else {
		tmp = polynomial_simplify(res);
		if (tmp.polynomial == NULL) free_polynomial(&res, 1);
		return tmp;
	}
}

Polynomial append_polynomial(Polynomial obj, int relen, double coefficient, int degree) {
	if (obj.alloclen == 0) {
		obj.nomiallen = 0;
		obj.polynomial = calloc_in(relen * sizeof(Monomial));
		if (obj.polynomial == NULL) {
			obj.alloclen = 0;
			return obj;
		}
		else {
			obj.alloclen = relen;
		}
	}
	else if (obj.alloclen == obj.nomiallen) {
		obj = realloc_Polynomial(obj, obj.alloclen + relen);
		if (obj.polynomial == NULL)
			return obj;
		//원래 함수에서 포인터를 가지고 있으니 여기서 free할 필요는 없음
	}
	(obj.polynomial)[obj.nomiallen].coefficient = coefficient;
	(obj.polynomial)[obj.nomiallen++].degree = degree;
	return obj;
}

Polynomial realloc_Polynomial(Polynomial obj, size_t len) {
	Polynomial res = { 0, };
	res.polynomial = realloc_in(obj.polynomial, len * sizeof(Monomial));
	if (res.polynomial != NULL) {
		res.alloclen = len;
		res.nomiallen = obj.nomiallen;
	}
	return res;
}

Polynomial polynomial_simplify(Polynomial obj) {
	size_t resize = obj.nomiallen;
	for (size_t p1 = 0; p1 < obj.nomiallen; p1++) {
		if (obj.polynomial[p1].coefficient == 0.0) {
			resize--;
			continue;
		}
		for (size_t p2 = p1 + 1; p2 < obj.nomiallen; p2++) {
			if (obj.polynomial[p2].coefficient == 0.0) continue;
			else if (obj.polynomial[p1].degree == obj.polynomial[p2].degree) {
				obj.polynomial[p1].coefficient += obj.polynomial[p2].coefficient;
				obj.polynomial[p2].coefficient = 0.0; //항 지우기
				obj.polynomial[p2].degree = 0; //항 지우기
			}
		}
		if (obj.polynomial[p1].coefficient == 0.0) //이 항도 같이 지워졌으면
			resize--;
	}
	qsort(obj.polynomial, obj.nomiallen, sizeof(Monomial), polynomial_sortbydegree);
	obj.nomiallen = resize;
	//realloc은 생략
	return obj;
}

int polynomial_sortbydegree(Monomial* p1, Monomial* p2) {
	if (p1->coefficient == 0.0) return 1;
	else if (p2->coefficient == 0.0) return -1;
	else return p2->degree - p1->degree;
}

void free_polynomial(Polynomial* obj, size_t len) {
	for (Polynomial* p = obj; p < obj + len; p++) {
		if (p->polynomial == EMPTY_POLYNOMIAL) p->polynomial = NULL;
		else freeset(&p->polynomial);
		p->alloclen = 0;
		p->nomiallen = 0;
	}
}