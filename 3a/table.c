#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "table.h"
#include "list.h"

#define SUCCESS 0
#define ERROR_TABLE_FULL 1
#define ERROR_ELEM_NOT_FOUND 2
#define ERROR_SOMETHING_BROKEN 3

#define FREE 0
#define BUSY 1
#define DELETED 2

#ifndef PRIME_STEP
 #define PRIME_STEP 17
#endif

#pragma clang diagnostic push
#pragma ide diagnostic ignored "DanglingPointers"

int table_hash(int x)
{
    srand(x);
    return rand();
}


int table_shuffle(int x, int i)
{
    return x + i * PRIME_STEP;
}


Table* table(int vector_size)
{
    Table* t = malloc(sizeof(Table));
    t->ks1 = NULL;
    t->ks2 = malloc(sizeof(KeySpace2) * vector_size);

    int i;
    for (i = 0; i < vector_size; i++)
        t->ks2[i].busy = false;

    t->csize1 = 0;
    t->csize2 = 0;
    t->msize2 = vector_size;

    if (t->msize2 % PRIME_STEP == 0)
        t->msize2++;

    return t;
}


KeySpace1* table_list_search(const Table* t, KeyType1 key)
{
    KeySpace1* ptr;

    for (ptr = t->ks1; ptr; ptr = ptr->next)
        if (ptr->key == key || !strcmp(ptr->key, key))
            return ptr;

    return NULL;
}


KeySpace2* table_hash_search(const Table* t, KeyType2 key, bool for_free)  // for free cell or for key
{
    int i, m = t->msize2;
    int h = table_hash(key) % m;
    int next_hash = -1;

    for (i = h; next_hash != h; i = table_shuffle(h, i) % m, next_hash = i)
    {
        if (!for_free && t->ks2[i].key == key && t->ks2[i].busy == BUSY || (for_free && t->ks2[i].busy != BUSY))
            return &(t->ks2[i]);

        if (!for_free && t->ks2->busy == FREE)
            return NULL;
    }

    return NULL;
}


int table_insert(Table* t, KeyType1 key1, KeyType2 key2, InfoType* info)
{
    List* ks_list = table_get_ks_by_key2(t, key2);
    int rel = ks_list->length;

    if (ks_list->length)   // key is NOT unique and there is some item with both same keys
    {
        Item* ks_tested;
        for (ks_tested = ks_list->head; ks_tested; ks_tested = ks_tested->next)   // checking if key1 is already present
            if (((KeySpace2*)ks_tested->data)->item->key1 == key1 || !strcmp(key1, ((KeySpace2*)ks_tested->data)->item->key1))
                break;

        if (ks_tested)   // there is an item with both same keys
        {
            // iterating through versions of item to  1) get release, 2) append item
            Element* item = malloc(sizeof(Element));
            Element* item_prev;

            rel = ((KeySpace2*) ks_tested->data)->item->release + 1;
            for (item_prev = ((KeySpace2*) ks_tested->data)->item; item_prev->next; item_prev = item_prev->next)
                if (item_prev->next->release >= rel)
                    rel = item_prev->next->release + 1;

            item_prev->next = item;

            item->key1 = strdup(key1);
            item->key2 = key2;
            item->release = rel;

            item->info = info;
            item->next = NULL;

            list_delete(ks_list, false);
            return SUCCESS;
        }
    }

    list_delete(ks_list, false);

    // no item that has both same keys, thus creating a new one
    Element* item = malloc(sizeof(Element));

    item->release = 0;
    item->key1 = strdup(key1);
    item->key2 = key2;

    item->info = info;
    item->next = NULL;

    // for table_hash-table keyspace 2
    KeySpace2* ks2 = table_hash_search(t, key2, true);

    // if the table is full
    if (!ks2)
    {
        free(item);
        return ERROR_TABLE_FULL;
    }

    ks2->busy = BUSY;
    ks2->key = key2;
    ks2->release = rel;

    ks2->item = item;
    t->csize2++;

    // for list-table keyspace 1
    KeySpace1* ks1 = table_list_search(t, key1);

    if (ks1)   // if key1 is not unique
    {

        Node1* node_prev = ks1->node;
        Node1* node = malloc(sizeof(Node1));

        node->release = node_prev->release + 1;
        node->item = item;
        node->next = node_prev;

        ks1->node = node;
    }
    else   // if both keys are unique
    {
        ks1 = malloc(sizeof(KeySpace1));

        ks1->key = strdup(key1);
        ks1->node = malloc(sizeof(Node1));

        ks1->node->release = 0;
        ks1->node->item = item;
        ks1->node->next = NULL;

        ks1->next = t->ks1;
        t->ks1 = ks1;
        t->csize1++;
    }

    return SUCCESS;
}


