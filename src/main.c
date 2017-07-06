#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "market.h"
#include "market_init.h"
#include "display.h"

#define MAX_BUFFER                  1024

#define ROUNDS                      5

#define SEED                        1

void run_sim()
{
    int n;
    Market m;
    srand(SEED);
    market_init(&m);
    print_market_info(&m, 1);
    for (n = 0; n < ROUNDS; n++) {
        update_market(&m);
        printf("\n---------------\n");
        printf("Round complete.");
        printf("\n---------------\n\n");
    }
}

int main()
{
    int tokens, i;
    char command[MAX_BUFFER];
    char *args[MAX_BUFFER];
    while (1) {
        printf("> ");
        if(fgets(command, MAX_BUFFER, stdin) != NULL) {
            i = 0;
            args[i++] = strtok(command, " ");
            while ((args[i++] = strtok(NULL, " ")));
            tokens = i - 1;
            if (tokens == 1) {
                for (i = 0; (*args)[i] != '\0'; i++);
                (*args)[i - 1] = '\0';
            }
            for (i = 0; i < tokens; i++) {
                if (strcmp(args[i], "run") == 0)
                    run_sim();
                if (strcmp(args[i], "exit") == 0)
                    exit(0);
            }
        }
    }
    return EXIT_SUCCESS;
}
