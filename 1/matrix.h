/*
 *    структуры данных и прототипы функций для вектора (Array) и матрицы (Matrix)
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Array
{
    size_t length;
    double* p;
} Array;

typedef struct Matrix
{
    size_t height;
    Array* p;
} Matrix;

int  inputMatrix(Matrix* matrix);
void printMatrix(const Matrix* matrix);
void printVector(const Array* b);
