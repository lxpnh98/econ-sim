#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "market.h"
#include "market_init.h"
#include "display.h"

#define MAX_BUFFER                  1024

#define ROUNDS                      5

#define DEFAULT_SEED                1

int run_sim(char **args)
{
    (void)args;
    int n;
    Market m;
    market_init(&m);
    print_market_info(&m, 1);
    for (n = 0; n < ROUNDS; n++) {
        update_market(&m);
        printf("\n---------------\n");
        printf("Round complete.");
        printf("\n---------------\n\n");
    }
    return 0;
}

int exit_sim(char **args)
{
    (void)args;
    exit(0);
    return 0;
}

int set_seed(char **args)
{
    int seed;
    if (args[1] == NULL) {
        printf("set_seed: missing argument\n");
        return 0;
    } else if (sscanf(args[1], "%d", &seed) != 1) {
        printf("set_seed: invalid argument\n");
    } else {
        srand(seed);
    }
    return 1;
}

struct {
    char *token;
    int (*f)(char **);
} function_table[] = {
    {"run", run_sim},
    {"exit", exit_sim},
    {"set_seed", set_seed},
    {NULL, NULL}
};

int main()
{
    int tokens, i, j;
    char command[MAX_BUFFER];
    char *args[MAX_BUFFER];
    srand(DEFAULT_SEED);
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
                        i += (function_table[j].f)(args + i);
                        break;
                    }
                }
                if (function_table[j].token == NULL) {
                    printf("%s: invalid command\n", args[i]);
                }
            }
        }
    }
    return EXIT_SUCCESS;
}
