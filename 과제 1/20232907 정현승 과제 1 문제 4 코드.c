#include "20232907 정현승 프로그래밍 과제 3 공통 헤더.h"

typedef struct _matrix {
	unsigned int row;
	unsigned int column;
	int value;
} SingleMatrix;

typedef struct _sparse_matrix {
	SingleMatrix* matrix;
	unsigned int row; //배를 젓는다: 가로
	unsigned int column; //기둥: 세로
	size_t vallen;
	size_t alloclen;
} SparseMatrix;

#define MAXVALDIG 3 //value의 최대 자리수
#define MATRIX_ALLOCLEN 8
#define NOT_CALCULATE_AVALIABLE (SingleMatrix*) 0x7F //참조할 일은 없음, 에러코드를 담을 데가 없어서 쓰는 것
#define EMPTY_MATRIX (SingleMatrix*) 0xFF

void print_matrix(const SparseMatrix const obj, const FILE* const _Stream);
SparseMatrix sparse_matrix_mult(const SparseMatrix obj1, SparseMatrix obj2);
SparseMatrix append_matrix(SparseMatrix obj, const size_t relen, const SingleMatrix value);
SparseMatrix matrix_transpose(SparseMatrix obj);
SparseMatrix matrix_simplify(SparseMatrix obj);
int matrix_sortbycolumn(const SingleMatrix* const p1, const SingleMatrix* const p2);
int matrix_sortbyrow(const SingleMatrix* const p1, const SingleMatrix* const p2);
void free_matrix(SparseMatrix* obj, const size_t len);

void main() {
	char* in = NULL;
	SingleMatrix tmp = { 0, };
	SparseMatrix matrix[4] = { {0,} };
	SparseMatrix ptmp = { 0, };
	//순서대로 다항식 1, 다항식 2, 덧셈 결과, 뺄셈 결과

	for (int i = 0; i < 2; i++) {

		printf("Input of matrix %d: total rows and columns? ", i + 1);
		scanf("%d%d", &matrix[i].row, &matrix[i].column);
		puts("Input Sparse Matrix with only nonzero values. Input ends when you write 0 at value.");
		printf("column, row, value? ");
		scanf("%u%u%d", &tmp.column, &tmp.row, &tmp.value);
		while (tmp.value!=0) {
			tmp.row--;
			tmp.column--; //배열의 index는 0부터 시작하기 때문에 처리함
			if (tmp.row < matrix[i].row && tmp.column < matrix[i].column) {
				ptmp = append_matrix(matrix[i], MATRIX_ALLOCLEN, tmp);
				if (ptmp.matrix == NULL) {
					free_matrix(matrix, sizeof(matrix) / sizeof(matrix[0]));
					return;
				}
				else matrix[i] = ptmp;
			}
			else puts("Not correct row and column! please write again.");
			printf("column, row, value? ");
			scanf("%u%u%d", &tmp.column, &tmp.row, &tmp.value);
		}

		if (matrix[i].vallen == 0) {
			matrix[i].matrix = EMPTY_MATRIX;
		}

		//정리, 정리가 된 상태로 들어온다 가정하면 필요 없는 코드
		ptmp = matrix_simplify(matrix[i]);
		if (ptmp.matrix == NULL) {
			free_matrix(matrix, sizeof(matrix) / sizeof(matrix[0]));
			return;
		}
		else matrix[i] = ptmp;
	}
	matrix[2] = sparse_matrix_mult(matrix[0], matrix[1]);
	matrix[3] = sparse_matrix_mult(matrix[1], matrix[0]);
	if (matrix[2].matrix == NULL|| matrix[3].matrix == NULL)
		free_matrix(matrix, sizeof(matrix) / sizeof(matrix[0]));
	if (matrix[2].matrix == NOT_CALCULATE_AVALIABLE)
		puts("matrix1*matrix2 isn't availble");
	else {
		puts("matrix1*matrix2:");
		print_matrix(matrix[2], stdout);
	}

	if (matrix[3].matrix == NOT_CALCULATE_AVALIABLE)
		puts("matrix2*matrix1 isn't availble");
	else {
		puts("matrix2*matrix1:");
		print_matrix(matrix[3], stdout);
	}

	free_matrix(matrix, sizeof(matrix) / sizeof(matrix[0]));
}

