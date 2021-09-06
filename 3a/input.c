/*
 *    safe scanf / input implementation
 */

#pragma once

#include <stdio.h>

#include "input.h"

#ifndef SUCCESS
 #define SUCCESS 0
#endif

#ifndef ERROR_SYNTAX
 #define ERROR_SYNTAX 1
#endif

int input(const char* format, void* ptr)
{
    int c;
    if (!(c = scanf(format, ptr)))
        return ERROR_SYNTAX;

    if (c == EOF)
    {
        // puts("EOF Error: got EOF while reading. Restart the program and try again.");
        return EOF;
    }
    return SUCCESS;
}

