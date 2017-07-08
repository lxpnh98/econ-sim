#include <stdio.h>
#include <stdlib.h>

#include "interface.h"
#include "market.h"
#include "market_init.h"
#include "display.h"

#define ROUNDS                      5

void state_init(State s)
{
    s->seed = DEFAULT_SEED;
}

int run_sim(State s, char **args)
{
    (void)args;
    int n;
    Market m;
    srand(s->seed);
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

int exit_sim(State s, char **args)
{
    (void)s;
    (void)args;
    exit(0);
    return 0;
}

int set_seed(State s, char **args)
{
    int seed;
    if (args[1] == NULL) {
        printf("set_seed: missing argument\n");
        return 0;
    } else if (sscanf(args[1], "%d", &seed) != 1) {
        printf("set_seed: invalid argument\n");
    } else {
        s->seed = seed;
    }
    return 1;
}

struct {
    char *token;
    int (*f)(State, char **);
} function_table[] = {
    {"run", run_sim},
    {"exit", exit_sim},
    {"set_seed", set_seed},
    {NULL, NULL}
};

