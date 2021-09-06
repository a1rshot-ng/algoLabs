#pragma once

#include <stdlib.h>
#include <string.h>

#include "vector.h"

#ifndef MAX_VECTOR_LENGTH
#define MAX_VECTOR_LENGTH 1024
#endif


Vector* vector()
{
    Vector* empty_vector = (Vector*) malloc(sizeof(Vector));
    empty_vector->max_length = MAX_VECTOR_LENGTH;
    empty_vector->begin = 0;
    empty_vector->end = 0;
    empty_vector->p = (void**) malloc(empty_vector->max_length * sizeof(void**));
    return empty_vector;
}

bool vector_push(Vector* given_vector, void* x)
{
    if ( (given_vector->begin - given_vector->end + given_vector->max_length)
        % given_vector->max_length >= given_vector->max_length - 1) return true;
    given_vector->p[given_vector->begin] = x;
    given_vector->begin++;
    given_vector->begin %= given_vector->max_length;
    return false;
}

bool vector_pushback(Vector* given_vector, void* x)
{
    if ( (given_vector->begin - given_vector->end + given_vector->max_length)
         % given_vector->max_length >= given_vector->max_length - 1) return true;
    given_vector->p[given_vector->end] = x;
    given_vector->end += given_vector->max_length - 1;
    given_vector->end %= given_vector->max_length;
    return true;
}

bool vector_insert(Vector* given_vector, size_t index, void* x)
{
    if ( (given_vector->begin - given_vector->end + given_vector->max_length)
         % given_vector->max_length >= given_vector->max_length - 1) return true;

    if (index >= given_vector->max_length) return true;

    size_t eff_index = (given_vector->begin + given_vector->max_length - index - 1) % given_vector->max_length;

    memmove(given_vector->p + ((eff_index + 1) % given_vector->max_length), given_vector->p + eff_index, given_vector->max_length - eff_index - 1);
    memmove(given_vector->p, given_vector->p + given_vector->max_length - eff_index, eff_index - 1);

    given_vector->p[eff_index] = x;

    given_vector->begin++;
    given_vector->begin %= given_vector->max_length;

    return false;
}

void* vector_extract(Vector* given_vector, size_t index)
{
    void* x;
    if (given_vector->begin == given_vector->end)
        return NULL;

    size_t eff_index = (given_vector->begin + given_vector->max_length - index - 1) % given_vector->max_length;

    x = given_vector->p[eff_index];

    memmove(given_vector->p + eff_index, given_vector->p + ((eff_index + 1) % given_vector->max_length), given_vector->max_length - eff_index - 1);
    memmove(given_vector->p + given_vector->max_length - eff_index, given_vector->p, eff_index - 1);

    given_vector->begin += given_vector->max_length - 1;
    given_vector->begin %= given_vector->max_length;

    return x;
}

void* vector_pop(Vector* given_vector)
{
    void* x;
    if (given_vector->begin == given_vector->end)
        return NULL;

    given_vector->begin += given_vector->max_length - 1;
    given_vector->begin %= given_vector->max_length;

    x = given_vector->p[given_vector->begin];
    return x;
}

void* vector_popback(Vector* given_vector)
{
    void* x;
    if (given_vector->begin == given_vector->end)
        return NULL;

    given_vector->end += 1;
    given_vector->end %= given_vector->max_length;

    x = given_vector->p[given_vector->end];
    return x;
}

void* vector_get(Vector* given_vector, size_t index)
{
    void* x;
    if (index >= given_vector->max_length)
        return NULL;

    return given_vector->p[(given_vector->begin + given_vector->max_length - index - 1) % given_vector->max_length];
}

void vector_delete(Vector* given_vector)
{
    size_t i;
    for (i = (given_vector->begin + given_vector->max_length - 1) % given_vector->max_length;
         i != (given_vector->end + given_vector->max_length - 1) % given_vector->max_length;
         i = (i + given_vector->max_length - 1) % given_vector->max_length)
        free(given_vector->p[i]);

    if (given_vector->p)
        free(given_vector->p);

    free(given_vector);
}

void vector_print(Vector* given_vector, const char* format)
{
    size_t i;
    for (i = (given_vector->begin + given_vector->max_length - 1) % given_vector->max_length;
         i != (given_vector->end + given_vector->max_length - 1) % given_vector->max_length;
         i = (i + given_vector->max_length - 1) % given_vector->max_length)
        printf(format, *((int*)given_vector->p[i]));
}