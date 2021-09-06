#pragma once

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wint-conversion"
#pragma ide diagnostic ignored "bugprone-reserved-identifier"
#pragma ide diagnostic ignored "cppcoreguidelines-narrowing-conversions"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#include "table.h"
#include "list.h"


void _table_write_params(Table *t) {
    fseek(t->f_main, 0, SEEK_SET);
    fwrite(t, sizeof(int), 3, t->f_main);
}

int table_hash(int x)
{
    srand(x);
    return rand();
}


int table_shuffle(int x, int i)
{
    return x + i * PRIME_STEP;
}


Table* table_create(const char* dir, int vector_size)
{
    if (mkdir(dir, 493) != 0 || chdir(dir) != 0)
        return ERROR_OPEN_DIR | errno;

    Table* t = malloc(sizeof(Table));
    if (!t)
        return ERROR_MEMORY_ALLOC;

    t->csize1 = 0;
    t->csize2 = 0;
    t->msize2 = vector_size % PRIME_STEP ? vector_size : vector_size+1;

    if (!(t->f_main = fopen("table.dat", "w+b")))
        return ERROR_OPEN_TABLE | errno;
    fwrite(&t->csize1, sizeof(int), 3, t->f_main);

    if (!(t->f_ks1 = fopen("ks1.dat", "w+b")))
        return ERROR_OPEN_KS1 | errno;

    KeySpace2 ks2;
    ks2.busy = FREE;
    int i;
    if (!(t->f_ks2 = fopen("ks2.dat", "w+b")))
        return ERROR_OPEN_KS2 | errno;
    for (i = 0; i < t->msize2; i++)
    {
        fwrite(&ks2.busy, sizeof(int), 1, t->f_ks2);
        fwrite(&ks2.key, sizeof(KeyType2), 1, t->f_ks2);
        fwrite(&ks2.release, sizeof(int), 2, t->f_ks2);
        fwrite(&ks2.item_offset, sizeof(Offset), 1, t->f_ks2);
    }

    if (!(t->f_node = fopen("node.dat", "w+b")))
        return ERROR_OPEN_NODE | errno;
    if (!(t->f_item = fopen("item.dat", "w+b")))
        return ERROR_OPEN_ITEM | errno;

    chdir("..");
    return t;
}


Table* table_load(const char* dir)
{
    if (chdir(dir) != 0)
        return ERROR_OPEN_DIR | errno;

    Table* t = malloc(sizeof(Table));
    if (!t)
        return ERROR_MEMORY_ALLOC | errno;

    if (!(t->f_main = fopen("table.dat", "r+b")))
    {
        free(t);
        return ERROR_OPEN_TABLE | errno;
    }
    if (!(t->f_ks1 = fopen("ks1.dat", "r+b")))
    {
        fclose(t->f_main);
        free(t);
        return ERROR_OPEN_KS1 | errno;
    }
    if (!(t->f_ks2 = fopen("ks2.dat", "r+b")))
    {
        fclose(t->f_ks1);
        fclose(t->f_main);
        free(t);
        return ERROR_OPEN_KS2 | errno;
    }
    if (!(t->f_node = fopen("node.dat", "r+b")))
    {
        fclose(t->f_ks2);
        fclose(t->f_ks1);
        fclose(t->f_main);
        free(t);
        return ERROR_OPEN_NODE | errno;
    }
    if (!(t->f_item = fopen("item.dat", "r+b")))
    {
        fclose(t->f_node);
        fclose(t->f_ks2);
        fclose(t->f_ks1);
        fclose(t->f_main);
        free(t);
        return ERROR_OPEN_ITEM | errno;
    }

    fread(t, sizeof(int), 3, t->f_main);

    chdir("..");
    return t;
}


void table_close(Table* t)
{
    _table_write_params(t);

    fclose(t->f_main);
    fclose(t->f_ks1);
    fclose(t->f_ks2);
    fclose(t->f_node);
    fclose(t->f_item);

    free(t);
}


