/*
 *		Lab 3: tables      (3.a) RAM version
 *
 *	It works! Really works. Without memory leaks, segfaults and other errors.
 *
 *	TODO: (3.b) file operations, (**)iterator? (***) buffer/cache?
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "input.h"
#include "table.h"

#ifndef VEC_SIZE
 #define VEC_SIZE 64
#endif

#ifndef KEY_LENGTH
 #define KEY_LENGTH 32
#endif

#ifndef INFO_LENGTH
 #define INFO_LENGTH 32
#endif


void print_element(Element* el, FILE* out);
void print_table(Table* t, FILE* out);
int interactive(Table* t);

void _syntax_error();


void print_element(Element* el, FILE* out)
{
    if (!el)
    {
        puts("Element not found.");
        return;
    }
    Element* ver;
    fprintf(out, "\nElement [\"%s\", %d]:\n", el->key1, el->key2);
    for (ver = el; ver; ver = ver->next)
    {
        fprintf(out, "    version [%d]:\n", ver->release);
        fprintf(out, "        float1:  %f\n", ver->info->f1);
        fprintf(out, "        float2:  %f\n", ver->info->f2);
        fprintf(out, "        string:  %s\n", ver->info->str);
    }
}


void print_table(Table* t, FILE* out)
{
    KeySpace1* ks;
    Node1* node;

    fprintf(out, "Table [%d Keys #1, %d Keys #2]\n", t->csize1, t->csize2);
    for (ks = t->ks1; ks; ks = ks->next)
        for (node = ks->node; node; node = node->next)
            print_element(node->item, out);
}


int interactive(Table* t)
{
    char command[16];
    char filename[256];
    int response;
    int error;

    Table* subt = NULL;          // assistant subtable

    KeyType1 key1;
    KeyType2 key2;

    Element* el;
    int version;

    while (true)
    {
        if (input("%15s", command) == EOF)
            strcpy(command, "exit");

        if (!strcmp("view", command))
            print_table(t, stdout);

        else if (!strcmp("size", command))
            printf("Table [%d Keys #1, %d Keys #2]\n", t->csize1, t->csize2);

        else if (!strcmp("add", command))
        {
            InfoType* info = malloc(sizeof(InfoType));

            key1 = malloc(KEY_LENGTH);
            info->str = malloc(INFO_LENGTH);

            error =
                input("%31s", key1)     ||
                input("%d", &key2)      ||
                input("%f", &info->f1)  ||
                input("%f", &info->f2)  ||
                input("%31s", info->str);

            if (!error)
                table_insert(t, key1, key2, info);
            else
            {
                _syntax_error();
                free(info->str);
                free(info);
            }
            free(key1);
        }

        else if (!strcmp("get", command))
        {
            key1 = malloc(KEY_LENGTH);
            input("%31s", key1);                        // key1 (string) && key2 (int)
            if (!(error = input("%d", &key2)))
            {
                el = table_get(t, key1, key2);
                print_element(el, stdout);
            }
            else _syntax_error();
            free(key1);
        }

        else if (!strcmp("find", command))
        {
            input("%5s", command);                      // "key1" or "key2"
            if (!strcmp("key1", command))
            {
                key1 = malloc(KEY_LENGTH);
                input("%31s", key1);                    // key1 (string)

                subt = table_get_subt_by_key1(t, key1);
                print_table(subt, stdout);

                // delete table
                table_delete(subt, false);
                free(key1);
            }
            else if (!strcmp("key2", command))
            {
                if (!(error = input("%d", &key2)))      // key2 (int)
                {
                    subt = table_get_subt_by_key2(t, key2);
                    print_table(subt, stdout);

                    // delete table
                    table_delete(subt, false);
                }
                else _syntax_error();
            }
            else puts("Usage:  find <\"key1\"/\"key2\"> <key1 / key2>");
        }

        else if (!strcmp("rm", command))
        {
            key1 = malloc(KEY_LENGTH);
            input("%31s", key1);
            if (!(error = input("%d", &key2)))
            {

                if (!scanf("%d", &version))     // probably not number, maybe "all"?
                {
                    input("%4s", command);
                    if (!strcmp("all", command))
                        version = -1;
                    else
                    {
                        _syntax_error();
                        free(key1);
                        return 0;
                    }
                }
                response = table_rm_elem(t, key1, key2, version);
                if (response)                           // error indicating that func couldn't find that element
                    puts("Element not found.");
            }
            else _syntax_error();
            free(key1);
        }

        else if (!strcmp("rmkey", command))
        {
            input("%5s", command);               // "key1" or "key2"
            if (!strcmp("key1", command))
            {
                key1 = malloc(KEY_LENGTH);
                input("%31s", key1);             // key1 (string)

                response = table_rm_key1(t, key1);
                printf("Removed %d elements by key \"%s\".\n", response, key1);
                free(key1);
            }
            else if (!strcmp("key2", command))
            {
                if (!(error = input("%d", &key2))) // key2 (int)
                {
                    response = table_rm_key2(t, key2);
                    printf("Removed %d elements by key %d.\n", response, key2);
                }
                else _syntax_error();
            }
            else puts("Usage:  rmkey <\"key1\"/\"key2\"> <key1 / key2>");
        }

        else if (!strcmp("save", command))
        {
            input("%*c%255[^\n]", filename);
            FILE* out = fopen(filename, "w");

            if (!out)
            {
                printf("File Error: could not open file \"%s\".\n", filename);
                return 0;
            }

            print_table(t, out);
            printf("Saved table to \"%s\".\n", filename);
        }

        else if (!strcmp("help", command))
        {
            puts("Available commands:\n"
                 "    add <key1:string> <key2:int> <float> <float> <string>\n"
                 "    rm <key1:string> <key2:int> <version:int/\"all\">\n"
                 "    rmkey <\"key1\"/\"key2\"> <key1:string/key2:int>\n"
                 "    get <key1:string> <key2:int>\n"
                 "    find <\"key1\"/\"key2\"> <key1:string/key2:int>\n"
                 "    view\n"
                 "    save <file>\n"
                 "    help\n"
                 "    exit\n");
        }

        else if (!strcmp("exit", command))
        {
            // cleanup & exit
            table_delete(t, true);
            return 0;
        }

        else
            printf("Unknown command: \"%s\".\n", command);

    }
}


void _syntax_error()
{
    puts("Syntax Error: check your command and try again.");
    scanf("%*[^\n]");
}


int main()
{
    Table* t = table(VEC_SIZE);

    puts("Dual-key table CLI\t (3.a) RAM version \n"
         "Type \"help\" to see available commands.");
    interactive(t);

    return 0;
}