/*
 *     graph basic structs and methods
 */

#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "list.h"
#include "table.h"

#define K 2


// ------------------------------------------------ Graph structs --------------------------------------------------- //

typedef int WeightType;

typedef Table Graph;
typedef TableErrorCode GraphErrorCode;

typedef struct Node
{
    double point[K];
    List* adj;
} Node;

typedef struct Edge
{
    WeightType weight;
    KeyType* key2;
} Edge;

typedef struct Pair
{
    long first;
    KeyType* second;
} Pair;

// ------------------------------------------------------------------------------------------------------------------ //



// ------------------------------------------------- functions ------------------------------------------------------ //

Graph* graph_create(size_t max_size);

Graph* graph_load(const char* filename);

Graph* graph_random(size_t max_size, size_t nnodes, float density);

// ------------------------------------------------------------------------------------------------------------------ //



// -------------------------------------------------- methods ------------------------------------------------------- //

GraphErrorCode graph_add_node(Graph* g, const KeyType* key, const double point[K]);

GraphErrorCode graph_add_edge(Graph* g, WeightType weight, const KeyType* key1, const KeyType* key2);

GraphErrorCode graph_rm_node(Graph* g, const KeyType* key);

GraphErrorCode graph_rm_edge(Graph* g, const KeyType* key1, const KeyType* key2);

Node* graph_get(Graph* g, const KeyType* key);

List* graph_path_bfs(Graph* g, KeyType* key_init, KeyType* key_target);

List* graph_path_bellmanford(Graph* g, KeyType* key_init, KeyType* key_target);

Graph* graph_path_tree(Graph* g);

GraphErrorCode graph_save(Graph* g, const char* filename);

GraphErrorCode graph_export(Graph* g, const char* filename);

void graph_print(Graph* g);

void graph_delete(Graph* g);

// ------------------------------------------------------------------------------------------------------------------ //



// ----------------------------------------------- internal utils --------------------------------------------------- //

static KeyType* graph_rand_key(size_t len);

static int graph_queue_insert(List* l, Pair* el);

// ------------------------------------------------------------------------------------------------------------------ //
