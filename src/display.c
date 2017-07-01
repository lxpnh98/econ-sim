#include <stdio.h>

#include "market.h"

void print_agent(Agent *a)
{
    int i;
    printf("Role: %d\n", a->role);
    printf("Goods:\n");
    for (i = 0; i < NUM_GOODS; i++) {
        printf("%d: %f\n", i, a->good_quantity[i]);
    }
    printf("Currency: %f\n", a->currency);
    printf("\n");
}

void print_bid(Bid *b)
{
    printf("Price: %f\n", b->price);
    printf("Quantity: %f\n", b->quantity);
}

void print_ask(Ask *a)
{
    printf("Price: %f\n", a->price);
    printf("Quantity: %f\n", a->quantity);
}

void print_market_info(Market *m)
{
    int i, g;
    printf("Agents: %d\n", m->num_agents);
    for (g = 0; g < NUM_GOODS; g++) {
        printf("Good %d info:\n", g);
        printf("Bids: %d\n", m->num_bids[g]);
        printf("Asks: %d\n", m->num_asks[g]);
        printf("Mean: %f\n", m->mean[g]);
    }
    for (i = 0; i < m->num_agents; i++) {
        print_agent(m->agents[i]);
    }
    for (g = 0; g < NUM_GOODS; g++) {
        printf("Good %d:\n", g);
        printf("Bids:\n");
        for (i = 0; i < m->num_bids[g]; i++) {
            print_bid(m->bids[g][i]);
        }
        printf("Asks:\n");
        for (i = 0; i < m->num_asks[g]; i++) {
            print_ask(m->asks[g][i]);
        }
    }
    printf("\n----------\n");
}