Offset table_list_search(Table* t, KeyType1* key)
{
    Offset ks1;
    fseek(t->f_ks1, 0, SEEK_END);
    Offset eof = ftell(t->f_ks1);
    KeySpace1 kbuf;

    fseek(t->f_ks1, 0, SEEK_SET);
    for (ks1 = 0; ks1 < eof; ks1 += sizeof(KeySpace1))
    {
        fread(&kbuf.key1, sizeof(KeyType1), KEY_LENGTH, t->f_ks1);
        fread(&kbuf.node_offset, sizeof(Offset), 1, t->f_ks1);
        if (!strcmp(key, kbuf.key1))
            return ks1;
    }
    return -1;
}


Offset table_hash_search(Table* t, KeyType2 key, bool for_free)
{
    int i, m = t->msize2;
    int h = table_hash(key) % m;
    int next_hash = -1;

    int t_ks2_i_busy;
    KeyType2 t_ks2_i_key;

    for (i = h; next_hash != h; i = table_shuffle(h, i) % m, next_hash = i)
    {
        fseek(t->f_ks2, i*sizeof(KeySpace2), SEEK_SET);
        fread(&t_ks2_i_busy, sizeof(int), 1, t->f_ks2);
        fread(&t_ks2_i_key, sizeof(KeyType2), 1, t->f_ks2);

        if (!for_free && t_ks2_i_key == key && t_ks2_i_busy == BUSY || for_free && t_ks2_i_busy != BUSY)
            return i*sizeof(KeySpace2);

        if (!for_free && t_ks2_i_busy == FREE)
            return -1;
    }

    return -1;
}


List* table_get_ks_by_key2(Table* t, KeyType2 key)
{
    List* list_ks = list();

    int i, m = t->msize2;
    int h = table_hash(key) % m;
    int next_hash = -1;

    int t_ks2_i_busy = 0;
    KeyType2 t_ks2_i_key = 0;

    for (i = h; next_hash != h; i = table_shuffle(h, i) % m, next_hash = i)
    {
        fseek(t->f_ks2, i*sizeof(KeySpace2), SEEK_SET);
        fread(&t_ks2_i_busy, sizeof(int), 1, t->f_ks2);
        fread(&t_ks2_i_key, sizeof(KeyType2), 1, t->f_ks2);

        if (t_ks2_i_busy == FREE)
            return list_ks;

        if (t_ks2_i_key == key && t_ks2_i_busy == BUSY)
            list_push(list_ks, i*sizeof(KeySpace2));
    }

    return list_ks;
}


