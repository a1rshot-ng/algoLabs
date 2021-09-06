/*
 *      K-d tree data structure basic declarations
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

#include "list.h"

#define SUCCESS 0
#define ERROR_MEM_FULL 1
#define ERROR_ELEM_NOT_FOUND 2
#define ERROR_FILE_OPEN 4


// -------------------------------------------- Task variant dependant ---------------------------------------------- //

#define K 2

// N was not specified in task, but can be defined at build-time
#ifndef N
#define N 3  // N >= 2
#endif

typedef int KeyType;

typedef struct InfoType
{
    float f1;
    float f2;
    char* str;
} InfoType;

// ------------------------------------------------------------------------------------------------------------------ //



// ------------------------------------------------ Basic structs --------------------------------------------------- //

typedef struct Node
{
    KeyType bound;
    int dir;         // line / flat / hyperflat direction
    struct Node* left;
    struct Node* right;
    struct Node* master;
    List* elements;  // used only if it's a leaf
} Node;

typedef struct Element
{
    KeyType point[K];
    InfoType* info;
} Element;

// ------------------------------------------------------------------------------------------------------------------ //



// ---------------------------------------------- extern functions -------------------------------------------------- //

bool  kdtree_samepoints(const KeyType p1[], const KeyType p2[]);

Node* kdtree_create();

Node* kdtree_load(const char* file);

Node* kdtree_newnode(KeyType bound, int dir, Node* master, Element* el);

Element* kdtree_find(Node* t, const KeyType point[], int version);

Element* kdtree_max(Node* t, int cd, int version);

Element* kdtree_farthest(Node* t);

void  kdtree_maxnode(Node* t, int cd, KeyType* max_key, Node** ans);

void  kdtree_list_range(Node* t, const KeyType point_max[], List* buf);

void  kdtree_print_range(Node* t, const KeyType point_max[], int cd);

int   kdtree_add(Node** t, const KeyType point[], InfoType* info);

int   kdtree_rm(Node** t, const KeyType point[], int version);

int   kdtree_save(Node* t, const char* filename);

int   kdtree_export(Node* t, const char* filename);

void  kdtree_print(Node* t);

void  kdtree_delete(Node* t);

// ------------------------------------------------------------------------------------------------------------------ //



// ----------------------------------------------- internal utils --------------------------------------------------- //

static void kdtree_print_r(Node* t, int intend);

static void kdtree_export_r(Node* t, int intend, FILE* f);

static bool metrics0(Element* a, Element* b);

static bool metrics1(Element* a, Element* b);

static void kdtree_list_sort(List* l, int cd, bool reversed);

static int kdtree_list_insert(List* l, Element* el, int cd);

static int kdtree_split(Node* t, int index);

static int kdtree_insert_r(Node** t, const KeyType *point, Node* master, InfoType* info, int depth);

static Node* kdtree_find_r(Node* t, const KeyType *point, int depth);

static Element* kdtree_list_max(List* l, int cd, int version);

static void kdtree_farthest_r(Node* t, long* dist2, Element** ans);

static void kdtree_write_r(Node* t, FILE* f);

// ------------------------------------------------------------------------------------------------------------------ //
