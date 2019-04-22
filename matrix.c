#include "matrix.h"

// 定义允许的误差
const double ALLOW_ERROR = 1e-17;

// 指向所有创建的矩阵，方便后面同一销毁
_pMatrixs _allMatrixs = NULL;

// 函数功能：初始化矩阵
// 函数参数：mxAllRows:待组成的矩阵的所有行，rows:矩阵行数，columns:矩阵列数，isImportant：矩阵是否长久保留直至强制清理矩阵
// 函数返回值：初始化完毕的矩阵
pMatrix MatrixInit(const pMatrixRow* mxAllRows, int rows, int columns, _Bool isImportant) {
	pMatrix mx;
	_pMatrixs mxNode;
	pMatrixRow fromRow, destRow, priorDestRow, headDestRow;
	int r;

	// 将传入的待组成的矩阵的行拷贝一份分配给矩阵，并用链表串联起来
	if (mxAllRows != NULL) {
		fromRow = mxAllRows[0];
		headDestRow = priorDestRow = (pMatrixRow)malloc(sizeof(MatrixRow));
		priorDestRow->row = (double*)malloc(sizeof(double)*columns);
		memcpy(priorDestRow->row, fromRow->row, sizeof(double)*columns);
		priorDestRow->next = NULL;
		for (r = 1; r < rows; r++) {
			fromRow = mxAllRows[r];
			destRow = (pMatrixRow)malloc(sizeof(MatrixRow));
			destRow->row = (double*)malloc(sizeof(double)*columns);
			memcpy(destRow->row, fromRow->row, sizeof(double)*columns);
			destRow->next = priorDestRow->next;
			priorDestRow->next = destRow;
			priorDestRow = priorDestRow->next;
		}
	}
	else {	//如果传入的行参数为NULL，则返回同等大小的矩阵，但不初始化
		headDestRow = priorDestRow = (pMatrixRow)malloc(sizeof(MatrixRow));
		priorDestRow->row = (double*)malloc(sizeof(double)*columns);
		priorDestRow->next = NULL;
		for (r = 1; r < rows; r++) {
			destRow = (pMatrixRow)malloc(sizeof(MatrixRow));
			destRow->row = (double*)malloc(sizeof(double)*columns);
			destRow->next = priorDestRow->next;
			priorDestRow->next = destRow;
			priorDestRow = priorDestRow->next;
		}
	}

	mx = (pMatrix)malloc(sizeof(Matrix));
	mx->rows = rows;
	mx->columns = columns;
	mx->pmatrix = headDestRow;

	// 采用尾插法将所有创建的矩阵连接起来，方便后面销毁
	if (_allMatrixs == NULL) {
		// 创建头节点
		_allMatrixs = (_pMatrixs)malloc(sizeof(_Matrixs));
		_allMatrixs->next = NULL;
		mxNode = (_pMatrixs)malloc(sizeof(_Matrixs));
		mxNode->mx = mx;
		mxNode->isImportant = isImportant;
		mxNode->next = _allMatrixs->next;
		_allMatrixs->next = mxNode;
	}
	else {
		mxNode = (_pMatrixs)malloc(sizeof(_Matrixs));
		mxNode->mx = mx;
		mxNode->isImportant = isImportant;
		mxNode->next = _allMatrixs->next;
		_allMatrixs->next = mxNode;
	}
	// 返回初始化的矩阵
	return mx;
}

// 函数功能：拷贝一个矩阵
// 函数参数：要拷贝的矩阵
// 函数返回值：传入的矩阵的拷贝
pMatrix MatrixCopy(const pMatrix mx, _Bool isImportant) {
	pMatrix copyMatrix;
	int r, c;

	copyMatrix = MatrixInit(NULL, mx->rows, mx->columns, isImportant);

	for (r = 1; r <= mx->rows; r++) {
		for (c = 1; c <= mx->columns; c++) {
			MatrixSetByIndex(copyMatrix, r, c, MatrixGetByIndex(mx, r, c));
		}
	}

	return copyMatrix;
}