int table_insert(Table* t, KeyType1* key1, KeyType2 key2, InfoType* info)
{
    List* ks_list = table_get_ks_by_key2(t, key2);
    Offset ks2 = -1;
    Offset eof = -1;
    int rel = ks_list->length;

    if (rel)    // key is NOT unique and there is some item with both same keys
    {
        Offset ks2_i_item;
        KeyType1 keybuf[KEY_LENGTH];
        while ((ks2 = list_pop(ks_list, 0)) != -1)
        {
            fseek(t->f_ks2, ks2 + sizeof(KeySpace2)-sizeof(Offset), SEEK_SET);
            fread(&ks2_i_item, sizeof(Offset), 1, t->f_ks2);
            fseek(t->f_item, ks2_i_item, SEEK_SET);
            fread(keybuf, sizeof(KeyType1), KEY_LENGTH, t->f_item);
            if (!strcmp(key1, keybuf))
                break;
        }

        if (ks2 != -1)    // there is an item with both same keys
        {
            Element* item = malloc(sizeof(Element));
            if (!item)
                return ERROR_SOMETHING_BROKEN;
            Offset item_prev;
            Offset item_prev_next;
            int rel_test;

            fseek(t->f_item, ks2_i_item + KEY_LENGTH*sizeof(KeyType1)+sizeof(KeyType2), SEEK_SET);
            fread(&rel, sizeof(int), 1, t->f_item);
            rel++;
            for (item_prev = ks2_i_item,
                    fseek(t->f_item, item_prev+sizeof(Element)-sizeof(Offset), SEEK_SET),
                    fread(&item_prev_next, sizeof(Offset), 1, t->f_item); item_prev_next != -1;
                    fseek(t->f_item, item_prev_next+sizeof(Element)-sizeof(Offset), SEEK_SET),
                    fread(&item_prev_next, sizeof(Offset), 1, t->f_item), item_prev = item_prev_next)
            {
                fseek(t->f_item, item_prev_next, SEEK_SET);
                fread(&rel_test, sizeof(int), 1, t->f_item);
                if (rel_test >= rel)
                    rel = rel_test + 1;
            }

            fseek(t->f_item, 0, SEEK_END);
            eof = ftell(t->f_item);
            fseek(t->f_item, item_prev + sizeof(Element)-sizeof(Offset), SEEK_SET);
            fwrite(&eof, sizeof(Offset), 1, t->f_item);

            strncpy(item->key1, key1, KEY_LENGTH);
            item->key2 = key2;
            item->release = rel;

            strncpy(item->info.str, info->str, INFO_LENGTH);
            item->info.f1 = info->f1;
            item->info.f2 = info->f2;

            item->next_offset = -1;

            fseek(t->f_item, 0, SEEK_END);
            fwrite(item, sizeof(Element), 1, t->f_item);

            free(item);
            list_delete(ks_list, false);
            _table_write_params(t);
            return SUCCESS;
        }
    }

    list_delete(ks_list, false);

    // no item that has both same keys, thus creating a new one
    Element* item = malloc(sizeof(Element));
    if (!item)
        return ERROR_SOMETHING_BROKEN;
    Offset item_offset;

    strcpy(item->key1, key1);
    item->key2 = key2;
    item->release = 0;

    strcpy(item->info.str, info->str);
    item->info.f1 = info->f1;
    item->info.f2 = info->f2;

    item->next_offset = -1;

    fseek(t->f_item, 0, SEEK_END);
    item_offset = ftell(t->f_item);
    fwrite(item, sizeof(Element), 1, t->f_item);
    free(item);

    ks2 = table_hash_search(t, key2, true);
    KeySpace2 ks2obj;
    if (ks2 == -1)
        return ERROR_TABLE_FULL;

    ks2obj.busy = BUSY;
    ks2obj.key = key2;
    ks2obj.release = rel;

    ks2obj.item_offset = item_offset;
    t->csize2++;

    fseek(t->f_ks2, ks2, SEEK_SET);
    fwrite(&ks2obj.busy, sizeof(int), 1, t->f_ks2);
    fwrite(&ks2obj.key, sizeof(KeyType2), 1, t->f_ks2);
    fwrite(&ks2obj.release, sizeof(int), 2, t->f_ks2);
    fwrite(&ks2obj.item_offset, sizeof(Offset), 1, t->f_ks2);

    // for list-table keyspace 1
    Offset ks1 = table_list_search(t, key1);
    Node1 node;
    if (ks1 != -1)
    {
        Offset node_prev;
        int node_prev_rel;
        fseek(t->f_ks1, ks1 + KEY_LENGTH*sizeof(KeyType1), SEEK_SET);
        fread(&node_prev, sizeof(Offset), 1, t->f_ks1);
        fseek(t->f_node, node_prev, SEEK_SET);
        fread(&node_prev_rel, sizeof(int), 1, t->f_node);

        node.release = node_prev_rel + 1;
        node.item_offset = item_offset;
        node.next_offset = node_prev;

        fseek(t->f_node, 0, SEEK_END);
        eof = ftell(t->f_node);
        fwrite(&node.release, sizeof(int), 1, t->f_node);
        fseek(t->f_node, sizeof(int), SEEK_CUR);
        fwrite(&node.item_offset, sizeof(Offset), 1, t->f_node);
        fwrite(&node.next_offset, sizeof(Offset), 1, t->f_node);
        fseek(t->f_ks1, ks1 + KEY_LENGTH*sizeof(KeyType1), SEEK_SET);
        fwrite(&eof, sizeof(Offset), 1, t->f_ks1);
    }
    else    // if both keys are unique
    {
        KeySpace1* ks1obj = malloc(sizeof(KeySpace1));
        if (!ks1obj)
            return ERROR_SOMETHING_BROKEN;

        node.release = 0;
        node.item_offset = item_offset;
        node.next_offset = -1;

        strcpy(ks1obj->key1, key1);
        fseek(t->f_node, 0, SEEK_END);
        eof = ftell(t->f_node);
        fwrite(&node.release, sizeof(int), 2, t->f_node);
        fwrite(&node.item_offset, sizeof(Offset), 1, t->f_node);
        fwrite(&node.next_offset, sizeof(Offset), 1, t->f_node);
        ks1obj->node_offset = eof;

        fseek(t->f_ks1, 0, SEEK_END);
        eof = ftell(t->f_ks1);
        fwrite(ks1obj->key1, sizeof(KeyType1), KEY_LENGTH, t->f_ks1);
        fwrite(&ks1obj->node_offset, sizeof(Offset), 1, t->f_ks1);

        free(ks1obj);
        t->csize1++;
    }

    _table_write_params(t);
    return SUCCESS;
}


