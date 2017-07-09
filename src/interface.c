#include <stdio.h>
#include <stdlib.h>

#include "interface.h"
#include "market.h"
#include "market_init.h"
#include "display.h"

void state_init(State s)
{
    s->seed = DEFAULT_SEED;
    s->rounds = DEFAULT_ROUNDS;
}

int run_sim(State s, char **args)
{
    (void)args;
    int n;
    Market m;
    srand(s->seed);
    market_init(&m);
    print_market_info(&m, 1);
    for (n = 0; n < s->rounds; n++) {
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

int set_rounds(State s, char **args)
{
    int rounds;
    if (args[1] == NULL) {
        printf("set_rounds: missing argument\n");
        return 0;
    } else if (sscanf(args[1], "%d", &rounds) != 1) {
        printf("set_rounds: invalid argument\n");
    } else {
        s->rounds = rounds;
    }
    return 1;
}

int print_help(State s, char **args)
{
    (void)s;
    (void)args;
    printf("Available commands:\n");
    printf("run - run simulation\n");
    printf("set_seed - set the seed at the start of the simulation\n");
    printf("set_rounds - set the number of rounds of the simulation\n");
    printf("exit - exit program\n");
    return 0;
}

struct {
    char *token;
    int (*f)(State, char **);
} function_table[] = {
    {"run", run_sim},
    {"set_seed", set_seed},
    {"set_rounds", set_rounds},
    {"exit", exit_sim},
    {"help", print_help},
    {NULL, NULL}
};

