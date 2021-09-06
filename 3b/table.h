#pragma once
#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-reserved-identifier"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "list.h"

#define SUCCESS 0

#define ERROR_TABLE_FULL 1
#define ERROR_ELEM_NOT_FOUND 2
#define ERROR_SOMETHING_BROKEN 3

#define ERROR_OPEN_DIR (1<<8)
#define ERROR_OPEN_TABLE (2<<8)
#define ERROR_OPEN_KS1 (3<<8)
#define ERROR_OPEN_KS2 (4<<8)
#define ERROR_OPEN_NODE (5<<8)
#define ERROR_OPEN_ITEM (6<<8)
#define ERROR_MEMORY_ALLOC (7<<8)

#define FREE 0
#define BUSY 1
#define DELETED 2

#ifndef PRIME_STEP
#define PRIME_STEP 17
#endif

#ifndef KEY_LENGTH
#define KEY_LENGTH 32
#endif

#ifndef INFO_LENGTH
#define INFO_LENGTH 32
#endif


// ------------------------------------------- task variant dependent -------------------------------------------------

typedef  char   KeyType1;
typedef  int    KeyType2;

typedef struct Info
{
    float f1;
    float f2;
    char str[INFO_LENGTH];
} InfoType;

// --------------------------------------------------------------------------------------------------------------------


typedef long Offset;

typedef struct Element
{
    KeyType1 key1[KEY_LENGTH];
    KeyType2 key2;
    int release;

    InfoType info;
    Offset next_offset;          /* для вещей с совпадающими обоими ключами   */
} Element;


typedef struct Node1
{
    int release;
    Offset item_offset;
    Offset next_offset;		   /* для вещей с совпадающим 1 ключом			*/
} Node1;


typedef struct KeySpace1
{
    KeyType1 key1[KEY_LENGTH];
    Offset node_offset;
} KeySpace1;


typedef struct KeySpace2
{
    int busy;		    /* признак занятости элемента		*/
    KeyType2 key;
    int release;
    Offset item_offset;	/* указатель на информацию			*/
} KeySpace2;


typedef struct Table
{
    int csize1;         /* текущее кол-во ключей в 1-м пространстве 		*/
    int csize2;         /* текущее кол-во ключей во 2-м пространстве 		*/
    int msize2;         /* размер области 2-го пространства ключей 			*/
    FILE* f_main;
    FILE* f_ks1;
    FILE* f_ks2;
    FILE* f_node;
    FILE* f_item;
} Table;


int table_hash(int x);
int table_shuffle(int x, int i);

Table* table_create(const char* dir, int vector_size);
Table* table_load(const char* dir);
void table_close(Table* t);

Offset table_list_search(Table* t, KeyType1* key);
Offset table_hash_search(Table* t, KeyType2 key, bool for_free);
List* table_get_ks_by_key2(Table* t, KeyType2 key);

int table_insert(Table* t, KeyType1* key1, KeyType2 key2, InfoType* info);
Offset table_get(Table* t, KeyType1* key1, KeyType2 key2);

Table* table_get_subt_by_key1(Table* t, KeyType1* key, const char* dir);
Table* table_get_subt_by_key2(Table* t, KeyType2 key, const char* dir);

int table_rm_elem(Table* t, KeyType1* key1, KeyType2 key2, int version);
int table_rm_key1(Table* t, KeyType1* key1);
int table_rm_key2(Table* t, KeyType2 key2);

void _table_cleanup_ks1(Table* t, Offset ks1, Offset item, bool multi_node);
void _table_write_params(Table* t);

#pragma clang diagnostic pop