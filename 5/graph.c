/*
 *      graph data structure
 *
 *      types described:
 *          KeyType  -  string (char*)
 *          Graph  -  represented by table with nodes as data
 *          Node  -  contains point (x,y) and adjacency list
 *          Edge  -  contains edge weight and adjacent key
 *
 *      functions:
 *          create()
 *          load()
 *          random()
 *
 *      methods:
 *          add_node()
 *          add_edge()
 *          rm_node()
 *          rm_edge()
 *          get()
 *          path_bfs()
 *          path_bellmanford()
 *          path_tree()
 *          print()
 *          save()
 *          export()
 *          delete()
 */

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

#include "list.h"
#include "graph.h"


#ifndef KEY_LENGTH
#define KEY_LENGTH 64
#endif

#ifndef RAND_POINT_MAX
#define RAND_POINT_MAX 100000.0
#endif

#define VISITED ((void*) 1)


// ----------------------------------------------- internal utils --------------------------------------------------- //

static KeyType* graph_rand_key(size_t len)
{
    KeyType* buf = malloc((len+1)*sizeof(KeyType));
    if (!buf) return NULL;

    size_t i;
    for (i = 0; i < len; i++)
        buf[i] = 'a' + (KeyType) (rand() % 26);
    buf[len] = '\0';

    return buf;
}

static int graph_queue_insert(List* l, Pair* el)
{
    // assuming list is ready-sorted
    Item* i, *next;

    if (!l->head || ((Pair*) l->head->data)->first > el->first)
    {
        list_push(l, el);
        return SUCCESS;
    }

    for (i = l->head; i->next; i = i->next)
        if (((Pair*) i->next->data)->first > el->first)
        {
            next = i->next;
            i->next = malloc(sizeof(Item));
            if (!i->next) return ERROR_MEM_ALLOC;

            i->next->data = el;
            i->next->next = next;
            l->length++;
            return SUCCESS;
        }

    list_append(l, el);
    return SUCCESS;
}

// ------------------------------------------------------------------------------------------------------------------ //



// -------------------------------------------------- functions ----------------------------------------------------- //

Graph* graph_create(size_t max_size)
{
    return table_create(max_size);
}


Graph* graph_load(const char* filename)
{
    FILE* f = fopen(filename, "r");
    if (!f) return NULL;

    size_t i, j, size, num_busy, list_size;
    WeightType weight;
    double point[K];

    KeyType key1[KEY_LENGTH];
    KeyType key2[KEY_LENGTH];

    if (fscanf(f, "%lu%lu", &size, &num_busy) == EOF)
        return NULL;

    Graph* g = graph_create(size);

    for (i = 0; i < num_busy && !feof(f); i++)
    {
        fscanf(f, "%63s%lf%lf%lu", key1, &point[0], &point[1], &list_size);
        graph_add_node(g, key1, point);
        for (j = 0; j < list_size; j++)
        {
            fscanf(f, "%d%s", &weight, key2);
            graph_add_edge(g, weight, key1, key2);
        }
    }
    fclose(f);
    return g;
}


Graph* graph_random(size_t max_size, size_t nnodes, float density)
{
    Graph* g = graph_create(max_size);
    if (!g) return NULL;

    size_t i, j, rand_index, len = KEY_LENGTH/8;
    KeyType** keys = malloc(nnodes*sizeof(KeyType*));
    if (!keys) { graph_delete(g); return NULL; }
    double point[K], rand_degree;

    for (i = 0; i < nnodes; i++)
    {
        keys[i] = graph_rand_key(len);
        if (!keys[i])
        {
            graph_delete(g);
            g = NULL;
            break;
        }
        point[0] = RAND_POINT_MAX * ((double)rand() / (double)RAND_MAX - 0.5);
        point[1] = RAND_POINT_MAX * ((double)rand() / (double)RAND_MAX - 0.5);
        graph_add_node(g, keys[i], point);
        if (i)
        {
            if (density == 1.0) rand_degree = i;
            else rand_degree = ((rand() % (2*i+1)) * density);
            for (j = 0; (float) j < rand_degree; j++)
            {
                if (density == 1.0) rand_index = j;
                else rand_index = rand() % i;
                graph_add_edge(g, rand() % 1000, keys[i], keys[rand_index]);
            }
        }
    }

    for (i = 0; i < nnodes; i++)
        free(keys[i]);
    free(keys);

    return g;
}

