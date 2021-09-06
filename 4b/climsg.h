/*
 *      Common messages used in Interactive mode (CLI)
 */

#pragma once

#define MSG_INTRO "Dual-key Tree CLI\t (4.b) K-d tree \n"                       \
                  "Type \"help\" to see available commands."

#define MSG_HELP "Available commands:\n"                                        \
                 "    add <key1:int> <key2:int> <float> <float> <string>\n"     \
                 "    rm <key1:int> <key2:int>\n"                               \
                 "    get <key1:int> <key2:int> <version:int>\n"                \
                 "    max <version:int>\n"                                      \
                 "    range [key1_max:int] [key2_max:int]\n"                    \
                 "    far\n"                                                    \
                 "    view\n"                                                   \
                 "    plot\n"                                                   \
                 "    save <file>\n"                                            \
                 "    load <file>\n"                                            \
                 "    close\n"                                                  \
                 "    help\n"                                                   \
                 "    exit"

#define MSG_SYNTAXERROR "Syntax Error: check your command and try again.\n"

#define MSG_FILEERROR "File Error: could not open file \"%s\":\n"

#define MSG_MEMFULL "Machine's memory is full.\n"

#define MSG_UNKNOWNCMD "Unknown command: \"%s\".\n"

#define MSG_ELNFOUND "Element not found.\n"

#define MSG_SAVED "Tree saved to \"%s\".\n"

#define MSG_LOADED "Tree loaded from \"%s\".\n"