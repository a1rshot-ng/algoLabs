#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


typedef long ListDtype;

typedef struct Item
{
    long data;
    struct Item* next;
} Item;

typedef struct List
{
    size_t length;
    Item* head;
    Item* tail;
} List;


List* list();

Item* list_getitem(List* given_list, size_t index);

ListDtype list_get(List* given_list, size_t index);

ListDtype list_pop(List* given_list, size_t index);

ListDtype list_popnext(List* given_list, Item* prev);

void list_push(List* given_list, ListDtype x);

void list_append(List* given_list, ListDtype x);

void list_insert(List* given_list, size_t index, ListDtype x);

void list_clear(List* given_list, size_t index);

void list_clearitem(Item* given_item);

void list_print(List* given_list, const char* format);

void list_delete(List* given_list, bool clear);
