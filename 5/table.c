/*
 *      table data structure
 *
 *      types described:
 *          KeyType  -  string (char*)
 *          Table  -  contains max size, busy cells number, and cells vector
 *          Cell  -  contains busy state, key, and data pointer
 *
 *      functions:
 *          hash()
 *          shuffle()
 *          create()
 *
 *      methods:
 *          insert()
 *          rm()
 *          get()
 *          delete()
 */

#include <stdlib.h>
#include <string.h>

#include "table.h"


// -------------------------------------------------- functions ----------------------------------------------------- //

size_t table_hash(const KeyType* key, size_t m)
{
    size_t i = 0, ans = 0;
    KeyType curr = '\0';
    do {
        ans <<= 8u;
        ans += curr;
        ans %= m;
        curr = key[i++];
    } while (curr);
    return ans;
}


size_t table_shuffle(size_t pos, size_t m)
{
    return (pos + PRIME_STEP) % m;
}


Table* table_create(size_t size)
{
    Table* t = malloc(sizeof(Table));
    if (!t) return NULL;

    t->size = size;
    t->num_busy = 0;
    t->p = calloc(size, sizeof(Cell));
    if (!t->p) { free(t); return NULL; }

    return t;
}

// ------------------------------------------------------------------------------------------------------------------ //



// --------------------------------------------------- methods ------------------------------------------------------ //

TableErrorCode table_insert(Table* t, const KeyType* key, void* data)
{
    if (t->num_busy >= t->size)
        return ERROR_TABLE_FULL;

    if (table_get(t, key))
        return ERROR_DUP_KEY;

    size_t pos = table_hash(key, t->size);
    size_t init_pos = pos;
    while (t->p[pos].busy == CELL_BUSY)
    {
        pos = table_shuffle(pos, t->size);
        if (pos == init_pos)
            return ERROR_TABLE_FULL;
    }

    t->p[pos].busy = CELL_BUSY;
    t->p[pos].key = strdup(key);
    t->p[pos].data = data;

    t->num_busy++;
    return SUCCESS;
}


TableErrorCode table_update(Table* t, const KeyType* key, void* data)
{
    size_t pos = table_hash(key, t->size);
    size_t init_pos = pos;
    while (t->p[pos].busy == CELL_BUSY && strcmp(key, t->p[pos].key) != 0)
    {
        pos = table_shuffle(pos, t->size);
        if (pos == init_pos)
            return ERROR_TABLE_FULL;
    }

    if (t->p[pos].busy != CELL_BUSY)
        return ERROR_ELEM_NOT_FOUND;

    t->p[pos].data = data;
    return SUCCESS;
}


TableErrorCode table_rm(Table* t, const KeyType* key, bool free_data)
{
    if (t->num_busy == 0)
        return ERROR_ELEM_NOT_FOUND;

    size_t pos = table_hash(key, t->size);
    size_t init_pos = pos;
    while (t->p[pos].busy == CELL_DELETED || t->p[pos].busy == CELL_BUSY && strcmp(key, t->p[pos].key) != 0)
    {
        pos = table_shuffle(pos, t->size);
        if (pos == init_pos)
            return ERROR_ELEM_NOT_FOUND;
    }

    if (t->p[pos].busy == CELL_FREE)
        return ERROR_ELEM_NOT_FOUND;

    t->p[pos].busy = CELL_DELETED;

    free(t->p[pos].key);
    t->p[pos].key = NULL;

    if (free_data) free(t->p[pos].data);
    t->p[pos].data = NULL;

    t->num_busy--;
    return SUCCESS;
}


void* table_get(Table* t, const KeyType* key)
{
    if (t->num_busy == 0)
        return NULL;

    size_t pos = table_hash(key, t->size);
    size_t init_pos = pos;
    while (t->p[pos].busy == CELL_DELETED || t->p[pos].busy == CELL_BUSY && strcmp(key, t->p[pos].key) != 0)
    {
        pos = table_shuffle(pos, t->size);
        if (pos == init_pos)
            return NULL;
    }

    if (t->p[pos].busy == CELL_FREE)
        return NULL;

    return t->p[pos].data;
}


void table_delete(Table* t, bool free_data)
{
    size_t i;
    for (i = 0; i < t->size; i++)
    {
        if (t->p[i].key) free(t->p[i].key);
        if (free_data) free(t->p[i].data);
    }
    free(t->p);
    free(t);
}

// ------------------------------------------------------------------------------------------------------------------ //
