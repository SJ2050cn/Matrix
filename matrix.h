/*--------------------------------------------------------------
* THIS FILE IS PART OF Cimple PROJECT
* matrix.c - The matrix operations library
* THIS PROGRAM IS FREE SOFTWARE, TRYING TO MAKING C MORE SIMPLE.
* Copyright(c) 2019 ShenJian(SJ2050)
----------------------------------------------------------------*/

#pragma once
#ifndef MATRIX_H_
#define MATRIX_H_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <float.h>

// 定义一个链表，每个链表节点指向一行元素
typedef struct tagMatrixRow {
	double* row;
	struct tagMatrixRow* next;
}MatrixRow, *pMatrixRow;

// 定义二维矩阵的储存结构
typedef struct {
	int rows;	//行数
	int columns;	//列数
	pMatrixRow pmatrix;	//指向矩阵的指针,且默认定义数据类型为double
}Matrix, *pMatrix;

// 将所有创建的矩阵连接成链表，方便后面销毁
typedef struct _tagMatrixs {
	pMatrix mx;
	_Bool isImportant;	// 表明该矩阵的重要性，不重要的矩阵会在MatrixsClean函数中被清理
	struct _tagMatrixs *next;
}_Matrixs, *_pMatrixs;

// 函数功能：初始化矩阵
// 函数参数：mxAllRows:待组成的矩阵的所有行，rows:矩阵行数，columns:矩阵列数，isImportant：矩阵是否长久保留直至强制清理矩阵
// 函数返回值：初始化完毕的矩阵
pMatrix MatrixInit(const pMatrixRow* mxAllRows, int rows, int columns, _Bool isImportant);

// 函数功能：拷贝一个矩阵
// 函数参数：要拷贝的矩阵
// 函数返回值：传入的矩阵的拷贝
pMatrix MatrixCopy(const pMatrix mx, _Bool isImportant);

// 函数功能：销毁矩阵(由Clean...函数调用)
// 函数参数：priorDelmxNode:待销毁的矩阵的前一个节点
// 函数返回值：void
void _MatrixDestroy(_pMatrixs* priorDelmxNode);

// 函数功能：打印矩阵
// 函数参数：mx:待打印的矩阵，precision：希望输出矩阵的精度
// 函数返回值：void
void MatrixPrint(const pMatrix mx, int precision);

// 函数功能：通过行列下标获得矩阵元素
// 函数参数：mx:矩阵，row/column:待获得元素的行列下标
// 函数返回值：须获得行列下标的元素
double MatrixGetByIndex(const pMatrix mx, int row, int column);

// 函数功能：通过行列下标修改矩阵元素
// 函数参数：mx:矩阵，row/column:待获得元素的行列下标，num:要修改成的值
// 函数返回值：void
void MatrixSetByIndex(pMatrix mx, int row, int column, double num);

// 函数功能：矩阵转置
// 函数参数：mx：待转置的矩阵，isImportant：矩阵是否长久保留直至强制清理矩阵
// 函数返回值：转置后的矩阵
pMatrix MatrixTranspose(const pMatrix mx, _Bool isImportant);

// 函数功能：矩阵加法
// 函数参数：mx1/mx2:待相加的两个矩阵，isImportant：矩阵是否长久保留直至强制清理矩阵
// 函数返回值：若相加成功，返回相加后的矩阵；否则，返回空指针NULL
pMatrix MatrixAdd(const pMatrix mx1, const pMatrix mx2, _Bool isImportant);

// 函数功能：矩阵减法
// 函数参数：mx1/mx2:待相减的两个矩阵，isImportant：矩阵是否长久保留直至强制清理矩阵
// 函数返回值：若相减成功，返回相减后的矩阵；否则，返回空指针NULL
pMatrix MatrixMinus(const pMatrix mx1, const pMatrix mx2, _Bool isImportant);