// ------------------------------------------------------------------------------------------------------------------ //



// -------------------------------------------------- methods ------------------------------------------------------- //

GraphErrorCode graph_add_node(Graph* g, const KeyType* key, const double point[K])
{
    if (!g) return ERROR_ELEM_NOT_FOUND;
    if (graph_get(g, key)) return ERROR_DUP_KEY;

    Node* node = malloc(sizeof(Node));
    if (!node) return ERROR_MEM_ALLOC;

    memcpy(node->point, point, K*sizeof(double));
    node->adj = list();

    GraphErrorCode err_code = table_insert(g, key, node);
    if (err_code != SUCCESS)
    {
        list_delete(node->adj, false);
        free(node);
    }

    return err_code;
}


GraphErrorCode graph_add_edge(Graph* g, WeightType weight, const KeyType* key1, const KeyType* key2)
{
    Node* node1 = graph_get(g, key1);
    Node* node2 = graph_get(g, key2);
    if (!node1 || !node2)
        return ERROR_ELEM_NOT_FOUND;

    Item* i;
    for (i = node1->adj->head; i; i = i->next)
        if (!strcmp(key2, ((Edge*) i->data)->key2))
            return ERROR_DUP_KEY;

    Edge* linker1 = malloc(sizeof(Edge));
    Edge* linker2 = malloc(sizeof(Edge));
    if (!linker1 || !linker2) return ERROR_MEM_ALLOC;

    linker1->weight = weight;
    linker2->weight = weight;

    linker1->key2 = strdup(key2);
    linker2->key2 = strdup(key1);

    list_push(node1->adj, linker1);
    list_push(node2->adj, linker2);
    return SUCCESS;
}


Node* graph_get(Graph* g, const KeyType* key)
{
    if (!g) return NULL;
    return table_get(g, key);
}


GraphErrorCode graph_rm_node(Graph* g, const KeyType* key)
{
    Node* target = graph_get(g, key);
    if (!target) return ERROR_ELEM_NOT_FOUND;

    Edge* edge;
    Node* neighbour;
    Item* i, *i_prev;
    while ((edge = list_pop(target->adj, 0)))
    {
        neighbour = graph_get(g, edge->key2);
        free(edge->key2);
        free(edge);
        for (i_prev = NULL, i = neighbour->adj->head; i; i_prev = i, i = i->next)
            if (!strcmp(key, ((Edge*) i->data)->key2))
            {
                edge = list_popnext(neighbour->adj, i_prev);
                free(edge->key2);
                free(edge);
                break;
            }
    }
    list_delete(target->adj, false);
    table_rm(g, key, true);
    return SUCCESS;
}


GraphErrorCode graph_rm_edge(Graph* g, const KeyType* key1, const KeyType* key2)
{
    if (!g) return ERROR_ELEM_NOT_FOUND;

    Node* target1 = graph_get(g, key1);
    Node* target2 = graph_get(g, key2);
    if (!target1 || !target2)
        return ERROR_ELEM_NOT_FOUND;

    Item* i, *i_prev;
    Edge* edge;
    for (i_prev = NULL, i = target1->adj->head; i; i_prev = i, i = i->next)
        if (!strcmp(key2, ((Edge*) i->data)->key2))
        {
            edge = list_popnext(target1->adj, i_prev);
            free(edge->key2);
            free(edge);
            break;
        }
    if (!i)
        return ERROR_ELEM_NOT_FOUND;

    for (i_prev = NULL, i = target2->adj->head; i; i_prev = i, i = i->next)
        if (!strcmp(key1, ((Edge*) i->data)->key2))
        {
            edge = list_popnext(target2->adj, i_prev);
            free(edge->key2);
            free(edge);
            break;
        }

    return SUCCESS;
}


Edge* graph_get_edge(Graph* g, const KeyType* key1, const KeyType* key2)
{
    if (!g) return NULL;

    Node* target1 = graph_get(g, key1);
    Node* target2 = graph_get(g, key2);
    if (!target1 || !target2)
        return NULL;

    Item* i;
    Edge* edge;
    for (i = target1->adj->head; i; i = i->next)
        if (!strcmp(key2, ((Edge*) i->data)->key2))
        {
            edge = i->data;
            break;
        }
    if (!i)
        return NULL;

    return edge;
}


