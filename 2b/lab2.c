#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "stack.h"

#ifndef MAX_LEN
#define MAX_LEN 1024
#endif

#define OVERFLOW (char*) 1

bool isOperator(const char x)
{
    switch (x)
    {
        case '+':
        case '-':
        case '*':
        case '/':
            return true;
        default:
            return false;
    }
}

bool isOperand(const char x)
{
    return (x >= 'a' && x <= 'z');
}

char* prefix2infix(const char* source)
{
    int i;
    char* a, * b;
    char* node;
    char* str;

    bool overflow = false;
    int length = strlen(source);
    Stack* formula = stack();

    for (i = length-1; i >= 0; i--)
    {
        node = malloc(sizeof(char) * 2);
        *node = source[i];
        *(node+1) = '\0';

        if (isOperator(*node))
        {
            a = pop(formula);
            b = pop(formula);

            if (!a || !b)
            {
                if (a) free(a);
                if (b) free(b);
                free(node);
                delete(formula);
                return NULL;
            }

            str = malloc(1 + strlen(a) + 1 + strlen(b) + 1 + 1);  // ( a * b ) \0
            sprintf(str, "(%s%c%s)", a, *node, b);
            if (overflow = push(formula, str))
                free(str);

            free(a);
            free(b);
            free(node);
        }
        else if (isOperand(*node))
        {
            if (overflow = push(formula, node))
                free(node);
        }
        else free(node);

        if (overflow)
        {
            delete(formula);
            return OVERFLOW;
        }
    }

    str = pop(formula);
    delete(formula);

    return str;
}

int main()
{
    char* src = malloc(MAX_LEN);
    puts("\tLab 2\nTransforms a prefix formula to the corresponding infix.\nOperators: [+-*/]\tOperands: [a-z]");
    printf(">>>  ");
    fgets(src, MAX_LEN-1, stdin);

    char* ans = prefix2infix(src);
    if (!ans)
        puts("Invalid formula. Check your syntax and try again.");
    else if (ans == OVERFLOW)
        puts("The formula is too bracing-deep. This version of program is not capable of processing it.");
    else
    {
        puts(ans);
        free(ans);
    }

    free(src);
    return 0;
}

