/*
 *      K-d tree data structure
 *
 *      task variant dependant:
 *          KeyType     -   type of keys used in trees
 *          InfoType    -   associated data type
 *          K           -   number of dimensions
 *          N           -   max number of keys stored in each leaf
 *
 *      types described:
 *          Node        -   main type used for all tree nodes
 *          Element     -   a record stored in a tree leaf
 *
 *      functions:
 *          create()    -   returns an empty tree
 *          load()      -   loads tree from file
 *          newnode()   -   used for creating nodes
 *
 *      methods:
 *          add()
 *          rm()
 *          find()
 *          max()
 *          maxnode()
 *          farthest()
 *          list_range()
 *          print_range()
 *          save()
 *          export()
 *          delete()
 */

#pragma clang diagnostic push
#pragma ide diagnostic ignored "DanglingPointers"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kdtree.h"

#ifndef INFO_LENGTH
#define INFO_LENGTH 64
#endif


// ---------------------------------------------- extern functions -------------------------------------------------- //

bool kdtree_samepoints(const KeyType p1[], const KeyType p2[])
{
    int i;
    for (i = 0; i < K; i++)
        if (p1[i] != p2[i])
            return false;
    return true;
}


Node* kdtree_create()
{
    // what is an empty tree? given 30 seconds to think
    return NULL;
}


Node* kdtree_newnode(KeyType bound, int dir, Node* master, Element* el)
{
    Node* new_node = malloc(sizeof(Node));
    if (!new_node)
        return NULL;

    new_node->bound = bound;
    new_node->dir = dir;
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->master = master;
    // initialize "next" after connecting the node?
    if (el)
    {
        new_node->elements = list();
        list_append(new_node->elements, el);
    }
    else new_node->elements = NULL;

    return new_node;
}


int kdtree_add(Node** t, const KeyType point[], InfoType* info)
{
    return kdtree_insert_r(t, point, NULL, info, 0);
}


Element* kdtree_find(Node* t, const KeyType point[], int version)
{
    Node* target = kdtree_find_r(t, point, 0);
    if (!target) return NULL;

    int curr_ver = 0;
    Item* i;
    for (i = target->elements->head; i; i = i->next)
        if (kdtree_samepoints(point, ((Element*) i->data)->point) && curr_ver++ == version)
            return i->data;

    return NULL;
}


void kdtree_maxnode(Node* t, int cd, KeyType* max_key, Node** ans)
{
    Element* max_el;

    if (!t) return;
    if (t->elements)
    {
        max_el = kdtree_list_max(t->elements, cd, 0);
        if (max_el && max_el->point[cd] >= *max_key)
        {
            *max_key = max_el->point[cd];
            *ans = t;
            return;
        }
    }

    if (t->right || t->left)
    {
        if (t->right && t->dir == cd)
            kdtree_maxnode(t->right, cd, max_key, ans);
        else
        {
            if (t->left)
                kdtree_maxnode(t->left, cd, max_key, ans);
            if (t->right)
                kdtree_maxnode(t->right, cd, max_key, ans);
        }
    }
}


Element* kdtree_max(Node* t, int cd, int version)
{
    Node* target = NULL;
    KeyType max_key = INT_MIN;
    kdtree_maxnode(t, cd, &max_key, &target);
    if (!target || !target->elements) return NULL;

    return kdtree_list_max(target->elements, cd, version);
}


Element* kdtree_farthest(Node* t)
{
    if (!t) return NULL;
    
    Element* farthest = NULL;
    long dist2 = -1;

    kdtree_farthest_r(t, &dist2, &farthest);
    
    return farthest;
}


void kdtree_list_range(Node* t, const KeyType point_max[], List* buf)
{
    Item* i;
    Element* el;

    if (!t) return;

    if (t->elements)
    {
        for (i = t->elements->head; i; i = i->next)
        {
            el = i->data;
            if (el->point[0] <= point_max[0] && el->point[1] <= point_max[1])
                list_push(buf, el);
        }
        return;
    }

    if (t->bound < point_max[t->dir])
        kdtree_list_range(t->right, point_max, buf);
    kdtree_list_range(t->left, point_max, buf);
}