// 函数功能：销毁矩阵(由Clean...函数调用)
// 函数参数：priorDelmxNode:待销毁的矩阵的前一个节点
// 函数返回值：void
void _MatrixDestroy(_pMatrixs* priorDelmxNode) {
	pMatrixRow priorRow, nextRow;
	_pMatrixs delmxNode;

	delmxNode = (*priorDelmxNode)->next;
	(*priorDelmxNode)->next = (*priorDelmxNode)->next->next;

	// 少删除了一行
	// 循环释放指向矩阵每个行的链表指针
	priorRow = delmxNode->mx->pmatrix;
	nextRow = priorRow->next;
	while (priorRow != NULL) {
		free(priorRow->row);
		priorRow->row = NULL;
		free(priorRow);
		priorRow = NULL;
		priorRow = nextRow;
		// 如果矩阵的下一行还有的话
		if (priorRow) {
			nextRow = nextRow->next;
		}
	}

	// 释放矩阵
	free(delmxNode);
	delmxNode = NULL;
}

// 函数功能：打印矩阵
// 函数参数：mx:待打印的矩阵，precision：希望输出矩阵的精度
// 函数返回值：void
void MatrixPrint(const pMatrix mx, int precision) {
	int r, c;
	pMatrixRow eachRow;

	if (mx == NULL) {
		printf("无打印矩阵！\n");
	}

	eachRow = mx->pmatrix;
	for (r = 0; r < mx->rows; r++) {
		for (c = 0; c < mx->columns; c++) {
			if (precision <= 0 && precision >= 77)	precision = 6;	//设置精度允许范围，默认为6
			printf("%.*f\t", precision, eachRow->row[c]);
		}
		printf("\n");
		eachRow = eachRow->next;
	}
}

// 函数功能：通过行列下标获得矩阵元素
// 函数参数：mx:矩阵，row/column:待获得元素的行列下标
// 函数返回值：须获得行列下标的元素
double MatrixGetByIndex(const pMatrix mx, int row, int column) {
	int r;
	pMatrixRow eachRow;

	eachRow = mx->pmatrix;

	for (r = 1; r < row; r++) {
		eachRow = eachRow->next;
	}

	return eachRow->row[column - 1];
}

// 函数功能：通过行列下标修改矩阵元素
// 函数参数：mx:矩阵，row/column:待获得元素的行列下标，num:要修改成的值
// 函数返回值：void
void MatrixSetByIndex(pMatrix mx, int row, int column, double num) {
	int r;
	pMatrixRow eachRow;

	eachRow = mx->pmatrix;

	for (r = 1; r < row; r++) {
		eachRow = eachRow->next;
	}

	eachRow->row[column - 1] = num;
}

// 函数功能：矩阵转置
// 函数参数：mx：待转置的矩阵，isImportant：矩阵是否长久保留直至强制清理矩阵
// 函数返回值：转置后的矩阵
pMatrix MatrixTranspose(const pMatrix mx, _Bool isImportant) {
	int r, c;
	pMatrix mxT;

	// 先创建一个转置后的矩阵
	mxT = MatrixInit(NULL, mx->columns, mx->rows, isImportant);

	// 将原矩阵的元素复制到转置矩阵的对应位置
	for (r = 1; r <= mxT->rows; r++) {
		for (c = 1; c <= mxT->columns; c++) {
			MatrixSetByIndex(mxT, r, c, MatrixGetByIndex(mx, c, r));
		}
	}

	return mxT;
}

