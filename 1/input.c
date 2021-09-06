/*
 *     реализация безопасного ввода пользователя / из файла
 */

#pragma once

#include <stdio.h>

#include "input.h"

int input(const char* format, void* ptr)
{
    int c;
    while (!(c = scanf(format, ptr)))
    {
        puts("Input error. Try again.");
        scanf("%*s");
    }
    if (c == EOF)
    {
        puts("EOF error while reading. Restart the program and try again.");
        return 1;
    }
    return 0;
}

