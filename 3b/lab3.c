/*
 *		Lab 3: tables      (3.c) Enhanced File-based version (**)
 *
 *	It works! Really works. Without memory leaks, segfaults and other errors.
 *
 */

#pragma clang diagnostic push
#pragma ide diagnostic ignored "DanglingPointers"
#pragma ide diagnostic ignored "bugprone-reserved-identifier"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <errno.h>

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


void print_element(Table* t, Offset el, FILE* out);
void print_table(Table* t, FILE* out);
int interactive();

char* _rand_dirname();
void _syntax_error();
void _notloaded_error();


char* _rand_dirname()
{
    srand(time(NULL));
    int i;
    char* rand_dirname = malloc(16);
    strcpy(rand_dirname, ".lab3_");
    for (i = 6; i < 15; i++)
        rand_dirname[i] = 'a' + (rand()%26);
    rand_dirname[15] = '\0';
    return rand_dirname;
}

void _syntax_error()
{
    fprintf(stderr, "Syntax Error: check your command and try again.\n");
    scanf("%*[^\n]");
}

void _notloaded_error()
{
    fprintf(stderr, "File Error: table is not loaded.\n");
    scanf("%*[^\n]");
}

void _file_error(char* filename, long err, long id)
{
    char _errfile[16];
    char* errfile = _errfile;
    switch (id)
    {
        case ERROR_OPEN_TABLE:
            strcpy(errfile, "table.dat");
            break;
        case ERROR_OPEN_KS1:
            strcpy(errfile, "ks1.dat");
            break;
        case ERROR_OPEN_KS2:
            strcpy(errfile, "ks1.dat");
            break;
        case ERROR_OPEN_NODE:
            strcpy(errfile, "node.dat");
            break;
        case ERROR_OPEN_ITEM:
            strcpy(errfile, "item.dat");
            break;
        case ERROR_MEMORY_ALLOC:
            strcpy(errfile, "memory block");
            break;
        default:  // error_open_dir included
            errfile = filename;

    }
    fprintf(stderr, "Could not open %s: ", errfile);

    switch (err)
    {//perror
        //strerror
        //man explain
        case EPERM:
            fprintf(stderr,"Operation not permitted.\n");
            break;
        case ENOENT:
            fprintf(stderr,"File not found.\n");
            break;
        case EACCES:
            fprintf(stderr,"Permission denied.\n");
            break;
        case ENAMETOOLONG:
            fprintf(stderr,"Filename is too long.\n");
            break;
        case ELOOP:
            fprintf(stderr, "Too many symbolic links encountered.\n");
            break;
        case EEXIST:
            fprintf(stderr, "Directory already exists.\n");
            break;
        default:
            fprintf(stderr, "File opening error.\n");
    }
}


void print_element(Table* t, Offset el, FILE* out)
{
    if (el == -1)
    {
        puts("Element not found.");
        return;
    }
    Element* ver = malloc(sizeof(Element));
    fseek(t->f_item, el, SEEK_SET);
    fread(ver, sizeof(Element), 1, t->f_item);

    fprintf(out, "\nElement [\"%s\", %d]:\n", ver->key1, ver->key2);
    fprintf(out, "    version [%d]:\n", ver->release);
    fprintf(out, "        float1:  %f\n", ver->info.f1);
    fprintf(out, "        float2:  %f\n", ver->info.f2);
    fprintf(out, "        string:  %s\n", ver->info.str);

    while (ver->next_offset != -1)
    {
        fseek(t->f_item, ver->next_offset, SEEK_SET);
        fread(ver, sizeof(Element), 1, t->f_item);

        fprintf(out, "    version [%d]:\n", ver->release);
        fprintf(out, "        float1:  %f\n", ver->info.f1);
        fprintf(out, "        float2:  %f\n", ver->info.f2);
        fprintf(out, "        string:  %s\n", ver->info.str);
    }
    free(ver);
}


void print_table(Table* t, FILE* out)
{
    fprintf(out, "Table [%d Keys #1, %d Keys #2]\n", t->csize1, t->csize2);
    Offset ks2;
    KeySpace2 ks2obj;
    fseek(t->f_ks2, 0, SEEK_SET);
    for (ks2 = 0; ks2 < t->msize2; ks2++)
    {
        fread(&ks2obj, sizeof(KeySpace2), 1, t->f_ks2);
        if (ks2obj.busy == BUSY)
            print_element(t, ks2obj.item_offset, out);
    }
}


