/*
 *      Lab 5 list data structure
 *
 *      types described:
 *          Item - pointer to data, pointer to next item
 *          List - length, pointer to first item (NULL if empty)
 *
 *      data type is (void *) - this means that ANY data could be stored there!
 *
 *      functions:
 *          list()  returns an empty list
 *
 *      methods:
 *          get()
 *          get_item()
 *          pop()
 *          pop_next()
 *          push()
 *          append()
 *          insert()
 *          print()
 *          clear()
 *          clear_item()
 *          delete()
 */

#pragma once

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>


typedef struct Item
{
    void* data;
    struct Item* next;
} Item;

typedef struct List
{
    size_t length;
    Item* head;
    Item* tail;
} List;


List* list()
{
    List* empty_list = (List*) malloc(sizeof(List));
    empty_list->length = 0;
    empty_list->head = NULL;
    empty_list->tail = NULL;
    return empty_list;
}


void push(List* given_list, void* x)
{
    Item* ptr = (Item*) malloc(sizeof(Item));
    ptr->data = x;
    ptr->next = given_list->head;

    given_list->head = ptr;
    given_list->length++;

    if (given_list->length == 1)
        given_list->tail = given_list->head;
}


Item* get_item(List* given_list, size_t index)
{
    if (index >= given_list->length)
        return NULL;
    if (index+1 == given_list->length)
        return given_list->tail;

    Item* item = given_list->head;
    for (size_t i = 0; i < index; i++)
        item = item->next;

    return item;
}


void* get(List* given_list, size_t index)
{
    return get_item(given_list, index)->data;
}


void* pop(List* given_list, size_t index)
{
    void* x = NULL;
    if (given_list->length <= index || given_list->head == NULL)
        return NULL;

    if (index == 0)
    {
        Item* prev = given_list->head;
        x = prev->data;
        given_list->head = given_list->head->next;
        if (!given_list->head)
            given_list->tail = NULL;
        free(prev);
    }
    else if (index+1 < given_list->length)
    {
        Item* prev = get_item(given_list, index-1);
        Item* to_pop = prev->next;
        x = to_pop->data;

        prev->next = to_pop->next;
        free(to_pop);
    }
    else if (index+1 == given_list->length && given_list->length >= 2)
    {
        Item* prev = get_item(given_list, given_list->length-2);
        x = given_list->tail->data;
        given_list->tail = prev;
        free(prev->next);
        prev->next = NULL;
    }

    given_list->length--;
    return x;
}

void* pop_next(List* given_list, Item* prev)
{
    if (!prev)
        return pop(given_list, 0);

    if (prev->next == given_list->tail)
        given_list->tail = prev;

    Item* to_pop = prev->next;
    void* x = to_pop->data;
    prev->next = to_pop->next;
    free(to_pop);

    given_list->length--;
    return x;
}


void append(List* given_list, void* x)
{
    if (given_list->length)
    {
        Item *ptr = (Item *) malloc(sizeof(Item));
        ptr->data = x;
        ptr->next = NULL;

        Item *last = given_list->tail;
        last->next = ptr;
        given_list->tail = ptr;
        given_list->length++;
    }
    else
    {
        push(given_list, x);
    }
}


void insert(List* given_list, size_t index, void* x)
{
    if (index+1 > given_list->length)
    {
        append(given_list, x);
        return;
    }
    if (!index)
    {
        push(given_list, x);
        return;
    }

    Item* ptr = (Item*) malloc(sizeof(Item));
    ptr->data = x;

    Item* prev = get_item(given_list, index-1);
    ptr->next = prev->next;
    prev->next = ptr;

    given_list->length++;
}


void clear(List* given_list, size_t index)
{
    void* x = get(given_list, index);
    free(x);
}


void clear_item(Item* given_item)
{
    void* x = given_item->data;
    free(x);
}


void print(List* given_list, char* format)
{
    Item* curr = given_list->head;
    for (size_t i = 0; i < given_list->length; i++)
    {
        printf(format, *((int*) curr->data));
        curr = curr->next;
    }
}


void delete(List* given_list)
{
    while (given_list->head)
    {
        clear(given_list,0);
        pop(given_list,0);
    }
    free(given_list);
}
