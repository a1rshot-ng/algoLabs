/*
        Лаб 1. Вариант 16.
 Из входного потока вводится непрямоугольная матрица вещественных чисел [aij], i = 1, …, m, j = 1, …, n.
 Значения m и n заранее не известны и вводятся из входного потока.
 Сформировать вектор {bi}, i = 1, …, m, i-ый элемент которого равен сумме тех не нулевых элементов i-ой строки матрицы,
которые превышают соответствующий элемент предыдущей (i-1-ой) строки. Для первой строки матрицы в качестве
предыдущей использовать последнюю строку матрицы.
 Исходную матрицу и полученный вектор вывести в выходной поток с необходимыми комментариями.
 */

#include <stdio.h>
#include <stdlib.h>

#include "matrix.h"

void getVectorB(const Matrix* matrix, Array* b)
{
    int i, j, m;
    double curr, prev;

    m = matrix->height;
    b->length = m;
    b->p = calloc(m, sizeof(double));

    for (i = 0; i < m; i++)
    {
        b->p[i] = 0;
        for (j = 0; j < matrix->p[i].length; j++)
        {
            prev = matrix->p[(i - 1 + m) % m].length > j
                    ?  matrix->p[(i - 1 + m) % m].p[j]
                    :  0.0;
            curr = matrix->p[i].p[j];
            if (curr > prev) b->p[i] += curr;
        }
    }

}

int main() {
    puts("Lab 1 [var.16]");
    puts("Reads non-rectangular martix A[m][n_i], then generates vector of sums s_i: s_i_j > A[i-1][j]");

    Matrix matrix;
    Array b;
    int i;

    if (inputMatrix(&matrix)) return EXIT_FAILURE;
    getVectorB(&matrix, &b);

    printMatrix(&matrix);
    printVector(&b);

    for (i = 0; i < matrix.height; i++)
        free(matrix.p[i].p);
    free(matrix.p);
    free(b.p);

    return 0;
}