// 函数功能：矩阵加法
// 函数参数：mx1/mx2:待相加的两个矩阵，isImportant：矩阵是否长久保留直至强制清理矩阵
// 函数返回值：若相加成功，返回相加后的矩阵；否则，返回空指针NULL
pMatrix MatrixAdd(const pMatrix mx1, const pMatrix mx2, _Bool isImportant) {
	pMatrix resultMatrix;
	int r, c;

	// 两个矩阵的相加得满足行列数相等，否则返回空指针
	if (mx1->rows != mx2->rows || mx1->columns != mx2->columns) {
		return NULL;
	}

	resultMatrix = MatrixInit(NULL, mx1->rows, mx1->columns, isImportant);

	for (r = 1; r <= resultMatrix->rows; r++) {
		for (c = 1; c <= resultMatrix->columns; c++) {
			// 对应位置的元素相加
			MatrixSetByIndex(resultMatrix, r, c, MatrixGetByIndex(mx1, r, c) + MatrixGetByIndex(mx2, r, c));
		}
	}

	return resultMatrix;
}

// 函数功能：矩阵减法
// 函数参数：mx1/mx2:待相减的两个矩阵，isImportant：矩阵是否长久保留直至强制清理矩阵
// 函数返回值：若相减成功，返回相减后的矩阵；否则，返回空指针NULL
pMatrix MatrixMinus(const pMatrix mx1, const pMatrix mx2, _Bool isImportant) {
	pMatrix resultMatrix;
	int r, c;

	// 两个矩阵的相加得满足行列数相等，否则返回空指针
	if (mx1->rows != mx2->rows || mx1->columns != mx2->columns) {
		return NULL;
	}

	resultMatrix = MatrixInit(NULL, mx1->rows, mx1->columns, isImportant);

	for (r = 1; r <= resultMatrix->rows; r++) {
		for (c = 1; c <= resultMatrix->columns; c++) {
			// 对应位置的元素相减
			MatrixSetByIndex(resultMatrix, r, c, MatrixGetByIndex(mx1, r, c) - MatrixGetByIndex(mx2, r, c));
		}
	}

	return resultMatrix;
}

// 函数功能：矩阵相乘
// 函数参数：mx1/mx2:待相乘的两个矩阵，isImportant：矩阵是否长久保留直至强制清理矩阵
// 函数返回值：能乘，返回相乘后的结果；否则，返回空指针NULL
pMatrix MatrixMultiply(const pMatrix mx1, const pMatrix mx2, _Bool isImportant) {
	pMatrix resultMatrix;
	int r, c, k;
	double sum;

	// 两个矩阵是否能相乘，不能乘就返回空指针
	if (mx1->columns != mx2->rows) {
		return NULL;
	}

	resultMatrix = MatrixInit(NULL, mx1->rows, mx2->columns, isImportant);
	// 矩阵相乘
	for (r = 1; r <= mx1->rows; r++) {
		for (c = 1; c <= mx2->columns; c++) {
			for (sum = 0, k = 1; k <= mx1->columns; k++) {
				sum += MatrixGetByIndex(mx1, r, k)*MatrixGetByIndex(mx2, k, c);
			}
			MatrixSetByIndex(resultMatrix, r, c, sum);
		}
	}

	return resultMatrix;
}

// 函数功能：矩阵和一个数相乘
// 函数参数：mx：相乘矩阵，num:相乘的数，isImportant：矩阵是否长久保留直至强制清理矩阵
// 函数返回值：相乘后的结果矩阵
pMatrix MatrixMultiplyNum(const pMatrix mx, double num, _Bool isImportant) {
	pMatrix resultMatrix;
	int r, c;

	resultMatrix = MatrixInit(NULL, mx->rows, mx->columns, isImportant);
	// 矩阵和一个数相乘
	for (r = 1; r <= mx->rows; r++) {
		for (c = 1; c <= mx->columns; c++) {
			MatrixSetByIndex(resultMatrix, r, c, MatrixGetByIndex(mx, r, c)*num);
		}
	}

	return resultMatrix;
}

