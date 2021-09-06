/*
 *     реализация операций ввода-вывода с векторами и матрицами
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>

#include "matrix.h"
#include "input.h"

int inputMatrix(Matrix* matrix)
{
    int i, j;
    int n, m;

    printf("Enter number of rows: ");
    if (input("%d", &m)) return 1;
    if (m < 0) m = 0;

    matrix->p = calloc(m, sizeof(Array));
    matrix->height = m;

    for (i = 0; i < m; i++)
    {
        printf("Row %d length: ", i + 1);
        if (input("%d", &n)) return 1;
        if (n < 0) n = 0;
        matrix->p[i].length = n;
        matrix->p[i].p = calloc(n, sizeof(double));

        printf("Enter %d numbers: ", n);
        for (j = 0; j < n; j++)
            if (input("%lf", &matrix->p[i].p[j])) return 1;
    }

    return 0;
}

void printMatrix(const Matrix* matrix)
{
    int i, j, m;

    m = matrix->height;
    puts("");
    puts("Original matrix:");
    for (i = 0; i < m; i++) {
        printf("[ ");
        for (j = 0; j < matrix->p[i].length; j++)
            printf("%08lf ", matrix->p[i].p[j]);
        puts("]");
    }
}

void printVector(const Array* b)
{
    int i, m = b->length;
    printf("Vector {b_i}:\n{ ");
    for (i = 0; i < m; i++)
        printf("%08lf ", b->p[i]);
    puts("}");
}