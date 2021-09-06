/*
 *      table structs and methods
 */

#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


#ifndef PRIME_STEP
#define PRIME_STEP              17
#endif

#define SUCCESS                 0
#define ERROR_TABLE_FULL        1
#define ERROR_ELEM_NOT_FOUND    2
#define ERROR_DUP_KEY           3
#define ERROR_MEM_ALLOC         4
#define ERROR_FILE_OPEN         5

#define CELL_FREE               0
#define CELL_BUSY               1
#define CELL_DELETED            2


// ----------------------------------------------- Table structs ---------------------------------------------------- //

typedef char KeyType;
typedef int TableErrorCode;

typedef struct Cell
{
    int busy;
    KeyType* key;
    void* data;
} Cell;

typedef struct Table
{
    size_t size;
    size_t num_busy;
    Cell* p;
} Table;

// ------------------------------------------------------------------------------------------------------------------ //



// -------------------------------------------------- functions ----------------------------------------------------- //

size_t table_hash(const KeyType* key, size_t m);

size_t table_shuffle(size_t pos, size_t m);

Table* table_create(size_t size);

// ------------------------------------------------------------------------------------------------------------------ //



// --------------------------------------------------- methods ------------------------------------------------------ //

TableErrorCode table_insert(Table* t, const KeyType* key, void* data);

TableErrorCode table_update(Table* t, const KeyType* key, void* data);

TableErrorCode table_rm(Table* t, const KeyType* key, bool free_data);

void* table_get(Table* t, const KeyType* key);

void table_delete(Table* t, bool free_data);

// ------------------------------------------------------------------------------------------------------------------ //