// 函数功能：交换矩阵两行
// 函数参数：mx要交换行的矩阵指针，row1/row2:要交换的行号
// 函数返回值：void
void MatrixSwapRows(pMatrix mx, int row1, int row2) {
	pMatrixRow priormxRow1, priormxRow2, swapmxRow1, swapmxRow2, tempRow;
	int r, tempNum;

	// 如果要交换的两行是同一行,或要交换的行大于矩阵行数，则直接返回
	if (row1 == row2 || row2 > mx->rows)	return;

	// 我们规定row1要小于row2
	if (row1 > row2) {
		tempNum = row1;
		row1 = row2;
		row2 = tempNum;
	}
	priormxRow1 = priormxRow2 = NULL;

	// 分别找到要交换两行的前一行
	for (r = 1, tempRow = mx->pmatrix; r <= row2; r++, tempRow = tempRow->next) {
		if ((r + 1) == row1) {
			priormxRow1 = tempRow;
			swapmxRow1 = priormxRow1->next;
			continue;
		}
		if ((r + 1) == row2) {
			priormxRow2 = tempRow;
			swapmxRow2 = priormxRow2->next;
			break;
		}
	}

	// 如果row1是第一行，因为其没有前驱节点，我们需要临时创建一个
	if (row1 == 1) {
		priormxRow1 = (pMatrixRow)malloc(sizeof(MatrixRow));
		priormxRow1->next = mx->pmatrix;
		swapmxRow1 = priormxRow1->next;
	}

	// 交换两行的核心
	priormxRow1->next = swapmxRow2;
	tempRow = swapmxRow1->next;

	swapmxRow1->next = swapmxRow2->next;
	swapmxRow2->next = swapmxRow1;

	if (tempRow != swapmxRow2) {
		// 类似头插法正序插入row1和row2之间的行
		swapmxRow2->next = tempRow;
		while (tempRow->next != swapmxRow2) {
			tempRow = tempRow->next;
		}
		tempRow->next = swapmxRow1;
	}

	// 释放临时创建的前驱节点
	if (row1 == 1) {
		mx->pmatrix = priormxRow1->next;
		free(priormxRow1);
		priormxRow1 = NULL;
	}
}

// 函数功能：将矩阵中的一行乘以一个数加到另一行
// 函数参数：mx:待操作的矩阵，row1:加给其他行的行，num:乘数，row2:被加行
// 函数返回值：void
void MatrixRowMultiAdd(pMatrix mx, int row1, double num, int row2) {
	pMatrixRow mxRow1, mxRow2;
	int r, c;

	if (row1 > mx->rows || row2 > mx->rows)	return;

	mxRow1 = mxRow2 = mx->pmatrix;
	for (r = 1; r < row1; r++) {
		mxRow1 = mxRow1->next;
	}
	for (r = 1; r < row2; r++) {
		mxRow2 = mxRow2->next;
	}

	for (c = 0; c < mx->columns; c++) {
		mxRow2->row[c] += mxRow1->row[c] * num;
	}
}

// 函数功能：给矩阵中的某一行乘以一个数
// 函数参数：mx:待操作的矩阵，row:被乘行，num:乘数
// 函数返回值：void
void MatrixRowMulti(pMatrix mx, int row, double num) {
	pMatrixRow mxRow;
	int r, c;

	if (row > mx->rows)	return;

	mxRow = mx->pmatrix;
	for (r = 1; r < row; r++) {
		mxRow = mxRow->next;
	}

	for (c = 0; c < mx->columns; c++) {
		mxRow->row[c] *= num;
	}
}

// 函数功能：创建一个单位矩阵
// 函数参数：rows：单位矩阵行数，isImportant：矩阵是否长久保留直至强制清理矩阵
// 函数返回值：创建的单位阵
pMatrix UnitMatrix(int rows, _Bool isImportant) {
	int r, c;
	pMatrix mx;

	mx = MatrixInit(NULL, rows, rows, isImportant);
	for (r = 1; r <= rows; r++) {
		for (c = 1; c <= rows; c++) {
			// 行列下标相等的元素初始化1，否则初始化为0
			if (r == c) {
				MatrixSetByIndex(mx, r, c, 1);
			}
			else {
				MatrixSetByIndex(mx, r, c, 0);
			}
		}
	}

	return mx;
}