// 函数功能：矩阵相乘
// 函数参数：mx1/mx2:待相乘的两个矩阵，isImportant：矩阵是否长久保留直至强制清理矩阵
// 函数返回值：能乘，返回相乘后的结果；否则，返回空指针NULL
pMatrix MatrixMultiply(const pMatrix mx1, const pMatrix mx2, _Bool isImportant);

// 函数功能：矩阵和一个数相乘
// 函数参数：mx：相乘矩阵，num:相乘的数，isImportant：矩阵是否长久保留直至强制清理矩阵
// 函数返回值：相乘后的结果矩阵
pMatrix MatrixMultiplyNum(const pMatrix mx, double num, _Bool isImportant);

// 函数功能：交换矩阵两行
// 函数参数：mx要交换行的矩阵，row1/row2:要交换的行号
// 函数返回值：void
void MatrixSwapRows(pMatrix mx, int row1, int row2);

// 函数功能：将矩阵中的一行乘以一个数加到另一行
// 函数参数：mx:待操作的矩阵，row1:加给其他行的行，num:乘数，row2:被加行
// 函数返回值：void
void MatrixRowMultiAdd(pMatrix mx, int row1, double num, int row2);

// 函数功能：给矩阵中的某一行乘以一个数
// 函数参数：mx:待操作的矩阵，row:被乘行，num:乘数
// 函数返回值：void
void MatrixRowMulti(pMatrix mx, int row, double num);

// 函数功能：创建一个单位矩阵
// 函数参数：rows：单位矩阵行数，isImportant：矩阵是否长久保留直至强制清理矩阵
// 函数返回值：创建的单位阵
pMatrix UnitMatrix(int rows, _Bool isImportant);

// 函数功能：解Ax=b形式的线性方程组（附加功能:求系数矩阵A的行列式）
// 函数参数：A：系数矩阵A，b：右端常数项向量，determinant:返回A的行列式
// 函数返回值：求解得到的x的向量
pMatrix _SolveLinearEquations(const pMatrix A, const pMatrix b, double* determinant);

// 函数功能：解右端项为矩阵的线性方程组，形式为AX=B
// 函数参数：A：系数矩阵A，B：右端常数项矩阵
// 函数返回值：求解得到的X的矩阵
pMatrix SolveLinearEquations(const pMatrix A, const pMatrix B);

// 函数功能：按列选主元素
// 函数参数：mx：操作的矩阵，row：从第几行开始找起，column：要选择的列
// 函数返回值：按列选取的主元素所在的行下标
int ChooseBiggestCol(const pMatrix mx, int row, int column);

// 函数功能：使用赛德尔迭代求解线性方程组
// 函数参数：A:系数矩阵，b:右端常数项向量，initx:初始x的向量，iterPrecision:迭代精度
// 函数返回值：解得x的向量，若不收敛，则返回NULL
pMatrix SeidelMethod(const pMatrix A, const pMatrix b, const pMatrix initx, double iterPrecision);

// 函数功能：求mx的逆矩阵
// 函数参数：mx:待求逆矩阵的矩阵，isImportant：矩阵是否长久保留直至强制清理矩阵
// 函数返回值：待求逆矩阵的逆矩阵
pMatrix MatrixInverse(const pMatrix mx, _Bool isImportant);

// 函数功能：求矩阵的行列式
// 函数参数：mx:待求行列式的矩阵
// 函数返回值：矩阵的行列式
double MatrixDeterminant(const pMatrix mx);

// 函数功能：计算两个矩阵的平方误差
// 函数参数：mx1/mx2：两个待计算的矩阵
// 函数返回值：计算得到的平方误差，如果发生错误则返回-1
double MatrixSquareError(const pMatrix mx1, const pMatrix mx2);

// 函数功能：销毁所有不重要的矩阵
// 函数参数：void
// 函数返回值：void
void MatrixClean();

// 函数功能：销毁所有矩阵
// 函数参数：void
// 函数返回值：void
void MatrixCleanAll();

