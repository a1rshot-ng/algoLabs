#pragma once

#include <stdlib.h>
#include <stdbool.h>

#ifdef USE_LIST
#include "list.h"
#else
#include "vector.h"
#endif

typedef struct Stack
{
#ifdef USE_LIST
    List* list_container;
#else
    Vector* vector_container;
#endif
} Stack;


Stack* stack();

bool push(Stack* given_vector, void* x);

void* pop(Stack* given_stack);

void delete(Stack* given_stack);

void print(Stack* given_stack, const char* format);
