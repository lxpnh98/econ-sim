#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "interface.h"

#define MAX_BUFFER                  1024

extern struct {
    char *token;
    int (*f)(State, char **);
} function_table[];

int main()
{
    int tokens, i, j;
    char command[MAX_BUFFER];
    char *args[MAX_BUFFER];
    struct state s;
    state_init(&s);
    while (1) {
        printf("> ");
        if(fgets(command, MAX_BUFFER, stdin) != NULL) {
            i = 0;
            args[i++] = strtok(command, " ");
            while ((args[i++] = strtok(NULL, " ")));
            tokens = i - 1;
            for (i = 0; args[tokens - 1][i] != '\0'; i++);
            args[tokens - 1][i - 1] = '\0';
            for (i = 0; i < tokens; i++) {
                for (j = 0; function_table[j].token != NULL; j++) {
                    if (strcmp(args[i], function_table[j].token) == 0) {
                        i += (function_table[j].f)(&s, args + i);
                        break;
                    }
                }
                if (function_table[j].token == NULL) {
                    printf("%s: invalid command\n", args[i]);
                    printf("type \"help\" to get the list of available commands\n");
                    break;
                }
            }
        }
    }
    return EXIT_SUCCESS;
}