void kdtree_print_range(Node* t, const KeyType point_max[], int cd)
{
    Element* el;
    List* buf = list();

    kdtree_list_range(t, point_max, buf);
    kdtree_list_sort(buf, cd, true);

    while ((el = list_pop(buf, 0)))
        printf("[%d %d]:  (%f %f %s)\n", el->point[0], el->point[1], el->info->f1, el->info->f2, el->info->str);

    list_delete(buf, false);
}


int kdtree_rm(Node** t, const KeyType point[], int version)
{
    Node* ptr = kdtree_find_r(*t, point, 0);
    if (!ptr)
        return ERROR_ELEM_NOT_FOUND;

    Element* el = kdtree_find(ptr, point, version);
    if (!el)
        return ERROR_ELEM_NOT_FOUND;

    free(el->info->str);
    free(el->info);
    free(el);

    int i;
    for (i = 0; i < ptr->elements->length; i++)
        if (list_get(ptr->elements, i) == el)
        {
            list_pop(ptr->elements, i);
            break;
        }

    if (!ptr->elements->length)
    {
        Node* master;
        while (ptr->master && ptr->master->left->elements && ptr->master->right->elements &&
                             !ptr->master->left->elements->length && !ptr->master->right->elements->length)
        {
            master = ptr->master;
            list_delete(master->left->elements, false);
            list_delete(master->right->elements, false);
            free(master->left);
            free(master->right);
            master->elements = list();
            ptr = master;
        }
        if (!ptr->master && ptr->elements && !ptr->elements->length)
        {
            list_delete(ptr->elements, false);
            free(ptr);
            *t = NULL;
        }
    }
    return SUCCESS;
}


void kdtree_print(Node* t)
{
    printf("[Tree view]\n");
    if (t)
        kdtree_print_r(t, 0);
}


int kdtree_save(Node* t, const char* filename)
{
    FILE* f = fopen(filename, "w");
    if (!f)
        return ERROR_FILE_OPEN;

    kdtree_write_r(t, f);

    fclose(f);
    return SUCCESS;
}


Node* kdtree_load(const char* file)
{
    FILE* f = fopen(file, "r");
    if (!f)
        return (Node*) ERROR_FILE_OPEN;

    Node* t = kdtree_create();
    KeyType point[K];
    InfoType* info;

    float f1, f2;
    char* str = malloc(INFO_LENGTH);
    if (!str) { fclose(f); return (Node*) ERROR_MEM_FULL; }

    while (!feof(f))
    {
        if (fscanf(f, "%d%d", point, point+1) == EOF ||
            fscanf(f, "%f%f%s", &f1, &f2, str) == EOF)
            break;

        info = malloc(sizeof(InfoType));
        if (!info) { fclose(f); free(str); return (Node*) ERROR_MEM_FULL; }

        info->f1 = f1;
        info->f2 = f2;
        info->str = strdup(str);

        kdtree_add(&t, point, info);
    }

    free(str);
    fclose(f);
    return t;
}


int kdtree_export(Node* t, const char* filename)
{
    FILE* f = fopen(filename, "w");
    if (!f)
        return ERROR_FILE_OPEN;

    fprintf(f, "{\n");
    kdtree_export_r(t, 1, f);
    fprintf(f, "}\n");
    fclose(f);

    return SUCCESS;
}


void kdtree_delete(Node* t)
{
    if (!t) return;

    if (t->elements)
    {
        Element* el;
        while ((el = list_pop(t->elements, 0)))
        {
            free(el->info->str);
            free(el->info);
            free(el);
        }
        list_delete(t->elements, false);
    }

    if (t->left) kdtree_delete(t->left);
    if (t->right) kdtree_delete(t->right);

    free(t);
}

// ------------------------------------------------------------------------------------------------------------------ //



// ----------------------------------------------- internal utils --------------------------------------------------- //