void print_matrix(const SparseMatrix const obj, const FILE* const _Stream) {
	SingleMatrix* p = obj.matrix;
	for (int column = 0; column < obj.column; column++) {
		for (int row = 0; row < obj.row; row++) {
			if (p < obj.matrix + obj.vallen //EMPTY_MATRIX인 경우 여기서 걸러짐(vallen==0)
				&& p->row == row && p->column == column)
				fprintf(_Stream, "%*d ", (MAXVALDIG)+1, (p++)->value);
			else
				fprintf(_Stream, "%*d ", (MAXVALDIG)+1, 0);
		}
		fputc('\n', _Stream);
	}
	/*
	for(SingleMatrix* p = obj.matrix;p<obj.matrix+obj.vallen;p++)
		fprintf(_Stream,"%*d %*d %*d\n",(MAXVALDIG)+1, p->row,(MAXVALDIG)+1, p->column,(MAXVALDIG)+1, p->value);
	*/
}

SparseMatrix sparse_matrix_mult(const SparseMatrix obj1, SparseMatrix obj2) {
	SparseMatrix res = { 0, }, ptmp;
	SingleMatrix* po1s = obj1.matrix, * po1d = obj1.matrix, * po2, * po1;
	SingleMatrix tmpres = { 0, };
	if (obj1.row != obj2.column) {
		// throw IllegalStateException("multiply of these two matrix isn't availble");
		res.matrix = NOT_CALCULATE_AVALIABLE;
		return res; //할당 오류시와 return값이 같음
	}
	obj2 = matrix_transpose(obj2);
	if (obj2.matrix == NULL) return res;
	//(obj1)의 i번째 줄과 (obj2)의 j번째 줄의 각각의 요소를 곱해서 더하면 res의 i번째 row j번째 column이 됨

	res.row = obj2.column; //transpose하기 전 row
	res.column = obj1.column;

	while (po1s < obj1.matrix + obj1.vallen) {
		while (po1d < obj1.matrix + obj1.vallen && po1d->column == po1s->column) po1d++;

		for (po2 = obj2.matrix; po2 < obj2.matrix + obj2.vallen; po2++) {
			for (po1 = po1s; po1 < po1d && po1->row != po2->row; po1++);

			if (po1 < po1d) {
				if (tmpres.column != po1->column || tmpres.row != po2->column) {
					if (tmpres.value != 0) {
						ptmp = append_matrix(res, MATRIX_ALLOCLEN, tmpres);
						if (ptmp.matrix == NULL) {
							free_matrix(&res, 1);
							return res;
						}
						else res = ptmp;
						tmpres.value = 0;
					}
					tmpres.row = po2->column;
					tmpres.column = po1->column;
				}
				tmpres.value += po1->value * po2->value;
			}
		}
		po1s = po1d;
	}
	free_matrix(&obj2, 1); //transpose하느라 받은 할당 해제
	if (tmpres.value != 0) {
		ptmp = append_matrix(res, MATRIX_ALLOCLEN, tmpres);
		if (ptmp.matrix == NULL) {
			free_matrix(&res, 1);
			return res;
		}
		else return ptmp;
	}
	else if (res.vallen == 0)
		res.matrix = EMPTY_MATRIX;
	return res;
}
/*
		if (po1->row > po2->row) po2++;
		else if (po1->row < po2->row)po1++;
		else if (po1->column > po2->column) po2++;
		else if (po1->column < po2->column) po1++;
		else {

		}*/

SparseMatrix append_matrix(SparseMatrix obj, const size_t relen, const SingleMatrix value) {
	if (obj.alloclen == 0) {
		obj.vallen = 0;
		obj.matrix = calloc_in(relen * sizeof(SingleMatrix));
		if (obj.matrix == NULL) {
			obj.alloclen = 0;
			return obj;
		}
		else {
			obj.alloclen = relen;
		}
	}
	else if (obj.alloclen == obj.vallen) {
		obj.matrix = realloc_in(obj.matrix, (obj.alloclen + relen) * sizeof(SingleMatrix));
		if (obj.matrix != NULL) {
			obj.alloclen += relen;
			obj.matrix = obj.matrix;
		}
		else return obj;
		//원래 함수에서 포인터를 가지고 있으니 여기서 free할 필요는 없음
	}
	(obj.matrix)[obj.vallen++] = value;
	return obj;
}