List* graph_path_bfs(Graph* g, KeyType* key_init, KeyType* key_target)
{
    Node* node_init = graph_get(g, key_init);
    Node* node_target = graph_get(g, key_target);
    if (!node_init || !node_target)
        return NULL;

    if (!strcmp(key_init, key_target))
    {
        List* path = list();
        list_append(path, key_init);
        return path;
    }

    Table* visited = table_create(g->num_busy * 2);
    if (!visited) return NULL;

    List* path = list();
    list_append(path, key_init);
    List* queue = list();
    list_append(queue, path);

    List* new_path;
    KeyType* vertex;
    Item* i;
    bool found = false;

    while (queue->length)
    {
        path = list_pop(queue, 0);
        vertex = path->tail->data;
        if (!strcmp(vertex, key_target))
        {
            found = true;
            break;
        }
        if (!table_get(visited, vertex))
        {
            for (i = ((Node*) graph_get(g, vertex))->adj->head; i; i = i->next)
            {
                new_path = list_copy(path);
                list_append(new_path, ((Edge*) i->data)->key2);
                list_append(queue, new_path);
            }
            table_insert(visited, vertex, VISITED);
        }
        list_delete(path, false);
    }
    if (!found)
        path = NULL;

    table_delete(visited, false);

    while ((new_path = list_pop(queue, 0)))
        list_delete(new_path, false);
    list_delete(queue, false);

    return path;
}


List* graph_path_bellmanford(Graph* g, KeyType* key_init, KeyType* key_target)
{
    Node* node_init = graph_get(g, key_init);
    Node* node_target = graph_get(g, key_target);
    if (!node_init || !node_target)
        return NULL;

    if (!strcmp(key_init, key_target))
    {
        List* path = list();
        list_append(path, key_init);
        list_push(path, 0);
        return path;
    }

    Table* p = table_create(g->num_busy*2);
    Table* d = table_create(g->size);
    if (!p || !d) return NULL;

    KeyType* key;
    Item *i, *j;
    Edge* edge;
    size_t pos;
    bool any;

    List* busy_cells = list();
    for (pos = 0; pos < g->size; pos++)
        if (g->p[pos].busy != CELL_FREE)
        {
            d->p[pos].busy = g->p[pos].busy;
            if (g->p[pos].busy == CELL_BUSY)
            {
                d->p[pos].key = strdup(g->p[pos].key);
                d->p[pos].data = (void*) LONG_MAX;
                d->num_busy++;
                list_push(busy_cells, (void*) pos);
            }
        }
    table_update(d, key_init, 0);

    while (true)
    {
        any = false;
        for (i = busy_cells->head; i; i = i->next)
            if ((size_t) d->p[(size_t)i->data].data < LONG_MAX)
                for (j = ((Node*) g->p[(size_t)i->data].data)->adj->head; j; j = j->next)
                {
                    edge = j->data;
                    if ((size_t) table_get(d, edge->key2) > (size_t) d->p[(size_t)i->data].data + edge->weight)
                    {
                        table_update(d, edge->key2, (void*) d->p[(size_t)i->data].data + edge->weight);
                        if (table_update(p, edge->key2, g->p[(size_t)i->data].key) != SUCCESS)
                            table_insert(p, edge->key2, g->p[(size_t)i->data].key);
                        any = true;
                    }
                }
        if (!any) break;
    }

    list_delete(busy_cells, false);
    if ((size_t) table_get(d, key_target) == LONG_MAX)
    {
        table_delete(d, false);
        table_delete(p, false);
        return NULL;
    }

    List* path = list();
    for (key = key_target; key; key = table_get(p, key))
        list_push(path, key);
    list_push(path, table_get(d, key_target));

    table_delete(d, false);
    table_delete(p, false);
    return path;
}