List* table_get_ks_by_key2(Table* t, KeyType2 key2)
{
    List* list_ks = list();

    int i, m = t->msize2;
    int h = table_hash(key2) % m;
    int next_hash = -1;

    for (i = h; next_hash != h; i = table_shuffle(h, i) % m, next_hash = i)
    {
        if (t->ks2[i].busy == FREE)
            return list_ks;
        if (t->ks2[i].key == key2 && t->ks2[i].busy == BUSY)
            list_push(list_ks, &(t->ks2[i]));
    }

    return list_ks;
}


Element* table_get(Table* t, KeyType1 key1, KeyType2 key2)
{
    // first, getting the list of KS's with same key2, then searching for key1 in them
    List* ks2_list = table_get_ks_by_key2(t, key2);

    KeySpace2* ks;
    while ((ks = (KeySpace2*) list_pop(ks2_list, 0)))
        if (ks->item->key1 == key1 || !strcmp(key1, ks->item->key1))
        {
            list_delete(ks2_list, false);
            return ks->item;
        }

    list_delete(ks2_list, false);
    return NULL;
}


Table* table_get_subt_by_key1(Table* t, KeyType1 key1)
{
    KeySpace1* ks = table_list_search(t, key1);
    Node1* ks_list = ks ? ks->node : NULL;

    Node1* curr_node;    // to find out how many different key2's are there for specific key1
    int len;
    for (len = 0, curr_node = ks_list; curr_node; len++, curr_node = curr_node->next);

    Table* subt = table(len + 2);

    Node1* node;
    Element* item;
    for (node = ks_list; node; node = node->next)
        for (item = node->item; item; item = item->next)
            table_insert(subt, key1, item->key2, item->info);

    return subt;
}


Table* table_get_subt_by_key2(Table* t, KeyType2 key2)
{
    List* ks_list = table_get_ks_by_key2(t, key2);

    KeySpace2* ks;
    KeyType1 key1;
    Element* i;

    Table* subt = table((int) ks_list->length + 2);

    while ((ks = (KeySpace2*) list_pop(ks_list, 0)))
    {
        key1 = ks->item->key1;
        for (i = ks->item; i; i = i->next)
            table_insert(subt, key1, key2, i->info);
    }

    list_delete(ks_list, false);
    return subt;
}


int table_rm_elem(Table* t, KeyType1 key1, KeyType2 key2, int version)
{
    // removes specific version of an element OR all versions if "version" set to -1
    List* ks2_list = table_get_ks_by_key2(t, key2);
    KeySpace1* ks1 = table_list_search(t, key1);

    if (!ks1 || !ks2_list->length)
    {
        list_delete(ks2_list, false);
        return ERROR_ELEM_NOT_FOUND;        // no elements with that key1 or key2
    }

    KeySpace2* ks2;
    while ((ks2 = (KeySpace2*) list_pop(ks2_list, 0)))
        if (ks2->item->key1 == key1 || !strcmp(key1, ks2->item->key1))
        {
            // found KS2 and Element
            list_delete(ks2_list, false);

            // deleting the version OR all of them
            Element* item = ks2->item;
            Element* item_prev = NULL;
            Element* item_next;

            Node1* node;

            while (item)
            {
                item_next = item->next;
                if (item->release == version || version == -1)
                {
                    free(item->info->str);
                    free(item->info);
                    free(item->key1);
                    free(item);

                    if (ks2->item == item)          // first item in the versions list
                        ks2->item = item_next;

                    for (node = ks1->node; node; node = node->next)
                        if (node->item == item)
                            node->item = item_next;

                    if (item_prev)
                        item_prev->next = item_next;          // linking previous and next items near the missing one

                    if (version != -1)              // assuming there is only one item with specific version
                        break;
                }
                else
                    item_prev = item;
                item = item_next;
            }

            // if there are no versions left
            // assuming ks1.node.item and ks2.item are both NULL

            // cleanup & delete from KS1
            _table_cleanup_ks1(t, ks1, NULL, false);

            // delete from KS2
            if (!ks2->item)
            {
                ks2->busy = DELETED;
                t->csize2--;
            }

            return SUCCESS;
        }

    // no elements with that composite key
    list_delete(ks2_list, false);
    return ERROR_ELEM_NOT_FOUND;
}