Offset table_get(Table* t, KeyType1* key1, KeyType2 key2)
{
    // first, getting the list of KS's with same key2, then searching for key1 in them
    List* ks2_list = table_get_ks_by_key2(t, key2);

    Offset ks = -1;
    Offset ks_item = -1;
    KeyType1 keybuf[KEY_LENGTH];

    while ((ks = list_pop(ks2_list, 0)) != -1)
    {
        fseek(t->f_ks2, ks + sizeof(KeySpace2)-sizeof(Offset), SEEK_SET);
        fread(&ks_item, sizeof(Offset), 1, t->f_ks2);
        fseek(t->f_item, ks_item, SEEK_SET);
        fread(keybuf, sizeof(KeyType1), KEY_LENGTH, t->f_item);
        if (!strcmp(key1, keybuf))
        {
            list_delete(ks2_list, false);
            return ks_item;
        }
    }

    list_delete(ks2_list, false);
    return -1;
}

Table* table_get_subt_by_key1(Table* t, KeyType1* key, const char* dir)
{
    Offset ks1 = table_list_search(t, key);
    Offset node;
    if (ks1 != -1)
    {
        fseek(t->f_ks1, ks1 + KEY_LENGTH*sizeof(KeyType1), SEEK_SET);
        fread(&node, sizeof(Offset), 1, t->f_ks1);
    }
    else node = -1;

    Offset curr_node;
    int len;
    for (len = 0, curr_node = node;
        curr_node != -1;
        len++, fseek(t->f_node, curr_node + sizeof(Node1)-sizeof(Offset), SEEK_SET),
        fread(&curr_node, sizeof(Offset), 1, t->f_node));

    Table* subt = table_create(dir, len + 2);
    if ((long) subt < (1<<16))
        return subt;

    Offset item_offset = -1;
    Element* item = malloc(sizeof(Element));
    for (; node != -1;
        fseek(t->f_node, node + sizeof(Node1)-sizeof(Offset), SEEK_SET),
        fread(&node, sizeof(Offset), 1, t->f_node))
    {
        fseek(t->f_node, node + sizeof(Node1) - 2 * sizeof(Offset), SEEK_SET);
        fread(&item_offset, sizeof(Offset), 1, t->f_node);
        do {
            fseek(t->f_item, item_offset, SEEK_SET);
            fread(item, sizeof(Element), 1, t->f_item);
            table_insert(subt, key, item->key2, &item->info);
            item_offset = item->next_offset;
        } while (item_offset != -1);
    }

    free(item);
    return subt;
}


