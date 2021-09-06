#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct Vector
{
    size_t max_length;
    size_t begin;
    size_t end;
    void** p;
} Vector;


Vector* vector();

bool vector_push(Vector* given_vector, void* x);

bool vector_pushback(Vector* given_vector, void* x);

bool vector_insert(Vector* given_vector, size_t index, void* x);

void* vector_get(Vector* given_vector, size_t index);

void* vector_pop(Vector* given_vector);

void* vector_popback(Vector* given_vector);

void* vector_extract(Vector* given_vector, size_t index);

void vector_delete(Vector* given_vector);

void vector_print(Vector* given_vector, const char* format);
