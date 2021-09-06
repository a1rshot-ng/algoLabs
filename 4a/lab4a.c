/*
 *          Lab 4a:  Simple Tree
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "climsg.h"
#include "input.h"
#include "bintree.h"


void _syntax_error()
{
    fprintf(stderr, MSG_SYNTAXERROR);
    scanf("%*[^\n]");
}


int interactive()
{
    char command[16];
    char filename[256];
    int error;

    Node* t = NULL;
    Node* node;
    KeyType key;
    KeyType key2;
    int version;
    char* infostr = malloc(INFO_LENGTH);

    while (true)
    {
        if (input("%15s", command) == EOF)
            strcpy(command, "exit");

        if (!strcmp("view", command))
            bintree_print(t);

        else if (!strcmp("add", command))
        {
            InfoType* info = malloc(sizeof(InfoType));

            error = input("%u", &key)       ||
                    input("%f", &info->f1)  ||
                    input("%63s", infostr);

            if (!error)
            {
                info->str = strdup(infostr);
                bintree_add(&t, key, info);
            }
            else
            {
                _syntax_error();
                free(info->str);
                free(info);
            }
        }

        else if (!strcmp("get", command))
            if (!input("%u", &key) && !input("%d", &version))
            {
                node = bintree_find(t, key, version);
                if (node)
                    printf("[%u]:\t%f\t%s\n", node->key, node->info->f1, node->info->str);
                else
                    fprintf(stderr, MSG_NOTFOUND);
            }
            else _syntax_error();

        else if (!strcmp("min", command))
            if (!input("%d", &version))
            {
                node = bintree_min(t, version);
                if (node)
                    printf("[%u]:\t%f\t%s\n", node->key, node->info->f1, node->info->str);
                else
                    fprintf(stderr, MSG_NOTFOUND);
            }
            else _syntax_error();

        else if (!strcmp("range", command))
            if (!input("%u", &key) && !input("%u", &key2))
                bintree_print_range(t, key, key2);
            else _syntax_error();

        else if (!strcmp("rm", command))
            if (!input("%u", &key) && !input("%d", &version))
            {
                if (bintree_rm(&t, key, version) == ERROR_ELEM_NOT_FOUND)
                    fprintf(stderr, MSG_NOTFOUND);
            }
            else _syntax_error();

        else if (!strcmp("save", command))
        {
            input("%*c%255[^\n]", filename);
            if (bintree_save(t, filename) == SUCCESS)
                printf(MSG_SAVED, filename);
            else
                fprintf(stderr, MSG_FILEERROR, filename);
        }

        else if (!strcmp("load", command))
        {
            input("%*c%255[^\n]", filename);
            if (t) bintree_delete(t);

            t = bintree_load(filename);
            if (t == (Node*) ERROR_FILE_OPEN)
            {
                t = bintree_create();
                fprintf(stderr, MSG_FILEERROR, filename);
            }
            else
                printf(MSG_LOADED, filename);
        }

        else if (!strcmp("close", command))
        {
            bintree_delete(t);
            t = bintree_create();
        }

        else if (!strcmp("help", command))
            puts(MSG_HELP);

        else if (!strcmp("exit", command))
        {
            bintree_delete(t);
            free(infostr);
            return SUCCESS;
        }

        else
            fprintf(stderr, MSG_UNKNOWNCMD, command);
    }
}


int main()
{
    int err;
    puts(MSG_INTRO);

    err = interactive();
    return err;
}