// 函数功能：解Ax=b形式的线性方程组（附加功能:求系数矩阵A的行列式）
// 函数参数：A：系数矩阵A，b：右端常数项向量，determinant:返回A的行列式
// 函数返回值：求解得到的x的向量
pMatrix _SolveLinearEquations(const pMatrix A, const pMatrix b, double* determinant) {
	int r, c;
	int biggestCol;
	double temp;
	pMatrix resultMatrix;
	pMatrix copyA, copyb;

	copyA = MatrixCopy(A, false);
	// 当传入的b矩阵为NULL时，说明只需求A的行列式
	if (b == NULL) {
		for (c = 1; c < copyA->rows; c++) {
			// 找列主元
			biggestCol = ChooseBiggestCol(copyA, c, c);
			if (biggestCol != c) {
				MatrixSwapRows(copyA, c, biggestCol);
			}

			if (fabs(MatrixGetByIndex(copyA, c, c)) < ALLOW_ERROR) {
				// 如果选择的主列元素近似为0，跳过消元，进入下一步迭代
				continue;
			}
			// 消元
			for (r = c + 1; r <= copyA->rows; r++) {
				temp = MatrixGetByIndex(copyA, r, c) / MatrixGetByIndex(copyA, c, c);
				MatrixRowMultiAdd(copyA, c, -temp, r);
			}
		}
		// 如果需要返回行列式的话
		if (determinant != NULL) {
			(*determinant) = 1;
			for (r = 1; r <= A->rows; r++) {
				(*determinant) *= MatrixGetByIndex(copyA, r, r);
			}
		}
		return NULL;
	}

	copyb = MatrixCopy(b, false);

	if (copyA->rows != copyA->columns || copyA->rows != copyb->rows) {
		// 要得到有限解，系数矩阵A的行列数必须相等且A，b矩阵的行数必须相等
		return NULL;
	}
	else if (copyb->columns != 1) {
		//	该函数解的线性方程组的右边常数项为一列的形式
		return NULL;
	}

	// 创建一个矩阵用来储存x的结果
	resultMatrix = MatrixInit(NULL, copyA->rows, 1, false);
	for (c = 1; c < copyA->rows; c++) {
		// 找列主元
		biggestCol = ChooseBiggestCol(copyA, c, c);
		if (biggestCol != c) {
			// 将系数矩阵A1的列主元换到第c行,相应的常数项矩阵b也得跟着变
			MatrixSwapRows(copyA, c, biggestCol);
			MatrixSwapRows(copyb, c, biggestCol);
		}

		if (fabs(MatrixGetByIndex(copyA, c, c)) < ALLOW_ERROR) {
			// 如果选择的主列元素近似为0，跳过消元，进入下一步迭代
			continue;
		}
		// 消元
		for (r = c + 1; r <= copyA->rows; r++) {
			temp = MatrixGetByIndex(copyA, r, c) / MatrixGetByIndex(copyA, c, c);
			MatrixRowMultiAdd(copyA, c, -temp, r);
			MatrixRowMultiAdd(copyb, c, -temp, r);
		}
	}

	// 回代求出x的值
	for (r = resultMatrix->rows; r >= 1; r--) {
		if (fabs(MatrixGetByIndex(copyA, r, r)) < ALLOW_ERROR) {
			// 如果xn对应的ann的系数近似为0，则xn置为0
			MatrixSetByIndex(resultMatrix, r, 1, 0);
			continue;
		}
		else {
			temp = MatrixGetByIndex(copyb, r, 1);
			for (c = r + 1; c <= resultMatrix->rows; c++) {
				temp -= MatrixGetByIndex(copyA, r, c)*MatrixGetByIndex(resultMatrix, c, 1);
			}
			MatrixSetByIndex(copyb, r, 1, temp);
			MatrixSetByIndex(resultMatrix, r, 1, MatrixGetByIndex(copyb, r, 1) / MatrixGetByIndex(copyA, r, r));
		}
	}

	return resultMatrix;
}

