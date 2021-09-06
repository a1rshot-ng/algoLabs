#pragma once

#define MSG_INTRO "Binary Tree CLI\t (4.a) Simple tree \n"  \
                  "Type \"help\" to see available commands."

#define MSG_HELP "Available commands:\n"                    \
                 "    add <key:int> <float> <string>\n"     \
                 "    rm <key:int> <version:int>\n"         \
                 "    get <key:int> <version:int>\n"        \
                 "    min <version:int>\n"                  \
                 "    range <key_min:int> <key_max:int>\n"  \
                 "    view\n"                               \
                 "    save <file>\n"                        \
                 "    load <file>\n"                        \
                 "    close\n"                              \
                 "    help\n"                               \
                 "    exit\n"

#define MSG_SYNTAXERROR "Syntax Error: check your command and try again.\n"

#define MSG_FILEERROR "File Error: could not open file \"%s\".\n"

#define MSG_UNKNOWNCMD "Unknown command: \"%s\".\n"

#define MSG_NOTFOUND "Element not found.\n"

#define MSG_SAVED "Tree saved to \"%s\".\n"

#define MSG_LOADED "Tree loaded from \"%s\".\n"