SparseMatrix matrix_transpose(SparseMatrix obj) {
	SingleMatrix* pr = NULL;
	SparseMatrix res = { 0, };
	/*
	res.matrix=calloc_in(obj.vallen*sizeof(SingleMatrix));
	if(res.matrix==NULL) {
		return res;
	}
	qsort(res.matrix, res.vallen, sizeof(SingleMatrix), matrix_sortbyrow);
	return res;
	*/
	if (obj.vallen == 0) { //0인 항밖에 없는 matrix인 경우
		res = obj;
		res.row = obj.column;
		res.column = obj.row;
		return res;
	}
	pr = calloc_in(obj.vallen * sizeof(SingleMatrix));
	if (pr == NULL) return res;
	res.row = obj.column;
	res.column = obj.row;
	res.matrix = pr;
	res.alloclen = res.vallen = obj.vallen;
	for (int column = 0; column < res.column; column++) {
		for (SingleMatrix* po = obj.matrix; po < obj.matrix + obj.vallen; po++) {
			if (po->row == column) {
				pr->row = po->column;
				pr->column = po->row;
				pr++->value = po->value;
			}
		}
	}
	return res;
}

SparseMatrix matrix_simplify(SparseMatrix obj) {
	const SingleMatrix nonematrix = { 0,0,0 };
	size_t resize = obj.vallen;
	for (size_t p1 = 0; p1 < obj.vallen; p1++) {
		if (obj.matrix[p1].value == 0) {
			resize--;
			continue;
		}
		for (size_t p2 = p1 + 1; p2 < obj.vallen; p2++) {
			if (obj.matrix[p2].value == 0.0) continue;
			else if (obj.matrix[p1].row == obj.matrix[p2].row && obj.matrix[p1].column == obj.matrix[p2].column) {
				obj.matrix[p1].value += obj.matrix[p2].value;
				obj.matrix[p2] = nonematrix; //항 지우기
			}
		}
		if (obj.matrix[p1].value == 0.0) //이 항도 같이 지워졌으면
			resize--;
	}
	qsort(obj.matrix, obj.vallen, sizeof(SingleMatrix), matrix_sortbycolumn);
	obj.vallen = resize;
	return obj;
}

int matrix_sortbycolumn(const SingleMatrix* const p1, const SingleMatrix* const p2) {
	if (p1->value == 0.0) return 1;
	else if (p2->value == 0.0) return -1;
	else if (p1->column == p2->column) return p1->row - p2->row;
	else return p1->column - p2->column;
	//숫자 작은 게 앞으로
}

int matrix_sortbyrow(const SingleMatrix* const p1, const SingleMatrix* const p2) {
	if (p1->value == 0.0) return 1;
	else if (p2->value == 0.0) return -1;
	else if (p1->row == p2->row) return p1->column - p2->column;
	else return p1->row - p2->row;
}

void free_matrix(SparseMatrix* obj, const size_t len) {
	for (SparseMatrix* p = obj; p < obj + len; p++) {
		if (p->matrix == NOT_CALCULATE_AVALIABLE || p->matrix == EMPTY_MATRIX) p->matrix = NULL;
		else freeset(&p->matrix);
		p->alloclen = 0;
		p->vallen = 0;
		p->row = 0;
		p->column = 0;
	}
}

/*
2 4
3 1 2
2 2 1
3 2 3
3 3 0

3 2
2 3 9
2 2 4
1 3 7
1 1 0


6 6
1 4 7
2 1 9
2 6 8
4 1 6
4 2 5
5 6 1
6 3 2
7 7 0
6 6
1 4 12
2 4 6
2 6 4
4 1 2
4 2 10
5 6 16
6 3 8
7 7 0
*/