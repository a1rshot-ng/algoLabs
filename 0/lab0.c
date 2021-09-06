/*
                Лаб 0. Вариант 19.
Дана последовательность из n натуральных чисел.
Для каждого числа исходной последовательности удалить из записи числа все
        вхождения минимальной цифры и подсчитать количество оставшихся цифр.
Сформировать новую последовательность, поместив в нее преобразованные числа
        исходной последовательности в порядке возрастания количества цифр в записи числа.
Вывести исходную и полученную последовательности на экран.
В программе использовать функцию, которая удаляет из записи числа все вхождения
        минимальной цифры и возвращает количество оставшихся цифр.
Например, для числа 116351 результат – 635.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

#define MAX_LENGTH 160

char findMin(const char* arr)
{
    char minc = 0x7f;
    size_t i;
    for (i = 0; arr[i] != '\0'; i++)
        if (arr[i] < minc) minc = arr[i];
    return minc;
}

size_t rmDigit(const char* arr, char digit, char* newarr)
{
    size_t maxlen = strlen(arr);
    size_t iw = 0, ir;
    for (ir = 0; ir < maxlen; ir++)
        if (arr[ir] != digit) newarr[iw++] = arr[ir];
    newarr[iw] = '\0';
    return iw;
}

List* numsInput(const size_t n)
{
    size_t i;
    List* nums = list();
    for (i = 0; i < n; i++)
    {
        char* num = calloc(MAX_LENGTH, sizeof(char));

        char format[16];
        sprintf(format, "%%%ds", MAX_LENGTH);
        scanf(format, num);

        append(nums, num);
    }
    return nums;
}

List* numsProcess(const List* orig)
{
    List* newnums = list();
    Item* i;
    Item* j;
    size_t len, jw;
    for (i = orig->head; i; i = i->next)
    {
        char* newnum = malloc(strlen(i->data));
        len = rmDigit(i->data, findMin(i->data), newnum);

        for (jw = 0, j = newnums->head; j && strlen(j->data) < len; jw++, j = j->next);
        insert(newnums, jw, newnum);
    }

    return newnums;
}

int main()
{
    size_t n;
    Item* i;
    int c;

    printf("Lab 0 [var.19]\n"
         "Reads n and then n numbers, writes n original numbers and n processed (with all min digits removed & sorted by length).\n"
         "Numbers count: ");

    while ((c = scanf("%lu", &n)) == 0)
    {
        puts("Input error. Try again.");
        scanf("%*s");
    }
    if (c == EOF)
    {
        puts("EOF error while reading. Restart the program and try again.");
        return EXIT_FAILURE;
    }


    printf("Enter %lu numbers: ", n);
    List* nums = numsInput(n);
    List* newnums = numsProcess(nums);

    puts("Original sequence:");
    for (i = nums->head; i; i = i->next)
        printf("%s ", i->data);
    puts("");

    puts("Processed sequence:");
    for (i = newnums->head; i; i = i->next)
        printf("%s ", i->data);
    puts("");

    delete(nums);
    delete(newnums);
}
