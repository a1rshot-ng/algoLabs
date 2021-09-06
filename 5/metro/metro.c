/*
 *      Lab 5  -  metro navigator (**)
 *
 *    Utility to find the best route between two Moscow metro stations
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <wctype.h>
#include <locale.h>

#include "input.h"
#include "list.h"
#include "graph.h"


#ifndef METRO_FILE
#define METRO_FILE "../metro_graph.txt"
#endif

#ifndef NAME_LENGTH
#define NAME_LENGTH 64
#endif


static void str_edit(char* s)
{
    size_t i;
    wchar_t ws[NAME_LENGTH];
    mbstowcs(ws, s, NAME_LENGTH);
    for (i = 0; ws[i]; i++)
    {
        ws[i] = towlower(ws[i]);
        if (ws[i] == L'ё')
            ws[i] = L'е';
        else if (ws[i] == L' ')
            ws[i] = L'_';
    }
    wcstombs(s, ws, NAME_LENGTH);
}


int main()
{
    Graph* g = graph_load(METRO_FILE);
    if (!g)
    {
        fprintf(stderr, "Couldn't load metro graph from '%s':\n", METRO_FILE);
        perror("");
        return EXIT_FAILURE;
    }

    setlocale(LC_ALL, "ru_RU");

    char station1[NAME_LENGTH];
    char station2[NAME_LENGTH];

    Item* item;
    List* path;
    WeightType weight;

    puts("Metro Navigator\t (Lab 5) Graph");
    puts("Type \"exit\" to exit the program.");
    while (true)
    {
        printf(" From: ");
        if (input("%63[^\n]%*c", station1) || !strcmp("exit", station1))
            break;
        str_edit(station1);
        if (!graph_get(g, station1))
        {
            fprintf(stderr, "Station '%s' not found, try again.\n", station1);
            continue;
        }

        printf(" To: ");
        if (input("%63[^\n]%*c", station2) || !strcmp("exit", station2))
            break;
        str_edit(station2);
        if (!graph_get(g, station2))
        {
            fprintf(stderr, "Station '%s' not found, try again.\n", station2);
            continue;
        }

        path = graph_path_bellmanford(g, station1, station2);
        printf("Path from '%s' to '%s':\n", station1, station2);
        if (path)
        {
            weight = (long) list_pop(path, 0);
            printf("[%d minutes, %lu stations]: ", weight, path->length);
            for (item = path->head; item; item = item->next)
                printf("'%s' ", (char*) item->data);
            putchar('\n');
            list_delete(path, false);
        }
        else printf("Is not built yet.\n");
    }

    graph_delete(g);
    return EXIT_SUCCESS;
}