// 函数功能：解右端项为矩阵的线性方程组，形式为AX=B
// 函数参数：A：系数矩阵A，B：右端常数项矩阵
// 函数返回值：求解得到的X的矩阵
pMatrix SolveLinearEquations(const pMatrix A, const pMatrix B) {
	int r, c;
	pMatrix resultMatrix;
	pMatrix x, b;

	if (A->rows != A->columns || A->rows != B->rows) {
		// 要得到有限解，系数矩阵A的行列数必须相等且A，B矩阵的行数必须相等
		return NULL;
	}

	// 创建一个矩阵用来储存X的结果
	resultMatrix = MatrixInit(NULL, A->rows, B->columns, true);

	// 分列计算每列的x向量的值
	for (c = 1; c <= resultMatrix->columns; c++) {
		b = MatrixInit(NULL, resultMatrix->rows, 1, false);
		for (r = 1; r <= resultMatrix->rows; r++) {
			MatrixSetByIndex(b, r, 1, MatrixGetByIndex(B, r, c));
		}
		x = _SolveLinearEquations(A, b, NULL);
		for (r = 1; r <= resultMatrix->rows; r++) {
			MatrixSetByIndex(resultMatrix, r, c, MatrixGetByIndex(x, r, 1));
		}
	}

	MatrixClean();
	return resultMatrix;
}

// 函数功能：使用赛德尔迭代求解线性方程组
// 函数参数：A:系数矩阵，b:右端常数项向量，initx:初始x的向量，iterPrecision:迭代精度
// 函数返回值：解得x的向量，若不收敛，则返回NULL
pMatrix SeidelMethod(const pMatrix A, const pMatrix b, const pMatrix initx, double iterPrecision) {
	pMatrix resultMatrix, seidelMatrix;
	double coefficient, sum;
	int r, c;
	double max, priorMax;
	int score = 0;	//得分，如果连续几次算出的结果发散，则函数返回NULL

	if (A->rows != A->columns || A->rows != b->rows) {
		// 要得到有限解，系数矩阵A的行列数必须相等且A，b矩阵的行数必须相等
		return NULL;
	}
	else if (b->columns != 1) {
		//	该函数解的线性方程组的右边常数项为一列的形式
		return NULL;
	}

	priorMax = DBL_MAX;	//初始假设精度为double所能表达的最大值
						// 初始化结果矩阵和赛德尔矩阵
	resultMatrix = MatrixCopy(initx, true);
	seidelMatrix = MatrixInit(NULL, A->rows, A->columns - 1, true);
	for (r = 1; r <= seidelMatrix->rows; r++) {
		coefficient = MatrixGetByIndex(A, r, r);
		for (c = 1; c <= seidelMatrix->columns; c++) {
			// 设置赛德尔矩阵
			MatrixSetByIndex(seidelMatrix, r, c, -MatrixGetByIndex(A, r, c >= r ? c + 1 : c) / coefficient);
		}
		MatrixSetByIndex(b, r, 1, MatrixGetByIndex(b, r, 1) / coefficient);
	}

	// 迭代开始
	while (score >= -5) {
		for (r = 1; r <= seidelMatrix->rows; r++) {
			max = 0;
			sum = MatrixGetByIndex(b, r, 1);
			for (c = 1; c <= seidelMatrix->columns; c++) {
				// 有问题？
				sum += MatrixGetByIndex(seidelMatrix, r, c) * \
					MatrixGetByIndex(resultMatrix, (c >= r ? c + 1 : c), 1);
			}
			// 迭代的结果与上一次的结果做差，得出偏差最大值
			if (fabs(sum - MatrixGetByIndex(resultMatrix, r, 1)) > max) {
				max = fabs(sum - MatrixGetByIndex(resultMatrix, r, 1));
			}
			MatrixSetByIndex(resultMatrix, r, 1, sum);
		}
		if (fabs(max) - fabs(priorMax) < 0) {
			// 说明结果收敛

			if (fabs(fabs(max) - fabs(priorMax)) < iterPrecision) {
				// 达到期望的精度需求时
				break;
			}

			score = 0;
		}
		else {
			// 结果发散时
			score -= 1;
		}
		priorMax = max;
	}

	if (score <= -5) {
		// 说明连续的迭代结果是发散的
		return NULL;
	}

	return resultMatrix;
}