Graph* graph_path_tree(Graph* g)
{
    Graph* tree = graph_create(g->num_busy*2);
    if (!tree) return NULL;

    Table* min_e = table_create(g->num_busy*2);
    Table* sel_e = table_create(g->num_busy*2);
    if (!min_e || !sel_e) { graph_delete(tree); return NULL; }

    KeyType *key, *key2;
    Item* i;
    long weight, min_weight;
    size_t pos;

    for (pos = 0; pos < g->size; pos++)
        if (g->p[pos].busy == CELL_BUSY)
        {
            key = g->p[pos].key;
            table_insert(min_e, key, (void*) LONG_MAX);
            table_insert(sel_e, key, NULL);
            graph_add_node(tree, key, graph_get(g, key)->point);
        }
    table_update(min_e, key, 0);

    List* q = list();
    Edge* edge;
    Pair* p = malloc(sizeof(Edge));
    if (!p) return NULL;
    p->first = 0;
    p->second = key;
    list_push(q, p);

    for (pos = 0; pos < g->size && q->length; pos++)
    {
        p = list_pop(q, 0);
        key = p->second;
        free(p);

        if ((key2 = table_get(sel_e, key)))
        {
            edge = graph_get_edge(g, key, key2);
            graph_add_edge(tree, edge->weight, key, key2);
        }

        for (i = ((Node*) table_get(g, key))->adj->head; i; i = i->next)
        {
            edge = i->data;
            key2 = edge->key2;
            weight = edge->weight;
            min_weight = (long) table_get(min_e, key2);
            if (weight < min_weight)
            {
                table_update(min_e, key2, (void*) weight);
                table_update(sel_e, key2, key);
                p = malloc(sizeof(Pair));
                p->first = weight;
                p->second = key2;
                graph_queue_insert(q, p);
            }
        }
    }
    table_delete(min_e, false);
    table_delete(sel_e, false);
    list_delete(q, true);
    return tree;
}


void graph_print(Graph* g)
{
    if (!g) return;

    size_t i;
    Item* j;
    Node* node;
    Edge* edge;
    puts("Graph view:");
    for (i = 0; i < g->size; i++)
        if (g->p[i].busy == CELL_BUSY)
        {
            node = g->p[i].data;
            printf("['%s': (%lf,%lf)] <-> [ ", g->p[i].key, node->point[0], node->point[1]);
            for (j = node->adj->head; j; j = j->next)
            {
                edge = j->data;
                printf("(%d,'%s') ", edge->weight, edge->key2);
            }
            printf("]\n");
        }
}


GraphErrorCode graph_save(Graph* g, const char* filename)
{
    if (!g) return ERROR_ELEM_NOT_FOUND;

    FILE* f = fopen(filename, "w");
    if (!f) return ERROR_FILE_OPEN;

    size_t i;
    Node* node;
    Edge* edge;
    KeyType* key;
    Item* j;
    fprintf(f, "%lu %lu\n", g->size, g->num_busy);
    for (i = 0; i < g->size; i++)
        if (g->p[i].busy == CELL_BUSY)
        {
            key = g->p[i].key;
            node = g->p[i].data;
            fprintf(f, "%s  %lf %lf  %lu  ", key, node->point[0], node->point[1], node->adj->length);
            for (j = node->adj->head; j; j = j->next)
            {
                edge = j->data;
                fprintf(f, "%d %s ", edge->weight, edge->key2);
            }
            fputc('\n', f);
        }
    fclose(f);
    return SUCCESS;
}


GraphErrorCode graph_export(Graph* g, const char* filename)
{
    if (!g) return ERROR_ELEM_NOT_FOUND;

    FILE* f = fopen(filename, "w");
    if (!f) return ERROR_FILE_OPEN;

    size_t i;
    Node* node;
    Edge* edge;
    KeyType* key;
    Item* j;
    fputs("{\n", f);
    for (i = 0; i < g->size; i++)
        if (g->p[i].busy == CELL_BUSY)
        {
            key = g->p[i].key;
            node = g->p[i].data;
            fprintf(f, "    '%s': ((%lf,%lf), [", key, node->point[0], node->point[1]);
            for (j = node->adj->head; j; j = j->next)
            {
                edge = j->data;
                fprintf(f, "(%d, '%s'), ", edge->weight, edge->key2);
            }
            fputs("]),\n", f);
        }
    fputs("}\n", f);
    fclose(f);
    return SUCCESS;
}


void graph_delete(Graph* g)
{
    if (!g) return;

    size_t i;
    Node* node;
    Edge* edge;
    for (i = 0; i < g->size; i++)
        if (g->p[i].busy == CELL_BUSY)
        {
            node = g->p[i].data;
            while ((edge = list_pop(node->adj, 0)))
            {
                free(edge->key2);
                free(edge);
            }
            list_delete(node->adj, true);
        }
    table_delete(g, true);
}

// ------------------------------------------------------------------------------------------------------------------ //
