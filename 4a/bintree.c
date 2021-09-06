#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#include "bintree.h"


Node* bintree_create()
{
    // what is an empty tree? given 30 seconds to think
    return NULL;
}


Node* bintree_find(Node* t, KeyType key, int version)
{
    Node* ptr = t;
    int curr_ver = 0;
    while (ptr)
    {
        if (ptr->key == key && curr_ver++ == version)
            return ptr;
        else if (key <= ptr->key)
            ptr = ptr->left;
        else
            ptr = ptr->right;
    }
    return NULL;
}


Node* bintree_min(Node* t, int version)
{
    Node* ptr = t;
    int curr_ver = 0;

    if (ptr)
    {
        while (ptr->left)
        {
            if (ptr->key == ptr->left->key)
            {
                if (curr_ver++ == version)
                    return ptr;
            }
            else curr_ver = 0;
            ptr = ptr->left;
        }
        return ptr;
    }
    return NULL;
}


Node* bintree_max(Node* t, int version)
{
    Node* ptr = t;
    int curr_ver = 0;

    if (ptr)
    {
        while (ptr->right)
        {
            if (ptr->key == ptr->right->key)
            {
                if (curr_ver++ == version)
                    return ptr;
            }
            else curr_ver = 0;
            ptr = ptr->right;
        }
        return ptr;
    }
    return NULL;
}


Node* _bintree_findnext(Node* t)
{
    if (t->right)
        return bintree_min(t->right, INT_MAX);

    Node* ptr;
    ptr = t->master;
    while (ptr && t == ptr->right)
    {
        t = ptr;
        ptr = t->master;
    }
    return ptr;
}


Node* _bintree_findprev(Node* t)
{
    if (t->left)
        return bintree_max(t->left, INT_MAX);

    Node* ptr;
    ptr = t->master;
    while (ptr && t == ptr->left)
    {
        t = ptr;
        ptr = t->master;
    }
    return ptr;
}


int bintree_add(Node** t, KeyType key, InfoType* info)
{
    if (*t)
    {
        Node* target_node = _bintree_findtarget(*t, key);
        return _bintree_attach(target_node, key, info);
    }
    else
    {
        Node* root = malloc(sizeof(Node));
        if (!root)
            return ERROR_MEM_FULL;

        root->info = info;
        root->key = key;
        root->left = NULL;
        root->right = NULL;
        root->next = NULL;
        root->master = NULL;

        *t = root;
    }
    return SUCCESS;
}


Node* _bintree_findtarget(Node* t, KeyType key)
{
    Node* ptr = t;
    Node* par = NULL;

    while (ptr)
    {
        par = ptr;
        if (key <= ptr->key)
            ptr = ptr->left;
        else
            ptr = ptr->right;
    }
    return par;
}


int _bintree_attach(Node* t, KeyType key, InfoType* info)
{
    Node* prev, *new_node = malloc(sizeof(Node));
    if (!new_node)
        return ERROR_MEM_FULL;

    new_node->info = info;
    new_node->key = key;
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->master = t;

    if (key <= t->key)
        t->left = new_node;
    else
        t->right = new_node;

    new_node->next = _bintree_findnext(new_node);
    prev = _bintree_findprev(new_node);
    if (prev) prev->next = new_node;

    return SUCCESS;
}


int bintree_rm(Node** t, KeyType key, int version)
{
    Node* x = bintree_find(*t, key, version);
    if (!x)
        return ERROR_ELEM_NOT_FOUND;
    Node* y;
    if (!x->left || !x->right)
        y = x;
    else
        y = _bintree_findnext(x);

    Node* p, *prev = _bintree_findprev(y);
    Node* par;

    if (y->left)
        p = y->left;
    else
        p = y->right;
    par = y->master;

    if (p)                      // if there's a subtree, reattach master
        p->master = par;
    if (!par)                   // if it was root, redefine it
        *t = p;
    else if (par->left == y)    // reattach subtree to master
        par->left = p;
    else // par->right == y
        par->right = p;

    if (y != x)                 // data transfer
    {
        free(x->info->str);
        free(x->info);
        x->key = y->key;
        x->info = y->info;
    }
    else
    {
        free(y->info->str);
        free(y->info);
    }

    if (prev)
        prev->next = _bintree_findnext(prev);

    free(y);
    return SUCCESS;
}


void bintree_print_range(Node* t, KeyType key_min, KeyType key_max)
{
    printf("Elements [%u - %u]\n", key_min, key_max);
    if (key_min > key_max)
        return;

    Node* ptr = _bintree_findtarget(t, key_min);
    while (ptr && ptr->key < key_min)
        ptr = ptr->next;
    while (ptr && ptr->key <= key_max)
    {
        printf("    [%u]:\t%16f\t%s\n", ptr->key, ptr->info->f1, ptr->info->str);
        ptr = ptr->next;
    }
}


void bintree_print(Node* t)
{
    printf("[Tree view]\n");
    if (t)
        _bintree_print_r(t, 0);
}


void _bintree_print_r(Node* t, int intend)
{
    int i;
    for (i = 0; i < intend; i++) printf("  ");
    printf("[%u]:  %f  %s\n", t->key, t->info->f1, t->info->str);

    if (t->left && t->right)
    {
        _bintree_print_r(t->left, intend+1);
        _bintree_print_r(t->right, intend+1);
    }
    else if (t->left)
    {
        _bintree_print_r(t->left, intend+1);
        for (i = 0; i < intend+1; i++) printf("  ");
        puts("[ ]");
    }
    else if (t->right)
    {
        for (i = 0; i < intend+1; i++) printf("  ");
        puts("[ ]");
        _bintree_print_r(t->right, intend+1);
    }
}


int bintree_save(Node* t, char* filename)
{
    FILE* f = fopen(filename, "w");
    if (!f)
        return ERROR_FILE_OPEN;

    _bintree_write(t, f);

    fclose(f);
    return SUCCESS;
}

void _bintree_write(Node* ptr, FILE* f)
{
    if (ptr)
    {
        fprintf(f, "%u\n%f %s\n", ptr->key, ptr->info->f1, ptr->info->str);
        if (ptr->left)
            _bintree_write(ptr->left, f);
        if (ptr->right)
            _bintree_write(ptr->right, f);
    }
}


Node* bintree_load(char* filename)
{
    FILE* f = fopen(filename, "r");
    if (!f)
        return (Node*) ERROR_FILE_OPEN;

    Node* t = bintree_create();
    KeyType key;
    InfoType* info;
    float f1;
    char* str = malloc(INFO_LENGTH);

    while (!feof(f))
    {
        if (fscanf(f, "%u", &key) == EOF ||
            fscanf(f, "%f", &f1) == EOF  ||
            fscanf(f, "%*c%63s", str) == EOF)
            break;

        info = malloc(sizeof(InfoType));
        info->f1 = f1;
        info->str = strdup(str);

        bintree_add(&t, key, info);
    }

    free(str);
    return t;
}


void bintree_delete(Node* t)
{
    Node *ptr, *next;
    ptr = bintree_min(t, INT_MAX);

    while (ptr)
    {
        next = ptr->next;
        free(ptr->info->str);
        free(ptr->info);
        free(ptr);
        ptr = next;
    }
}