// 函数功能：按列选主元素
// 函数参数：mx：操作的矩阵，row：从第几行开始找起，column：要选择的列
// 函数返回值：按列选取的主元素所在的行下标
int ChooseBiggestCol(const pMatrix mx, int row, int column) {
	int r, biggestRowNum;
	double biggest;

	// 要查找的行数大于矩阵的行数或小于等于0，返回错误
	if (row > mx->rows || row <= 0) return -1;

	// 先假定列主元素所在行就是第row行
	biggestRowNum = row;
	biggest = fabs(MatrixGetByIndex(mx, row, column));

	for (r = row + 1; r <= mx->rows; r++) {
		if (fabs(MatrixGetByIndex(mx, r, column)) > biggest) {
			biggest = fabs(MatrixGetByIndex(mx, r, column));
			biggestRowNum = r;
		}
	}

	return biggestRowNum;
}

// 函数功能：求mx的逆矩阵
// 函数参数：mx:待求逆矩阵的矩阵，isImportant：矩阵是否长久保留直至强制清理矩阵
// 函数返回值：待求逆矩阵的逆矩阵
pMatrix MatrixInverse(const pMatrix mx, _Bool isImportant) {
	// 求逆矩阵的矩阵的行列数必须相等
	if (mx->rows != mx->columns) {
		return NULL;
	}

	return SolveLinearEquations(mx, UnitMatrix(mx->rows, isImportant));
}

// 函数功能：求矩阵的行列式
// 函数参数：mx:待求行列式的矩阵
// 函数返回值：矩阵的行列式
double MatrixDeterminant(const pMatrix mx) {
	double dm;

	_SolveLinearEquations(mx, NULL, &dm);

	return dm;
}

// 函数功能：计算两个矩阵的平方误差
// 函数参数：mx1/mx2：两个待计算的矩阵
// 函数返回值：计算得到的平方误差，如果发生错误则返回-1
double MatrixSquareError(const pMatrix mx1, const pMatrix mx2) {
	double squareError = 0;
	int r, c;

	// 计算的两个矩阵的行列数必须相等
	if (mx1->rows != mx2->rows && mx1->columns != mx2->columns) {
		return -1;
	}

	for (r = 1; r <= mx1->rows; r++) {
		for (c = 1; c <= mx1->columns; c++) {
			squareError += pow(MatrixGetByIndex(mx1, r, c) - MatrixGetByIndex(mx2, r, c), 2);
		}
	}

	return squareError;
}

// 函数功能：销毁所有不重要的矩阵
// 函数参数：void
// 函数返回值：void
void MatrixClean() {
	_pMatrixs mxNode, priorDelmxNode;

	if (_allMatrixs == NULL) {
		return;
	}
	priorDelmxNode = _allMatrixs;
	mxNode = _allMatrixs->next;
	// 遍历矩阵链表销毁所有isImportant为false的矩阵
	while (mxNode != NULL) {
		if (!mxNode->isImportant) {
			_MatrixDestroy(&priorDelmxNode);
			mxNode = priorDelmxNode->next;
			continue;
		}
		mxNode = mxNode->next;
		priorDelmxNode = priorDelmxNode->next;
	}
}

// 函数功能：销毁所有矩阵
// 函数参数：void
// 函数返回值：void
void MatrixCleanAll() {
	if (_allMatrixs == NULL) {
		return;
	}

	// 遍历矩阵链表销毁所有矩阵
	while (_allMatrixs->next != NULL) {
		_MatrixDestroy(&_allMatrixs);
	}
	free(_allMatrixs);
	_allMatrixs = NULL;
}
