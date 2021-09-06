/*
 *      list data structure
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
 *          list_getitem()
 *          pop()
 *          pop_next()
 *          list_push()
 *          append()
 *          insert()
 *          print()
 *          clear()
 *          clear_item()
 *          sort()
 *          delete()
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "list.h"


List* list()
{
    List* empty_list = (List*) malloc(sizeof(List));
    empty_list->length = 0;
    empty_list->head = NULL;
    empty_list->tail = NULL;
    return empty_list;
}

void list_push(List* given_list, void* x)
{
    Item* ptr = (Item*) malloc(sizeof(Item));
    ptr->data = x;
    ptr->next = given_list->head;

    given_list->head = ptr;
    given_list->length++;

    if (given_list->length == 1)
        given_list->tail = given_list->head;
}

Item* list_getitem(List* given_list, size_t index)
{
    if (!given_list || index >= given_list->length)
        return NULL;
    if (index+1 == given_list->length)
        return given_list->tail;

    Item* item = given_list->head;
    for (size_t i = 0; i < index && item; i++)
        item = item->next;

    return item;
}

void* list_get(List* given_list, size_t index)
{
    Item* item = list_getitem(given_list, index);
    if (!item) return NULL;
    return item->data;
}

void* list_pop(List* given_list, size_t index)
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
        Item* prev = list_getitem(given_list, index - 1);
        Item* to_pop = prev->next;
        x = to_pop->data;

        prev->next = to_pop->next;
        free(to_pop);
    }
    else if (index+1 == given_list->length && given_list->length >= 2)
    {
        Item* prev = list_getitem(given_list, given_list->length - 2);
        x = given_list->tail->data;
        given_list->tail = prev;
        free(prev->next);
        prev->next = NULL;
    }

    given_list->length--;
    return x;
}

void* list_popnext(List* given_list, Item* prev)
{
    if (!prev)
        return list_pop(given_list, 0);

    if (prev->next == given_list->tail)
        given_list->tail = prev;

    Item* to_pop = prev->next;
    void* x = to_pop->data;
    prev->next = to_pop->next;
    free(to_pop);

    given_list->length--;
    return x;
}

void list_append(List* given_list, void* x)
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
        list_push(given_list, x);
    }
}

void list_insert(List* given_list, size_t index, void* x)
{
    if (index+1 > given_list->length)
    {
        list_append(given_list, x);
        return;
    }
    if (!index)
    {
        list_push(given_list, x);
        return;
    }

    Item* ptr = (Item*) malloc(sizeof(Item));
    ptr->data = x;

    Item* prev = list_getitem(given_list, index - 1);
    ptr->next = prev->next;
    prev->next = ptr;

    given_list->length++;
}

void list_clear(List* given_list, size_t index)
{
    void* x = list_get(given_list, index);
    free(x);
}

void list_clearitem(Item* given_item)
{
    void* x = given_item->data;
    free(x);
}

void list_print(List* given_list, const char* format)
{
    Item* curr = given_list->head;
    for (size_t i = 0; i < given_list->length; i++)
    {
        printf(format, *((int*) curr->data));
        curr = curr->next;
    }
}

void list_delete(List* given_list, bool clear)
{
    while (given_list->head)
    {
        if (clear)
            list_clear(given_list, 0);

        list_pop(given_list, 0);
    }
    free(given_list);
}


List* list_copy(List* given_list)
{
    List* new_list = list();
    Item* i;

    for (i = given_list->head; i; i = i->next)
        list_append(new_list, i->data);

    return new_list;
}


void list_sort(List* given_list, bool (*metrics)(), bool reversed)
{
    if (!given_list || !given_list->head) return;
    given_list->head = list_sort_r(given_list->head, metrics, reversed);

    Item* i;
    for (i = given_list->head; i->next; i = i->next);
    given_list->tail = i;
}


static Item* list_sort_r(Item* head, bool (*metrics)(), bool reversed)
{
    if (!head->next) return head;

    Item* mid = list_sort_findmid(head);
    Item* head2 = mid->next;
    mid->next = NULL;

    Item* new_head1 = list_sort_r(head, metrics, reversed);
    Item* new_head2 = list_sort_r(head2, metrics, reversed);

    return list_sort_merge(new_head1, new_head2, metrics, reversed);
}


static Item* list_sort_merge(Item* a, Item* b, bool (*metrics)(), bool reversed)
{
    Item* ans;
    Item* result = malloc(sizeof(Item));
    Item* temp = result;
    if (!result) return NULL;

    while (a && b)
    {
        if (metrics(a->data, b->data) ^ reversed)
        {
            temp->next = b;
            b = b->next;
        }
        else
        {
            temp->next = a;
            a = a->next;
        }
        temp = temp->next;
    }
    while (a)
    {
        temp->next = a;
        a = a->next;
        temp = temp->next;
    }
    while (b)
    {
        temp->next = b;
        b = b->next;
        temp = temp->next;
    }

    ans = result->next;
    free(result);
    return ans;
}


static Item* list_sort_findmid(Item* src)
{
    Item* slow = src;
    Item* fast = src->next;

    while (fast)
    {
        fast = fast->next;
        if (fast)
        {
            slow = slow->next;
            fast = fast->next;
        }
    }
    return slow;
}