Table* table_get_subt_by_key2(Table* t, KeyType2 key, const char* dir)
{
    Element* item = malloc(sizeof(Element));
    if (!item)
        return NULL;

    List* ks_list = table_get_ks_by_key2(t, key);
    Offset ks_offset;
    Offset item_offset = -1;

    Table* subt = table_create(dir, ks_list->length + 2);
    if ((long) subt < (1<<16))
    {
        free(item);
        list_delete(ks_list, false);
        return subt;
    }

    while ((ks_offset = list_pop(ks_list, 0)) != -1)
    {
        fseek(t->f_ks2, ks_offset + sizeof(KeySpace2)-sizeof(Offset), SEEK_SET);
        fread(&item_offset, sizeof(Offset), 1, t->f_ks2);
        do {
            fseek(t->f_item, item_offset, SEEK_SET);
            fread(item, sizeof(Element), 1, t->f_item);
            table_insert(subt, item->key1, key, &item->info);
            item_offset = item->next_offset;
        } while (item_offset != -1);
    }

    free(item);
    list_delete(ks_list, false);
    return subt;
}


int table_rm_elem(Table* t, KeyType1* key1, KeyType2 key2, int version)
{
    Element* item = malloc(sizeof(Element));
    if (!item)
        return ERROR_SOMETHING_BROKEN;
    List* ks2_list = table_get_ks_by_key2(t, key2);
    Offset ks1 = table_list_search(t, key1);

    if (ks1 == -1 || !ks2_list || !ks2_list->length)
    {
        list_delete(ks2_list, false);
        return ERROR_ELEM_NOT_FOUND;    // no elements with that key1 or key2
    }

    Offset ks2;
    Offset ks2_item;
    Offset item_offset;

    Offset item_prev = -1;
    Offset item_next;
    Offset node;
    Offset node_item;

    while ((ks2 = list_pop(ks2_list, 0)) != -1)
    {
        fseek(t->f_ks2, ks2 + sizeof(KeySpace2)-sizeof(Offset), SEEK_SET);
        fread(&item_offset, sizeof(Offset), 1, t->f_ks2);
        fseek(t->f_item, item_offset, SEEK_SET);
        fread(item, sizeof(Element), 1, t->f_item);
        if (!strcmp(item->key1, key1))
        {
            list_delete(ks2_list, false);
            item_prev = -1;
            while (item_offset != -1)
            {
                fseek(t->f_item, item_offset, SEEK_SET);
                fread(item, sizeof(Element), 1, t->f_item);
                item_next = item->next_offset;
                if (item->release == version || version == -1)
                {
                    fseek(t->f_ks2, ks2 + sizeof(KeySpace2)-sizeof(Offset), SEEK_SET);
                    fread(&ks2_item, sizeof(Offset), 1, t->f_ks2);
                    if (ks2_item == item_offset)
                    {
                        fseek(t->f_ks2, ks2 + sizeof(KeySpace2)-sizeof(Offset), SEEK_SET);
                        fwrite(&item_next, sizeof(Offset), 1, t->f_ks2);
                    }

                    fseek(t->f_ks1, ks1 + sizeof(KeySpace1)-sizeof(Offset), SEEK_SET);
                    fread(&node, sizeof(Offset), 1, t->f_ks1);
                    for (; node != -1;
                        fseek(t->f_node, node + sizeof(Node1)-sizeof(Offset), SEEK_SET),
                        fread(&node, sizeof(Offset), 1, t->f_node))
                    {
                        fseek(t->f_node, node + sizeof(Node1)-2*sizeof(Offset), SEEK_SET);
                        fread(&node_item, sizeof(Offset), 1, t->f_node);
                        if (node_item == item_offset)
                        {
                            node_item = item_next;
                            fseek(t->f_node, node + sizeof(Node1)-2*sizeof(Offset), SEEK_SET);
                            fwrite(&node_item, sizeof(Offset), 1, t->f_node);
                        }
                    }

                    if (item_prev != -1)
                    {
                        fseek(t->f_item, item_prev + sizeof(Element)-sizeof(Offset), SEEK_SET);
                        fwrite(&item_next, sizeof(Offset), 1, t->f_item);
                    }

                    if (version != -1)
                        break;
                }
                else
                    item_prev = item_offset;
                item_offset = item_next;
            }

            _table_cleanup_ks1(t, ks1, -1, false);

            fseek(t->f_ks2, ks2 + sizeof(KeySpace2)-sizeof(Offset), SEEK_SET);
            fread(&ks2_item, sizeof(Offset), 1, t->f_ks2);
            if (ks2_item == -1)
            {
                fseek(t->f_ks2, ks2, SEEK_SET);
                int deleted = DELETED;
                fwrite(&deleted, sizeof(int), 1, t->f_ks2);
                t->csize2--;
                _table_write_params(t);
            }
            free(item);
            return SUCCESS;
        }
    }

    free(item);
    list_delete(ks2_list, false);
    return ERROR_ELEM_NOT_FOUND;
}


