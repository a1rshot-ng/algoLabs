#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SUCCESS 0
#define ERROR_MEM_FULL 1
#define ERROR_ELEM_NOT_FOUND 2
#define ERROR_FILE_OPEN 3

#ifndef INFO_LENGTH
#define INFO_LENGTH 64
#endif

// ------------------------------------------- Task variant dependant ----------------------------------------------- //
typedef u_int32_t KeyType;

typedef struct InfoType
{
    float f1;
    char* str;
} InfoType;
// ------------------------------------------------------------------------------------------------------------------ //

typedef struct Node
{
    KeyType key;
    struct Node* left;
    struct Node* right;
    struct Node* next;
    struct Node* master;
    struct InfoType* info;
} Node;


Node* bintree_create();

Node* bintree_load(char* filename);

int   bintree_add(Node** t, KeyType key, InfoType* info);

int   bintree_rm(Node** t, KeyType key, int version);

Node* bintree_find(Node* t, KeyType key, int version);

void  bintree_print_range(Node* t, KeyType key_min, KeyType key_max);

Node* bintree_min(Node* t, int version);

Node* bintree_max(Node* t, int version);

void  bintree_print(Node* t);

int   bintree_save(Node* t, char* file);

void  bintree_delete(Node* t);

Node* _bintree_findprev(Node* t);

Node* _bintree_findnext(Node* t);

Node* _bintree_findtarget(Node* t, KeyType key);

int   _bintree_attach(Node* t, KeyType key, InfoType* info);

void  _bintree_print_r(Node* t, int intend);

void  _bintree_write(Node* ptr, FILE* f);