static void kdtree_print_r(Node* t, int intend)
{
    int i;
    Item* item;
    List* elements = t->elements;
    for (i = 0; i < intend; i++) printf("  ");

    if (t->dir == 0)
        printf("[%d *]", t->bound);
    else if (t->dir == 1)
        printf("[* %d]", t->bound);
    else
        printf("[*]");

    if (elements)
    {
        putchar(':');
        for (item = elements->head; item; item = item->next)
            printf(" ((%d, %d), %f, %f, %s)", ((Element*) item->data)->point[0],((Element*) item->data)->point[1],
                   ((Element*) item->data)->info->f1, ((Element*) item->data)->info->f2,
                   ((Element*) item->data)->info->str);
    }
    putchar('\n');

    if (t->left && t->right)
    {
        kdtree_print_r(t->left, intend + 1);
        kdtree_print_r(t->right, intend + 1);
    }
    else if (t->left)
    {
        kdtree_print_r(t->left, intend + 1);
        for (i = 0; i < intend+1; i++) printf("  ");
        puts("[ ]");
    }
    else if (t->right)
    {
        for (i = 0; i < intend+1; i++) printf("  ");
        puts("[ ]");
        kdtree_print_r(t->right, intend + 1);
    }
}


static void kdtree_export_r(Node* t, int intend, FILE* f)
{
    int i;
    Item* item;
    List* elements = t->elements;

    for (i = 0; i < intend; i++) fprintf(f, "  ");
    fprintf(f, "(%d, %d): ", t->bound, t->dir);

    if (elements)
    {
        fprintf(f, "[");
        for (item = elements->head; item; item = item->next)
            fprintf(f, "((%d, %d), %f, %f, \"%s\"), ",
                    ((Element*) item->data)->point[0], ((Element*) item->data)->point[1],
                    ((Element*) item->data)->info->f1, ((Element*) item->data)->info->f2, ((Element*) item->data)->info->str);
        fprintf(f, "],");
    }
    fprintf(f, "\n");

    if (t->left || t->right)
    {
        for (i = 0; i < intend; i++) fprintf(f, "  ");
        fprintf(f, "{\n");
    }

    if (t->left && t->right)
    {
        kdtree_export_r(t->left, intend + 1, f);
        kdtree_export_r(t->right, intend + 1, f);
    }
    else if (t->left)
    {
        kdtree_export_r(t->left, intend + 1, f);
        for (i = 0; i < intend+1; i++) fprintf(f, "  ");
        fprintf(f, "(%d, -1): None, \n", i);
    }
    else if (t->right)
    {
        for (i = 0; i < intend+1; i++) fprintf(f, "  ");
        fprintf(f, "(%d, -1): None, \n", i);
        kdtree_export_r(t->right, intend + 1, f);
    }

    if (t->left || t->right)
    {
        for (i = 0; i < intend; i++) fprintf(f, "  ");
        fputs("},\n", f);
    }

}


static bool metrics0(Element* a, Element* b)
{
    return a->point[0] > b->point[0];
}


static bool metrics1(Element* a, Element* b)
{
    return a->point[1] > b->point[1];
}


static void kdtree_list_sort(List* l, int cd, bool reversed)
{
    if (cd) list_sort(l, &metrics1, reversed);
    else    list_sort(l, &metrics0, reversed);
}


static int kdtree_list_insert(List* l, Element* el, int cd)
{
    // assuming list is ready-sorted
    Item* i, *next;

    if (!l->head || ((Element*) l->head->data)->point[cd] > el->point[cd])
    {
        list_push(l, el);
        return SUCCESS;
    }

    for (i = l->head; i->next; i = i->next)
        if (((Element*) i->next->data)->point[cd] > el->point[cd])
        {
            next = i->next;
            i->next = malloc(sizeof(Item));
            if (!i->next) return ERROR_MEM_FULL;

            i->next->data = el;
            i->next->next = next;
            l->length++;
            return SUCCESS;
        }

    list_append(l, el);
    return SUCCESS;
}


static int kdtree_split(Node* t, int index)
{
    Item* i;
    Item* j;

    t->left = kdtree_newnode(0, -1, t, NULL);
    t->right = kdtree_newnode(1, -1, t, NULL);
    if (!t->left || !t->right) return ERROR_MEM_FULL;

    i = list_getitem(t->elements, index);
    for (; i->next && kdtree_samepoints(((Element*) i->data)->point,((Element*) i->next->data)->point); i = i->next, index++);

    j = i->next;
    i->next = NULL;

    List* r_elem = list();
    if (!r_elem)
    {
        free(t->left);
        free(t->right);
        return ERROR_MEM_FULL;
    }

    r_elem->length = t->elements->length - (index+1);
    r_elem->head = j;
    r_elem->tail = t->elements->tail;
    t->elements->length = index+1;
    t->elements->tail = i;

    t->left->elements = t->elements;
    t->right->elements = r_elem;
    t->elements = NULL;

    kdtree_list_sort(t->left->elements, (t->dir + K - 1) % K, false);
    kdtree_list_sort(t->right->elements, (t->dir + K - 1) % K, false);

    return SUCCESS;
}