int table_rm_key1(Table* t, KeyType1* key1)
{
    //  key1 -> node -> items -> keys2 -> get_ks2 -> deleted
    int removed = 0;

    Offset ks1 = 0;
    Offset eof;
    Offset ks2;
    Offset ks2_item;
    int ks2_busy;

    Offset node;
    Offset item_offset;
    Element* item = malloc(sizeof(Element));
    if (!item)
        return 0;

    KeyType1 empty[KEY_LENGTH] = {'\0'};
    KeyType1 keybuf[KEY_LENGTH];
    KeyType2 key2;
    List* ks2_list;

    fseek(t->f_ks1, 0, SEEK_END);
    eof = ftell(t->f_ks1);

    // clear KS1 & KS2
    while (ks1 < eof)
    {
        fseek(t->f_ks1, ks1, SEEK_SET);
        fread(keybuf, sizeof(KeyType1), KEY_LENGTH, t->f_ks1);
        if (!strcmp(key1, keybuf))
        {
            fread(&node, sizeof(Offset), 1, t->f_ks1);
            while (node != -1)
            {
                fseek(t->f_node, node + sizeof(Node1)-2*sizeof(Offset), SEEK_SET);
                fread(&item_offset, sizeof(Offset), 1, t->f_node);
                while (item_offset != -1)
                {
                    // free (item)
                    fseek(t->f_item, item_offset, SEEK_SET);
                    fread(item, sizeof(Element), 1, t->f_item);
                    key2 = item->key2;
                    ks2_list = table_get_ks_by_key2(t, key2);
                    while ((ks2 = list_pop(ks2_list, 0)) != -1)
                    {
                        fseek(t->f_ks2, ks2, SEEK_SET);
                        fread(&ks2_busy, sizeof(int), 1, t->f_ks2);
                        fseek(t->f_ks2, sizeof(KeySpace2)-sizeof(int)-sizeof(Offset), SEEK_CUR);
                        fread(&ks2_item, sizeof(Offset), 1, t->f_ks2);
                        if (ks2_item == item_offset && ks2_busy == BUSY)
                        {
                            ks2_busy = DELETED;
                            fseek(t->f_ks2, ks2, SEEK_SET);
                            fwrite(&ks2_busy, sizeof(int), 1, t->f_ks2);
                            t->csize2--;
                            _table_write_params(t);
                            break;
                        }
                    }

                    list_delete(ks2_list, false);
                    item_offset = item->next_offset;
                    removed++;
                }
                fseek(t->f_node, node + sizeof(Node1)-sizeof(Offset), SEEK_SET);
                fread(&node, sizeof(Offset), 1, t->f_node);
            }

            fseek(t->f_ks1, ks1, SEEK_SET);
            fwrite(empty, sizeof(KeyType1), KEY_LENGTH, t->f_ks1);

            t->csize1--;
            _table_write_params(t);

            free(item);
            return removed;
        }
        ks1 += sizeof(KeySpace1);
    }
    free(item);
    return removed;
}


