/*
 *      Lab 2 stack data structure (list-based and vector-based)
 *
 *      types described:
 *          Item - pointer to data, pointer to next item
 *          List - length, pointer to first item (NULL if empty)
 *
 *      data type is (void*)
 *
 *      functions:
 *          stack()  returns an empty stack
 *
 *      methods:
 *          pop()
 *          push()
 *          delete()
 */

#pragma once

#include <stdlib.h>
#include <stdbool.h>

#include "stack.h"


Stack* stack()
{
    Stack* empty_stack = (Stack*) malloc(sizeof(Stack));
#ifdef USE_LIST
    empty_stack->list_container = list();
#else
    empty_stack->vector_container = vector();
#endif
    return empty_stack;
}

bool push(Stack* given_stack, void* x)
{
#ifdef USE_LIST
    list_push(given_stack->list_container, x);
    return false;
#else
    return vector_push(given_stack->vector_container, x);
#endif
}

void* pop(Stack* given_stack)
{
#ifdef USE_LIST
    return list_pop(given_stack->list_container, 0);
#else
    return vector_pop(given_stack->vector_container);
#endif
}

void delete(Stack* given_stack)
{
#ifdef USE_LIST
    list_delete(given_stack->list_container);
#else
    vector_delete(given_stack->vector_container);
#endif
    free(given_stack);
}

void print(Stack* given_stack, const char* format)
{
#ifdef USE_LIST
    list_print(given_stack->list_container, format);
#else
    vector_print(given_stack->vector_container, format);
#endif
}