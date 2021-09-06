/*
 *      Lab 5 - Graph
 *
 *   Undirected, non-negative weighted graph using strings as its keys.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include "climsg.h"
#include "input.h"
#include "list.h"
#include "graph.h"


#ifndef VIZ_PATH
#define VIZ_PATH "./viz.py"
#endif

#ifndef TMP_DIR
#define TMP_DIR "/tmp"
#endif


#ifndef GRAPH_SIZE
#define GRAPH_SIZE 1024
#endif

#ifndef KEY_LENGTH
#define KEY_LENGTH 64
#endif


// ------------------------------------------------ internal utils -------------------------------------------------- //

static void syntax_error()
{
    fprintf(stderr, MSG_SYNTAXERROR);
    scanf("%*[^\n]");
}


static void notloaded_error()
{
    fprintf(stderr, MSG_NOTLOADED);
    scanf("%*[^\n]");
}


static void file_error(const char* filename)
{
    fprintf(stderr, MSG_FILEERROR, filename);
    if (errno) perror("");
}


static char* rand_dirname()
{
    int i;
    char* rand_dirname = malloc(24);
    strcpy(rand_dirname, TMP_DIR "/.lab4b_");
    for (i = 12; i < 23; i++)
        rand_dirname[i] = (char) ('a' + rand()%26);
    rand_dirname[23] = '\0';
    return rand_dirname;
}

// ------------------------------------------------------------------------------------------------------------------ //



// --------------------------------------------------- functions ---------------------------------------------------- //

int interactive()
{
    char command[32];
    char filename[256];
    char* rand_dir;
    GraphErrorCode error;

    Graph* g = NULL;
    Graph* tree;

    Node* node;
    Edge* edge;
    Item* item;
    List* path;

    KeyType key1[KEY_LENGTH];
    KeyType key2[KEY_LENGTH];
    WeightType weight;
    int rand_size;
    float rand_density;
    double point[K];

    while (true)
    {
        if (input("%15s", command) == EOF)
            strcpy(command, "exit");

        if (!strcmp("create", command))
        {
            if (g) graph_delete(g);
            g = graph_create(GRAPH_SIZE);
        }

        else if (!strcmp("load", command))
        {
            input("%*c%255[^\n]", filename);
            if (g) graph_delete(g);

            g = graph_load(filename);
            if (!g)
                file_error(filename);
            else
                printf(MSG_LOADED, filename);
        }

        else if (!strcmp("random", command))
            if (!input("%d", &rand_size) && !input("%f", &rand_density))
            {
                if (rand_size < 0) fprintf(stderr, MSG_NEGSIZE);
                else if (rand_density < 0 || rand_density > 1) fprintf(stderr, MSG_INV_DENSITY);
                else
                {
                    if (g) graph_delete(g);
                    g = graph_random(rand_size*4, rand_size, rand_density);
                    if (!g) fprintf(stderr, MSG_MEMFULL);
                    else printf(MSG_GENERATED, rand_size);
                }
            }
            else syntax_error();

        else if (!strcmp("add", command))
            if (!g) notloaded_error();
            else if (!input("%5s", command))
            {
                if (!strcmp("node", command))
                    if (!input("%63s", key1) && !input("%lf", &point[0]) && !input("%lf", &point[1]))
                    {
                        error = graph_add_node(g, key1, point);
                        if (error == ERROR_MEM_ALLOC) fprintf(stderr, MSG_MEMFULL);
                        else if (error == ERROR_DUP_KEY) fprintf(stderr, MSG_DUPKEY);
                    }
                    else syntax_error();
                else if (!strcmp("edge", command))
                    if (!input("%63s", key1) && !input("%63s", key2) && !input("%d", &weight))
                    {
                        if (weight < 0)
                        {
                            fprintf(stderr, MSG_NEGWEIGHT);
                            scanf("%*[^\n]");
                            continue;
                        }
                        error = graph_add_edge(g, weight, key1, key2);
                        if (error == ERROR_MEM_ALLOC) fprintf(stderr, MSG_MEMFULL);
                        else if (error == ERROR_DUP_KEY) fprintf(stderr, MSG_DUPKEY);
                        else if (error == ERROR_ELEM_NOT_FOUND) fprintf(stderr, MSG_ELNFOUND);
                    }
                    else syntax_error();
                else syntax_error();
            }
            else syntax_error();

        else if (!strcmp("rm", command))
            if (!g) notloaded_error();
            else if (!input("%5s", command))
            {
                if (!strcmp("node", command))
                    if (!input("%63s", key1))
                    {
                        error = graph_rm_node(g, key1);
                        if (error == ERROR_ELEM_NOT_FOUND) fprintf(stderr, MSG_ELNFOUND);
                    }
                    else syntax_error();
                else if (!strcmp("edge", command))
                    if (!input("%63s", key1) && !input("%63s", key2))
                    {
                        error = graph_rm_edge(g, key1, key2);
                        if (error == ERROR_ELEM_NOT_FOUND) fprintf(stderr, MSG_ELNFOUND);
                    }
                    else syntax_error();
                else syntax_error();
            }
            else syntax_error();

        else if (!strcmp("get", command))
            if (!g) notloaded_error();
            else if (!input("%63s", key1))
            {
                node = graph_get(g, key1);
                if (!node) fprintf(stderr, MSG_ELNFOUND);
                else
                {
                    printf("['%s': (%lf,%lf)] <-> ", key1, node->point[0], node->point[1]);
                    for (item = node->adj->head; item; item = item->next)
                    {
                        edge = item->data;
                        printf("(%d,'%s') ", edge->weight, edge->key2);
                    }
                    putchar('\n');
                }
            }
            else syntax_error();

        else if (!strcmp("path", command))
            if (!g) notloaded_error();
            else if (!input("%5s", command))
            {
                if (!strcmp("near", command))
                    if (!input("%63s", key1) && !input("%63s", key2))
                    {
                        path = graph_path_bfs(g, key1, key2);
                        printf("Path from '%s' to '%s':\n", key1, key2);
                        if (path)
                        {
                            printf("[%lu nodes]: ", path->length);
                            for (item = path->head; item; item = item->next)
                                printf("'%s' ", (char*) item->data);
                            putchar('\n');
                            list_delete(path, false);
                        }
                        else printf("does not exist.\n");
                    }
                    else syntax_error();
                else if (!strcmp("best", command))
                    if (!input("%63s", key1) && !input("%63s", key2))
                    {
                        path = graph_path_bellmanford(g, key1, key2);
                        printf("Path from '%s' to '%s':\n", key1, key2);
                        if (path)
                        {
                            weight = (long) list_pop(path, 0);
                            printf("[weight %d, %lu nodes]: ", weight, path->length);
                            for (item = path->head; item; item = item->next)
                                printf("'%s' ", (char*) item->data);
                            putchar('\n');
                            list_delete(path, false);
                        }
                        else printf("does not exist.\n");
                    }
                    else syntax_error();
                else if (!strcmp("tree", command))
                {
                    tree = graph_path_tree(g);
                    if (tree)
                    {
                        rand_dir = rand_dirname();
                        if (graph_export(tree, rand_dir) != SUCCESS)
                        {
                            file_error(rand_dir);
                            free(rand_dir);
                            continue;
                        }
                        graph_delete(tree);
                        sprintf(filename, VIZ_PATH " %24s plt &", rand_dir);
                        free(rand_dir);
                        system(filename);
                    }
                    else fprintf(stderr, MSG_FAILED_MST);
                }
                else syntax_error();
            }
            else syntax_error();

        else if (!strcmp("print", command))
            if (g) graph_print(g);
            else puts(MSG_NOTLOADED);

        else if (!strcmp("save", command))
            if (!g) notloaded_error();
            else
            {
                input("%*c%255[^\n]", filename);
                if (graph_save(g, filename) == SUCCESS)
                    printf(MSG_SAVED, filename);
                else
                    file_error(filename);
            }

        else if (!strcmp("plot", command) || !strcmp("view", command))
            if (!g) notloaded_error();
            else
            {
                rand_dir = rand_dirname();
                if (graph_export(g, rand_dir) != SUCCESS)
                {
                    file_error(rand_dir);
                    free(rand_dir);
                    continue;
                }
                if (!strcmp("plot", command))
                    sprintf(filename, VIZ_PATH " %24s plt &", rand_dir);
                else    // ("view", command))
                    sprintf(filename, VIZ_PATH " %24s nx &", rand_dir);
                free(rand_dir);
                system(filename);
            }

        else if (!strcmp("help", command))
            puts(MSG_HELP);

        else if (!strcmp("close", command))
            if (!g) notloaded_error();
            else
            {
                graph_delete(g);
                g = NULL;
            }

        else if (!strcmp("exit", command))
        {
            if (g) graph_delete(g);
            return EXIT_SUCCESS;
        }

        else
        {
            fprintf(stderr, MSG_UNKNOWNCMD, command);
            scanf("%*[^\n]");
        }
    }
}


int main()
{
    puts(MSG_INTRO);
    srand(time(NULL));

    int err = interactive();
    return err;
}

// ------------------------------------------------------------------------------------------------------------------ //
