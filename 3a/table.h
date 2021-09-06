#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "list.h"

// ------------------------------------------- task variant dependent -------------------------------------------------

typedef  char*  KeyType1;
typedef  int    KeyType2;

typedef struct Info
{
    float f1;
    float f2;
    char* str;
} InfoType;

// --------------------------------------------------------------------------------------------------------------------


typedef struct Element
{
    KeyType1 key1;
    KeyType2 key2;
    int release;

    InfoType* info;
    struct Element* next;      /* для вещей с совпадающими обоими ключами */
} Element;


typedef struct Node1
{
    int release;
    Element* item;
    struct Node1* next;		/* для вещей с совпадающим 1 ключом			*/
} Node1;


typedef struct KeySpace1
{
    KeyType1 key;
    Node1* node;
    struct KeySpace1* next;		/* указатель на следующий элемент		*/
} KeySpace1;


typedef struct KeySpace2
{
    int busy;		    /* признак занятости элемента		*/
    KeyType2 key;
    int release;
    Element *item;	    /* указатель на информацию			*/
} KeySpace2;


typedef struct Table
{
    KeySpace1 *ks1;     /* указатель на первое пространство ключей			*/
    KeySpace2 *ks2;     /* указатель на второе пространство ключей			*/

/*  также может содержать опциональные поля, ограничивающие размер пространства ключей		*/
/*  при этом их наличие определяется типом организации соответствующего пространства   		*/

    int csize1;         /* текущее кол-во ключей в 1-м пространстве 		*/
    int csize2;         /* текущее кол-во ключей во 2-м пространстве 		*/

    int msize2;         /* размер области 2-го пространства ключей 			*/

} Table;


int table_hash(int x);
int table_shuffle(int x, int i);

Table* table(int vector_size);

Element* table_get(Table* t, KeyType1 key1, KeyType2 key2);
KeySpace1* table_list_search(const Table* t, KeyType1 key);
KeySpace2* table_hash_search(const Table* t, KeyType2 key, bool for_free);
List* table_get_ks_by_key2(Table* t, KeyType2 key2);

Table* table_get_subt_by_key1(Table* t, KeyType1 key1);
Table* table_get_subt_by_key2(Table* t, KeyType2 key2);

int table_insert(Table* t, KeyType1 key1, KeyType2 key2, InfoType* info);

int table_rm_elem(Table* t, KeyType1 key1, KeyType2 key2, int version);
int table_rm_key1(Table* t, KeyType1 key1);
int table_rm_key2(Table* t, KeyType2 key2);

void table_delete(Table* t, bool clear_info);

KeySpace1* _table_cleanup_ks1(Table* t, KeySpace1* ks1, Element* item, bool multi_node);
/*
 *    Table:
 *          keyspace 1:  viewed table / list
 *          [ ks:
 *              key
 *              node:
 *                  release
 *                  item:
 *                      key1
 *                      key2
 *                      release
 *                      info
 *                      next (same both keys)
 *                  next (same 1st key)
 *              next (list)
 *          ]
 *
 *          keyspace 2:  hashed table / vector
 *          [ ks:
 *              busy
 *              key
 *              release
 *              item
 *                  key1
 *                  key2
 *                  release
 *                  info
 *                  next (same both keys)
 *          ]
 *
 *
 *
 *
 */