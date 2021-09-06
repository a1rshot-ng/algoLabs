/*
 *      Common messages used in Interactive mode (CLI)
 */

#pragma once

#define MSG_INTRO "Weighted graph CLI\t (Lab 5) Graph \n"                       \
                  "Type \"help\" to see available commands."

#define MSG_HELP "Available commands:\n"                                        \
                 "    create\n"                                                 \
                 "    random <nodes:uint> <density:float>\n"                    \
                 "    load <file>\n"                                            \
                 "    add node <key:string> <x:int> <y:int>\n"                  \
                 "    add edge <key1:string> <key2:string> <weight:uint>\n"     \
                 "    rm node <key:string>\n"                                   \
                 "    rm edge <key1:string> <key2:string>\n"                    \
                 "    get <key:string>\n"                                       \
                 "    path near <key1:string> <key2:string>\n"                  \
                 "    path best <key1:string> <key2:string>\n"                  \
                 "    tree\n"                                                   \
                 "    print\n"                                                  \
                 "    view\n"                                                   \
                 "    plot\n"                                                   \
                 "    save <file>\n"                                            \
                 "    close\n"                                                  \
                 "    help\n"                                                   \
                 "    exit"

#define MSG_SYNTAXERROR "Syntax Error: check your command and try again.\n"

#define MSG_FILEERROR "File Error: could not open file \"%s\":\n"

#define MSG_MEMFULL "Memory Error: machine's memory is full.\n"

#define MSG_FAILED_MST "Failed to build MST for this graph.\n"

#define MSG_UNKNOWNCMD "Unknown command: \"%s\".\n"

#define MSG_ELNFOUND "Element not found.\n"

#define MSG_DUPKEY "Key Error: can't add a duplicate key.\n"

#define MSG_NEGWEIGHT "Value Error: negative weights are not allowed.\n"

#define MSG_NEGSIZE "Value Error: negative nodes count.\n"

#define MSG_INV_DENSITY "Value Error: invalid density (0 <= d <= 1).\n"

#define MSG_NOTLOADED "Graph is not loaded.\n"

#define MSG_SAVED "Graph saved to \"%s\".\n"

#define MSG_LOADED "Graph loaded from \"%s\".\n"

#define MSG_GENERATED "Generated graph of size %d.\n"