int table_rm_key2(Table* t, KeyType2 key2)
{
    // key2 -> get_ks2 -> keys1 -> search for key1
    int removed = 0;

    List* ks2_list = table_get_ks_by_key2(t, key2);

    Offset ks1;
    Offset ks2;
    Offset eof;

    int deleted = DELETED;
    Offset item_offset;

    Element* item = malloc(sizeof(Element));
    if (!item)
        return 0;

    fseek(t->f_ks1, 0, SEEK_END);
    eof = ftell(t->f_ks1);

    while ((ks2 = list_pop(ks2_list, 0)) != -1)
    {
        fseek(t->f_ks2, ks2, SEEK_SET);
        fwrite(&deleted, sizeof(int), 1, t->f_ks2);
        t->csize2--;
        fseek(t->f_ks2, sizeof(KeySpace2)-sizeof(int)-sizeof(Offset), SEEK_SET);
        fread(&item_offset, sizeof(Offset), 1, t->f_ks2);
        while (item_offset != -1)
        {
            fseek(t->f_item, item_offset, SEEK_SET);
            fread(item, sizeof(Element), 1, t->f_item);

            for (ks1 = 0; ks1 < eof; ks1 += sizeof(KeySpace1))
                _table_cleanup_ks1(t, ks1, item_offset, true);

            item_offset = item->next_offset;
            removed++;
        }
    }

    free(item);
    list_delete(ks2_list, false);
    _table_write_params(t);
    return removed;
}


void _table_cleanup_ks1(Table* t, Offset ks1, Offset item, bool multi_node)
{
    Offset ks1_node;
    Offset node_offset;
    Offset node_prev = -1;
    Offset node_next;
    Node1 node;

    fseek(t->f_ks1, ks1 + KEY_LENGTH*sizeof(KeyType1), SEEK_SET);
    fread(&node_offset, sizeof(Offset), 1, t->f_ks1);

    while (node_offset != -1)
    {
        fseek(t->f_node, node_offset, SEEK_SET);
        fread(&node.release, sizeof(int), 2, t->f_node);
        fread(&node.item_offset, sizeof(Offset), 1, t->f_node);
        fread(&node.next_offset, sizeof(Offset), 1, t->f_node);
        node_next = node.next_offset;
        if (node.item_offset == item)
        {
            // free (node)
            fseek(t->f_ks1, ks1 + KEY_LENGTH*sizeof(KeyType1), SEEK_SET);
            fread(&ks1_node, sizeof(Offset), 1, t->f_ks1);
            if (ks1_node == node_offset)
            {
                ks1_node = node_next;
                fseek(t->f_ks1, ks1 + KEY_LENGTH*sizeof(KeyType1), SEEK_SET);
                fwrite(&node_next, sizeof(Offset), 1, t->f_ks1);
            }

            if (node_prev != -1)
            {
                fseek(t->f_node, node_prev + sizeof(Node1)-sizeof(Offset), SEEK_SET);
                fwrite(&node_next, sizeof(Offset), 1, t->f_node);
            }

            if (!multi_node)
                break;
        }
        else
            node_prev = node_offset;
        node_offset = node_next;
    }

    // now if ks1.node is NULL, delete ks1 (make unreachable)
    fseek(t->f_ks1, ks1 + KEY_LENGTH*sizeof(KeyType1), SEEK_SET);
    fread(&ks1_node, sizeof(Offset), 1, t->f_ks1);
    if (ks1_node == -1)
    {
        KeyType1 empty[KEY_LENGTH] = {'\0'};
        fseek(t->f_ks1, ks1, SEEK_SET);
        fwrite(empty, KEY_LENGTH*sizeof(KeyType1), 1, t->f_ks1);
        t->csize1--;
        _table_write_params(t);
    }
}

#pragma clang diagnostic pop