int table_rm_key1(Table* t, KeyType1 key1)
{
    //  key1 -> node -> items -> keys2 -> get_ks2 -> deleted
    int removed = 0;

    // find node
    KeySpace1* ks1 = t->ks1;
    KeySpace1* ks1_prev = NULL;
    KeySpace2* ks2;

    Node1* node;
    Node1* node_next;

    Element* item;
    Element* item_next;

    KeyType2 key2;
    List* ks2_list;

    // clear KS1 & KS2
    while (ks1)
        if (ks1->key == key1 || !strcmp(key1, ks1->key))
        {
            node = ks1->node;
            while (node)
            {
                item = node->item;
                while (item)
                {
                    free(item->info->str);
                    free(item->info);
                    free(item->key1);

                    key2 = item->key2;
                    ks2_list = table_get_ks_by_key2(t, key2);
                    while ((ks2 = (KeySpace2*)list_pop(ks2_list, 0)))  // clear KS2
                        if (ks2->item == item && ks2->busy == BUSY)
                        {
                            ks2->busy = DELETED;
                            t->csize2--;
                            break;
                        }

                    list_delete(ks2_list, false);

                    item_next = item->next;
                    free(item);
                    item = item_next;

                    removed++;
                }
                node_next = node->next;
                free(node);
                node = node_next;
            }

            if (!ks1_prev)
            {
                if (t->ks1 == ks1)
                    t->ks1 = ks1->next;
                else
                    return ERROR_SOMETHING_BROKEN;
            }
            else
                ks1_prev->next = ks1->next;

            free(ks1->key);
            free(ks1);

            t->csize1--;

            return removed;
        }
        else
        {
            ks1_prev = ks1;
            ks1 = ks1->next;
        }

    return removed;
}


int table_rm_key2(Table* t, KeyType2 key2)
{
    // key2 -> get_ks2 -> keys1 -> search for key1
    int removed = 0;

    List* ks2_list = table_get_ks_by_key2(t, key2);

    KeySpace1* ks1;
    KeySpace2* ks2;
    Element* item;
    Element* item_next;

    while ((ks2 = (KeySpace2*)list_pop(ks2_list, 0)))
    {
        ks2->busy = DELETED;
        t->csize2--;
        item = ks2->item;
        while (item)
        {
            free(item->info->str);
            free(item->info);
            free(item->key1);

            for (ks1 = t->ks1; ks1; ks1 = _table_cleanup_ks1(t, ks1, item, true));

            item_next = item->next;
            free(item);
            item = item_next;

            removed++;
        }
    }

    list_delete(ks2_list, false);

    return removed;
}


void table_delete(Table* t, bool clear_info)
{
    KeySpace1* ks1 = t->ks1;
    KeySpace1* ks1_next;

    Node1* node;
    Node1* node_next;

    Element* item;
    Element* item_next;

    // clear KS1
    while (ks1)
    {
        node = ks1->node;
        while (node)
        {
            item = node->item;
            while (item)
            {
                if (clear_info)
                {
                    free(item->info->str);
                    free(item->info);
                }

                free(item->key1);

                item_next = item->next;
                free(item);
                item = item_next;
            }
            node_next = node->next;
            free(node);
            node = node_next;
        }

        free(ks1->key);

        ks1_next = ks1->next;
        free(ks1);
        ks1 = ks1_next;
    }

    // clear KS2
    free(t->ks2);

    // delete table itself
    free(t);

}


KeySpace1* _table_cleanup_ks1(Table* t, KeySpace1* ks1, Element* item, bool multi_node)
{
    KeySpace1* next;
    Node1* node = ks1->node;
    Node1* node_prev = NULL;
    Node1* node_next;

    while (node)
    {
        node_next = node->next;
        if (node->item == item)
        {
            free(node);

            if (ks1->node == node)
                ks1->node = node_next;

            if (node_prev)
                node_prev->next = node_next;

            if (!multi_node)
                break;                      // if there is only one node with NULL item
        }
        else
            node_prev = node;
        node = node_next;
    }

    // now if ks1.node is NULL, delete ks1
    if (!ks1->node)
    {
        KeySpace1* ks1_prev;
        for (ks1_prev = t->ks1; ks1_prev; ks1_prev = ks1_prev->next)
            if (ks1_prev->next == ks1)
                break;

        if (!ks1_prev)
        {
            if (t->ks1 == ks1)
                t->ks1 = ks1->next;
            else
                return NULL;
        }
        else
            ks1_prev->next = ks1->next;

        next = ks1->next;
        free(ks1->key);
        free(ks1);
        t->csize1--;

        return next;
    }

    return ks1->next;
}

#pragma clang diagnostic pop


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