static int kdtree_insert_r(Node** t, const KeyType *point, Node* master, InfoType* info, int depth)
{
    int cd = depth % K;
    int cd_prev = (cd+K-1)%K;

    // base case
    if (!*t)
    {
        Element* el = malloc(sizeof(Element));
        if (!el)
            return ERROR_MEM_FULL;

        memcpy(el->point, point, K*sizeof(KeyType));
        el->info = info;

        if (!master) // insert into root
        {
            *t = kdtree_newnode(0, -1, NULL, el);
            if (!*t) return ERROR_MEM_FULL;
            return SUCCESS;
        }

        if (master->elements->length < N)  // we can just insert it into the current list
            return kdtree_list_insert(master->elements, el, cd_prev);

        // list is full now, gotta do something about that
        int median_index = (N-1) / 2;
        master->bound = ((Element*) list_get(master->elements, median_index))->point[cd_prev];
        master->dir = cd_prev;
        if (kdtree_split(master, median_index) != SUCCESS)
        {
            free(el);
            return ERROR_MEM_FULL;
        }
        if (point[(cd+K-1)%K] <= master->bound)
            return kdtree_list_insert(master->left->elements, el, cd);
        return kdtree_list_insert(master->right->elements, el, cd);
    }
    // recursive case
    if (point[cd] <= (*t)->bound)
        return kdtree_insert_r(&(*t)->left, point, *t, info, depth + 1);
    return kdtree_insert_r(&(*t)->right, point, *t, info, depth + 1);
}


static Node* kdtree_find_r(Node* t, const KeyType *point, int depth)
{
    if (!t) return NULL;
    if (t->elements) return t;

    int cd = depth % K;
    if (point[cd] <= t->bound)
        return kdtree_find_r(t->left, point, depth + 1);
    return kdtree_find_r(t->right, point, depth + 1);
}


static Element* kdtree_list_max(List* l, int cd, int version)
{
    KeyType max_key = INT_MIN;
    List* max_elems = list();
    Element* ans;

    Item* i;
    for (i = l->head; i; i = i->next)
    {
        if (((Element*) i->data)->point[cd] > max_key)
        {
            list_delete(max_elems, false);
            max_elems = list();
        }
        if (((Element*) i->data)->point[cd] >= max_key)
        {
            list_append(max_elems, i->data);
            max_key = ((Element*) i->data)->point[cd];
        }
    }

    ans = list_get(max_elems, version);
    list_delete(max_elems, false);

    return ans;
}


#define DIST2(a, b) ((long)(a)*(a) + (long)(b)*(b))
static void kdtree_farthest_r(Node* t, long* dist2, Element** ans)
{
    Item* i;
    Element* el;
    long curr_dist2;

    if (!t) return;
    if (t->elements)
        for (i = t->elements->head; i; i = i->next)
        {
            el = i->data;
            if ((curr_dist2 = DIST2(el->point[0], el->point[1])) > *dist2)
            {
                *dist2 = curr_dist2;
                *ans = el;
            }
        }
    else
    {
        kdtree_farthest_r(t->left, dist2, ans);
        kdtree_farthest_r(t->right, dist2, ans);
    }
}


static void kdtree_write_r(Node* t, FILE* f)
{
    Item* i;
    Element* el;

    if (!t) return;

    if (t->elements)
        for (i = t->elements->head; i; i = i->next )
        {
            el = i->data;
            fprintf(f, "%d %d\t%f %f %s\n", el->point[0], el->point[1], el->info->f1, el->info->f2, el->info->str);
        }
    else
    {
        kdtree_write_r(t->left, f);
        kdtree_write_r(t->right, f);
    }
}

// ------------------------------------------------------------------------------------------------------------------ //

#pragma clang diagnostic pop