int interactive()
{
    char command[16];
    char filename[256];
    char* rnd_dir = NULL;
    int response;
    int error;

    Table* t = NULL;             // main table
    Table* subt = NULL;          // assistant subtable

    KeyType1 key1[KEY_LENGTH];
    KeyType2 key2;

    Offset el;
    Element* item = malloc(sizeof(Element));
    InfoType* info = malloc(sizeof(InfoType));
    if (!item || !info)
        return ERROR_SOMETHING_BROKEN;

    int version;

    while (true)
    {
        if (input("%15s", command) == EOF)
            strcpy(command, "exit");

        if (!strcmp("cd", command))
        {
            if (!(error = input("%*c%127[^\n]", filename)))
                chdir(filename);
            else
                system("cd");
            system("pwd");
        }

        else if (!strcmp("pwd", command))
            system("pwd");

        else if (!strcmp("la", command))
            system("ls -lah");

        else if (!strcmp("ll", command))
            system("ls -lh");

        else if (!strcmp("ls", command))
            system("ls");

        else if (!strcmp("create", command))
        {
            if (!(error = input("%*c%127[^\n]", filename)))
            {
                if (t) table_close(t);
                t = table_create(filename, VEC_SIZE);
                if ((long) t < (1<<16))
                {
                    _file_error(filename, (long) t & 0xff, (long) t & 0xff00);
                    t = NULL;
                }
                else
                    printf("Created table \"%s\".\n", filename);
            }
            else _syntax_error();
        }

        else if (!strcmp("open", command))
        {
            if (!(error = input("%*c%127[^\n]", filename)))
            {
                if (t) table_close(t);
                t = table_load(filename);
                if ((long) t < (1<<16))
                {
                    _file_error(filename, (long) t & 0xff, (long) t & 0xff00);
                    t = NULL;
                }
                else
                    printf("Opened table \"%s\":\n[%d Keys #1, %d Keys #2]\n", filename, t->csize1, t->csize2);
            }
            else _syntax_error();
        }

        else if (!strcmp("close", command))
            if (t)
            {
                table_close(t);
                t = NULL;
                puts("Successfully saved and closed.");
            }
            else _notloaded_error();

        else if (!strcmp("view", command))
            if (t) print_table(t, stdout);
            else _notloaded_error();

        else if (!strcmp("size", command))
            if (t) printf("Table [%d Keys #1, %d Keys #2]\n", t->csize1, t->csize2);
            else _notloaded_error();

        else if (!strcmp("add", command))
            if (t)
            {
                error = input("%31s", key1)     ||
                        input("%d", &key2)      ||
                        input("%f", &info->f1)  ||
                        input("%f", &info->f2)  ||
                        input("%31s", info->str);
                if (!error)
                    table_insert(t, key1, key2, info);
                else _syntax_error();
            }
            else _notloaded_error();

        else if (!strcmp("get", command))
            if (t)
            {
                input("%31s", key1);                        // key1 (string) && key2 (int)
                if (!(error = input("%d", &key2)))
                {
                    el = table_get(t, key1, key2);
                    print_element(t, el, stdout);
                }
                else _syntax_error();
            }
            else _notloaded_error();

        else if (!strcmp("find", command))
            if (t)
            {
                input("%5s", command);                      // "key1" or "key2"
                if (!strcmp("key1", command))
                {
                    input("%31s", key1);                    // key1 (string)
                    scanf("%c", command);
                    if (command[0] == '\n' || !scanf("%127[^\n]%*c", filename))        // directory to save into
                    {
                        rnd_dir = _rand_dirname();
                        sprintf(filename, "/tmp/%s", rnd_dir);
                        free(rnd_dir);
                    }
                    subt = table_get_subt_by_key1(t, key1, filename);
                    if ((long) subt < (1<<16))
                    {
                        _file_error(filename, (long) subt & 0xff, (long) subt & 0xff00);
                        continue;
                    }
                    printf("Subtable has been saved to \"%s\".\nView table [%d Keys #1, %d Keys #2]? (Y/n) ", filename, subt->csize1, subt->csize2);
                    scanf("%c", command);
                    if (command[0] != 'n' && command[0] != 'N')
                        print_table(subt, stdout);

                    table_close(subt);
                    if (!strncmp("/tmp/.lab3_", filename, 10))
                        rmdir(filename);
                }
                else if (!strcmp("key2", command))
                {
                    if (!(error = input("%d", &key2)))       // key2 (int)
                    {
                        scanf("%c", command);
                        if (command[0] == '\n' || !scanf("%127[^\n]%*c", filename))     // directory to save into
                        {
                            rnd_dir = _rand_dirname();
                            sprintf(filename, "/tmp/%s", rnd_dir);
                            free(rnd_dir);
                        }
                        subt = table_get_subt_by_key2(t, key2, filename);
                        if ((long) subt < (1<<16))
                        {
                            _file_error(filename, (long) subt & 0xff, (long) subt & 0xff00);
                            continue;
                        }
                        printf("Subtable has been saved to \"%s\".\nView table [%d Keys #1, %d Keys #2]? (Y/n) ", filename, subt->csize1, subt->csize2);
                        scanf("%c", command);
                        if (command[0] != 'n' && command[0] != 'N')
                            print_table(subt, stdout);

                        table_close(subt);
                    }
                    else _syntax_error();
                }
                else fprintf(stderr, "Usage:  find <\"key1\"/\"key2\"> <key1/key2>\n");
            }
            else _notloaded_error();

        else if (!strcmp("rm", command))
            if (t)
            {
                input("%31s", key1);
                if (!(error = input("%d", &key2)))
                {

                    if (!scanf("%d", &version))     // probably not number, maybe "all"?
                    {
                        input("%4s", command);
                        if (!strcmp("all", command))
                            version = -1;
                        else
                            _syntax_error();
                    }
                    response = table_rm_elem(t, key1, key2, version);
                    if (response)                           // error indicating that func couldn't find that element
                        fprintf(stderr, "Element not found.\n");
                }
                else _syntax_error();
            }
            else _notloaded_error();

        else if (!strcmp("rmkey", command))
            if (t)
            {
                input("%5s", command);               // "key1" or "key2"
                if (!strcmp("key1", command))
                {
                    input("%31s", key1);             // key1 (string)

                    response = table_rm_key1(t, key1);
                    printf("Removed %d elements by key \"%s\".\n", response, key1);
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
                else fprintf(stderr, "Usage:  rmkey <\"key1\"/\"key2\"> <key1 / key2>\n");
            }
            else _notloaded_error();

        else if (!strcmp("export", command))
            if (t)
            {
                input("%*c%255[^\n]", filename);
                FILE* out = fopen(filename, "w");

                if (!out)
                {
                    _file_error(filename, errno, ERROR_OPEN_DIR);
                    continue;
                }

                print_table(t, out);
                printf("Saved table to \"%s\".\n", filename);
            }
            else _notloaded_error();

        else if (!strcmp("help", command))
        {
            puts("Available commands:\n"
                 "    ls / ll / la\n"
                 "    cd <dir> / pwd\n"
                 "    create <table_dir>\n"
                 "    open <table_dir>\n"
                 "    close\n"
                 "    add <key1:string> <key2:int> <float> <float> <string>\n"
                 "    rm <key1:string> <key2:int> <version:int/\"all\">\n"
                 "    rmkey <\"key1\"/\"key2\"> <key1:string/key2:int>\n"
                 "    get <key1:string> <key2:int>\n"
                 "    find <\"key1\"/\"key2\"> <key1:string/key2:int> [subtable_dir]\n"
                 "    view\n"
                 "    export <file>\n"
                 "    help\n"
                 "    exit");
        }

        else if (!strcmp("exit", command))
        {
            // cleanup & exit
            free(info);
            free(item);
            if (t) table_close(t);
            return 0;
        }

        else
        {
            fprintf(stderr, "Unknown command: \"%s\".\n", command);
            scanf("%*[^\n]");
        }

    }
}


int main()
{
    puts("Dual-key table CLI\t\t (3.c) Enhanced File-based version (**) \n"
         "Type \"help\" to see available commands.");

    srand(time(NULL));
    interactive();

    return 0;
}

#pragma clang diagnostic pop