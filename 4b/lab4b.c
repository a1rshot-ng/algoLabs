/*
 *      Lab 4.b  -  K-d tree
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include "climsg.h"
#include "input.h"
#include "kdtree.h"


#ifndef VIZ_PATH
#define VIZ_PATH "./viz.py"
#endif

#ifndef TMP_DIR
#define TMP_DIR "/tmp"
#endif

#ifndef INFO_LENGTH
#define INFO_LENGTH 64
#endif


// ------------------------------------------------ internal utils -------------------------------------------------- //

static void syntax_error()
{
    fprintf(stderr, MSG_SYNTAXERROR);
    scanf("%*[^\n]");
}


static void file_error(const char* filename)
{
    fprintf(stderr, MSG_FILEERROR, filename);
    if (errno) perror("");
    else fprintf(stderr, MSG_MEMFULL);
}


static char* rand_dirname()
{
    srand(time(NULL));
    int i;
    char* rand_dirname = malloc(24);
    strcpy(rand_dirname, TMP_DIR "/.lab4b_");
    for (i = 12; i < 23; i++)
        rand_dirname[i] = 'a' + (rand()%26);
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
    int error;
    int offset;

    Node* t = NULL;
    KeyType keys[K];
    int version;
    char* infostr = malloc(INFO_LENGTH);
    Element* el;

    while (true)
    {
        if (input("%15s", command) == EOF)
            strcpy(command, "exit");

        if (!strcmp("view", command))
            kdtree_print(t);

        else if (!strcmp("add", command))
        {
            InfoType* info = malloc(sizeof(InfoType));

            error = input("%d", &keys[0]) ||
                    input("%d", &keys[1]) ||
                    input("%f", &info->f1)    ||
                    input("%f", &info->f2)    ||
                    input("%63s", infostr);

            if (!error)
            {
                info->str = strdup(infostr);
                kdtree_add(&t, keys, info);
            }
            else
            {
                syntax_error();
                free(info->str);
                free(info);
            }
        }

        else if (!strcmp("get", command))
            if (!input("%d", &keys[0]) && !input("%d", &keys[1]) && !input("%d", &version))
            {
                if ((el = kdtree_find(t, keys, version)))
                    printf("[%d %d]: [%d](%f, %f, %s)\n", keys[0], keys[1], version,
                                                                 el->info->f1, el->info->f2, el->info->str);
                else fprintf(stderr, MSG_ELNFOUND);
            }
            else syntax_error();

        else if (!strcmp("max", command))
            if (!input("%5s", command) && !input("%d", &version))
            {
                if (!strcmp("key1", command))
                    el = kdtree_max(t, 0, version);
                else if (!strcmp("key2", command))
                    el = kdtree_max(t, 1, version);
                else
                {
                    syntax_error();
                    continue;
                }

                if (el)
                    printf("[%d]: ([%d %d], %f %f %s)\n", version, el->point[0], el->point[1],
                                                                 el->info->f1, el->info->f2, el->info->str);
                else fprintf(stderr, MSG_ELNFOUND);
            }
            else syntax_error();

        else if (!strcmp("far", command))
        {
            el = kdtree_farthest(t);
            if (el)
                printf("[%d %d]: (%f, %f, %s)\n", el->point[0], el->point[1],
                                                         el->info->f1, el->info->f2, el->info->str);
            else fprintf(stderr, MSG_ELNFOUND);
        }

        else if (!strcmp("range", command))
        {
            command[0] = '\0';
            input("%31[^\n]", command);
            sscanf(command, "%*s%n",  &offset);
            if (sscanf(command, "%d", &keys[0]) != 1)
                keys[0] = INT_MAX;
            if (sscanf(command+offset, "%d", &keys[1]) != 1)
                keys[1] = INT_MAX;
            kdtree_print_range(t, keys, (keys[0] == INT_MAX && keys[1] != INT_MAX) ? 1 : 0);
        }

        else if (!strcmp("rm", command))
            if (!input("%d", &keys[0]) && !input("%d", &keys[1]))
            {
                if (kdtree_rm(&t, keys, 0) != SUCCESS)
                    fprintf(stderr, MSG_ELNFOUND);
            }
            else syntax_error();

        else if (!strcmp("save", command))
        {
            input("%*c%255[^\n]", filename);
            if (kdtree_save(t, filename) == SUCCESS)
                printf(MSG_SAVED, filename);
            else
                file_error(filename);
        }

        else if (!strcmp("load", command))
        {
            input("%*c%255[^\n]", filename);
            if (t) kdtree_delete(t);

            t = kdtree_load(filename);
            if (t == (Node*) ERROR_FILE_OPEN || t == (Node*) ERROR_MEM_FULL)
            {
                t = kdtree_create();
                file_error(filename);
            }
            else
                printf(MSG_LOADED, filename);
        }

        else if (!strcmp("plot", command))
        {
            rand_dir = rand_dirname();
            if (kdtree_export(t, rand_dir) == ERROR_FILE_OPEN)
            {
                file_error(rand_dir);
                free(rand_dir);
                continue;
            }
            sprintf(filename, VIZ_PATH " %24s &", rand_dir);
            free(rand_dir);
            system(filename);
        }

        else if (!strcmp("close", command))
        {
            kdtree_delete(t);
            t = kdtree_create();
        }

        else if (!strcmp("help", command))
            puts(MSG_HELP);

        else if (!strcmp("exit", command))
        {
            kdtree_delete(t);
            free(infostr);
            return SUCCESS;
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
    int err;
    puts(MSG_INTRO);

    err = interactive();
    return err;
}

// ------------------------------------------------------------------------------------------------------------------ //
