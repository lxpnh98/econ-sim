#include <stdio.h>
#include <stdlib.h>

#include "interface.h"
#include "market.h"
#include "market_init.h"
#include "display.h"

void state_init(State *s)
{
    s->seed = DEFAULT_SEED;
    s->rounds = DEFAULT_ROUNDS;
    s->first_round = s->current_round = NULL;
}

Market *free_market(Market *m)
{
    int i, g;
    Market *next;
    for (i = 0; i < m->num_agents; i++) {
        free(m->agents[i]);
    }
    for (g = 0; g < NUM_GOODS; g++) {
        for (i = 0; i < m->num_bids[g]; i++) {
            free(m->bids[g][i]);
        }
        for (i = 0; i < m->num_asks[g]; i++) {
            free(m->asks[g][i]);
        }
    }
    next = m->next_round;
    free(m);
    return next;
}

void free_markets(State *s)
{
    Market *aux;
    for (aux = s->first_round; aux != NULL;) {
        aux = free_market(aux);
    }
    s->first_round = s->current_round = NULL;
}

int run_sim(State *s, char **args)
{
    (void)args;
    int n;
    srand(s->seed);
    if (s->first_round != NULL) {
        free_markets(s);
    }
    market_init(s);
    print_market_info(s->first_round, 1);
    for (n = 0; n < s->rounds; n++) {
        update_market(s);
        printf("\n---------------\n");
        printf("Round complete.");
        printf("\n---------------\n\n");
    }
    free_markets(s);
    return 0;
}

int step_sim(State *s, char **args)
{
    int i;
    int argc = 1;
    int rounds;
    if (args[1] == NULL) {
        rounds = 1;
        argc = 0;
    } else if (sscanf(args[1], "%d", &rounds) != 1) {
        printf("step: invalid argument\n");
        return argc;
    }

    if (s->first_round == NULL) {
        market_init(s);
    }
    for (i = 0; i < rounds; i++) {
        update_market(s);
        printf("\n---------------\n");
        printf("Round complete.");
        printf("\n---------------\n\n");
    }
    return argc;
}

int end_sim(State *s, char **args)
{
    (void)args;
    if (s->first_round != NULL) {
        free_markets(s);
    }
    return 0;
}

int exit_sim(State *s, char **args)
{
    (void)args;
    if (s->first_round != NULL) {
        free_markets(s);
    }
    exit(0);
    return 0;
}

int set_seed(State *s, char **args)
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

int set_rounds(State *s, char **args)
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

int display_sim(State *s, char **args)
{
    (void)args;
    if (s->current_round != NULL) {
        print_market_info(s->current_round, 0);
    } else {
        printf("display: market uninitialzed\n");
    }
    return 0;
}

int print_help(State *s, char **args)
{
    (void)s;
    (void)args;
    printf("Available commands:\n");
    printf("run - run simulation\n");
    printf("step - simulate n rounds (no argument simulates a single round)\n");
    printf("end - end the current simulation\n");
    printf("set_seed - set the seed at the start of the simulation\n");
    printf("set_rounds - set the number of rounds of the simulation\n");
    printf("display - display simulation info for current round\n");
    printf("exit - exit program\n");
    return 0;
}

struct {
    char *token;
    int (*f)(State *, char **);
} function_table[] = {
    {"run", run_sim},
    {"step", step_sim},
    {"end", end_sim},
    {"set_seed", set_seed},
    {"set_rounds", set_rounds},
    {"exit", exit_sim},
    {"help", print_help},
    {"display", display_sim},
    {NULL, NULL}
};

