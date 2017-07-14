#include <stdio.h>

#include "market.h"

#define GOOD(X)                         good_table[X].g

#define GOOD_NAME(X)                    good_table[X].name

struct good_names {
    Good g;
    char *name;
} good_table[NUM_GOODS] = {
    {WOOD, "WOOD"},
    {PAPER, "PAPER"},
    {IRON, "IRON"},
    {FOOD, "FOOD"}
};

void print_agent(Agent *a)
{
    int i;
    printf("Role: %d\n", a->role);
    printf("Goods:\n");
    for (i = 0; i < NUM_GOODS; i++) {
        printf("%s: %f\n", GOOD_NAME(i), a->good_quantity[GOOD(i)]);
    }
    printf("Currency: %f\n", a->currency);
    printf("\n");
}

void print_bid(Bid *b)
{
    printf("Price: %f\n", b->price);
    printf("Quantity: %f\n", b->quantity);
    printf("Status: %d\n", b->status);
}

void print_ask(Ask *a)
{
    printf("Price: %f\n", a->price);
    printf("Quantity: %f\n", a->quantity);
    printf("Status: %d\n", a->status);
}

void print_market_info(Market *m, int bare)
{
    int i, g;
    printf("Agents: %d\n", m->num_agents);
    if (!bare)
        for (i = 0; i < m->num_agents; i++) {
            print_agent(m->agents[i]);
        }
    for (g = 0; g < NUM_GOODS; g++) {
        printf("Mean: %f\n", m->mean[g]);
        printf("Good %s:\n", GOOD_NAME(g));
        printf("Bids: %d\n", m->num_bids[GOOD(g)]);
        if (!bare)
            for (i = 0; i < m->num_bids[GOOD(g)]; i++) {
                print_bid(m->bids[GOOD(g)][i]);
            }
        printf("Asks: %d\n", m->num_asks[GOOD(g)]);
        if (!bare)
            for (i = 0; i < m->num_asks[GOOD(g)]; i++) {
                print_ask(m->asks[GOOD(g)][i]);
            }
    }
}
