#include <stdlib.h>

#define BETWEEN(X, A, B) ((X < A) ? A : ((X > B) ? B : X))

#define MAX2(A, B) ((A > B) ? A : B)

#define NUM_GOODS           4

#define MAX_AGENTS          10

#define MAX_REQUESTS        50

typedef enum {
    WOODWORKER;
    MINER;
    FARMER;
    FACTORY_WORKER;
} Role;

typedef enum {
    WOOD,
    IRON,
    FOOD,
    PAPER
} Good;

typedef struct {
    float upper_bound;
    float lower_bound;
} PriceBelief;

typedef struct {
    Role role;
    PriceBelief p[NUM_GOODS];
    int good_quantity[NUM_GOODS];
} Agent;

typedef struct {
    Agent *agent;
    Good good;
    float price;
    int quantity;
} Bid;

typedef struct {
    Agent *agent;
    Good good;
    float price;
    int quantity;
} Ask;

typedef struct {
    Agent agents[MAX_AGENTS];
    int num_bids;
    Bid bids[MAX_REQUESTS];
    int num_asks;
    Ask asks[MAX_REQUESTS];
} Market;

float price_guess(PriceBelief p)
{
    float random = (float)rand() / (float)RAND_MAX;
    float diff = p.upper_bound - p.lower_bound;
    return p.lower_bound + random * diff;
}

int determine_sale_quantity(float mean, PriceBelief p, int excess)
{
    float avg_belief = (p.lower_bound + p.upper_bound) / 2.0;
    float diff = p.upper_bound - p.lower_bound;
    float favorability = 0.5 + (mean - avg_belief) / diff;
    favorability = BETWEEN(favorability, 0.0, 1.0);
    return favorability * excess;
}

int determine_purchase_quantity(float mean, PriceBelief p, int available)
{
    float avg_belief = (p.lower_bound + p.upper_bound) / 2.0;
    float diff = p.upper_bound - p.lower_bound;
    float favorability = 0.5 - (mean - avg_belief) / diff;
    favorability = BETWEEN(favorability, 0.0, 1.0);
    return favorability * available;
}

void perform_consumption(Agent *a)
{
    a->good_quantity[FOOD] = MAX2(0, --(a->good_quantity[FOOD]));
    a->good_quantity[PAPER] = MAX2(0, --(a->good_quantity[PAPER]));
}

void perform_production(Agent *a)
{
    switch(a->role) {
        case WOODWORKER:
            a->good_quantity[IRON] = MAX2(0, --(a->good_quantity[IRON]));
            a->good_guantity[WOOD] += 2;
            break;
        case MINER:
            a->good_quantity[FOOD] = MAX2(0, --(a->good_quantity[FOOD]));
            a->good_guantity[IRON] += 2;
            break;
        case FARMER:
            a->good_quantity[WOOD] = MAX2(0, --(a->good_quantity[WOOD]));
            a->good_guantity[FOOD] += 2;
            break;
        case FACTORY_WORKER:
            a->good_quantity[WOOD] = MAX2(0, --(a->good_quantity[WOOD]));
            a->good_guantity[PAPER] += 2;
            break;
    }
}

void create_bids(Market *m, Agent a)
{
}

void create_asks(Market *m, Agent a)
{
}

void resolve_trades(Market *m)
{
}

void update_price_beliefs(Market *m)
{
}

void update_market(Market *m)
{
    int i;
    for (i = 0; i < MAX_AGENTS; i++) {
        perform_consumption(&(m->agents[i]));
        perform_production(&(m->agents[i]));
        create_bids(m, m->agents[i]);
        create_asks(m, m->agents[i]);
    }
    resolve_trades(m);
    update_price_beliefs(m);
}

int main()
{
    return EXIT_SUCCESS;
}
