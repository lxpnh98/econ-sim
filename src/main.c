#include <stdlib.h>

#define BETWEEN(X, A, B) ((X < A) ? A : ((X > B) ? B : X))

#define MAX2(A, B) ((A > B) ? A : B)

#define MIN2(A, B) ((A < B) ? A : B)

#define NUM_GOODS           4

#define MAX_AGENTS          100

#define MAX_REQUESTS        500

#define MAX_FOOD            50

#define TRADE_SUCCESS       1

#define TRADE_FAILURE       0

typedef enum {
    WOODWORKER,
    MINER,
    FARMER,
    FACTORY_WORKER
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
    float good_quantity[NUM_GOODS];
    float currency;
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
    Bid *bids[MAX_REQUESTS];
    int num_asks;
    Ask *asks[MAX_REQUESTS];
    float mean;
} Market;

float price_guess(PriceBelief p)
{
    float random = (float)rand() / (float)RAND_MAX;
    float diff = p.upper_bound - p.lower_bound;
    return p.lower_bound + random * diff;
}

int determine_sale_quantity(float mean, PriceBelief p, float excess)
{
    float avg_belief = (p.lower_bound + p.upper_bound) / 2.0;
    float diff = p.upper_bound - p.lower_bound;
    float favorability = 0.5 + (mean - avg_belief) / diff;
    favorability = BETWEEN(favorability, 0.0, 1.0);
    return favorability * excess;
}

int determine_purchase_quantity(float mean, PriceBelief p, float available)
{
    float avg_belief = (p.lower_bound + p.upper_bound) / 2.0;
    float diff = p.upper_bound - p.lower_bound;
    float favorability = 0.5 - (mean - avg_belief) / diff;
    favorability = BETWEEN(favorability, 0.0, 1.0);
    return favorability * available;
}

void perform_consumption(Agent *a)
{
    a->good_quantity[FOOD] = MAX2(0, a->good_quantity[FOOD] - 1);
    a->good_quantity[PAPER] = MAX2(0, a->good_quantity[PAPER] - 1);
}

void perform_production(Agent *a)
{
    switch(a->role) {
        case WOODWORKER:
            a->good_quantity[IRON] = MAX2(0, a->good_quantity[IRON] - 1);
            a->good_quantity[WOOD] += 2;
            break;
        case MINER:
            a->good_quantity[FOOD] = MAX2(0, a->good_quantity[FOOD] - 1);
            a->good_quantity[IRON] += 2;
            break;
        case FARMER:
            a->good_quantity[WOOD] = MAX2(0, a->good_quantity[WOOD] - 1);
            a->good_quantity[FOOD] += 2;
            break;
        case FACTORY_WORKER:
            a->good_quantity[WOOD] = MAX2(0, a->good_quantity[WOOD] - 1);
            a->good_quantity[PAPER] += 2;
            break;
    }
}

void create_bid(Market *m, Agent *a, Good g, float price, float quantity)
{
    if (m->num_bids < MAX_REQUESTS) {
        m->bids[m->num_bids] = (Bid *)malloc(sizeof(Bid));
        m->bids[m->num_bids]->agent = a;
        m->bids[m->num_bids]->good = g;
        m->bids[m->num_bids]->price = price;
        m->bids[m->num_bids]->quantity = quantity;
        m->num_bids++;
    }
}

void create_bids(Market *m, Agent *a)
{
    switch(a->role) {
        default:
            create_bid(m, a, FOOD, price_guess(a->p[FOOD]), determine_purchase_quantity(m->mean, a->p[FOOD], MAX_FOOD));
            break;
    }
}

void create_ask(Market *m, Agent *a, Good g, float price, float quantity)
{
    if (m->num_asks < MAX_REQUESTS) {
        m->asks[m->num_asks] = (Ask *)malloc(sizeof(Ask));
        m->asks[m->num_asks]->agent = a;
        m->asks[m->num_asks]->good = g;
        m->asks[m->num_asks]->price = price;
        m->asks[m->num_asks]->quantity = quantity;
        m->num_asks++;
    }
}

void create_asks(Market *m, Agent *a)
{
    switch(a->role) {
        default:
            create_ask(m, a, FOOD, price_guess(a->p[FOOD]), determine_sale_quantity(m->mean, a->p[FOOD], MAX_FOOD));
            break;
    }
}

int cmp_offer(const void *o1, const void *o2)
{
    return (((Bid *)o1)->price < ((Bid *)o1)->price) - ((Bid *)o1)->price > ((Bid *)o2)->price;
}

void remove_top_bid(Market *m)
{
    int i;
    for (i = 0; i < m->num_bids - 1; i++) {
        m->bids[i] = m->bids[i + 1];
    }
    m->num_bids--;
}

void remove_top_ask(Market *m)
{
    int i;
    for (i = 0; i < m->num_asks - 1; i++) {
        m->asks[i] = m->asks[i + 1];
    }
    m->num_asks--;
}

void update_price_model(Agent *a, float price, int success)
{
}

void resolve_offers(Market *m)
{
    float quantity;
    float price;
    Bid *top_bid;
    Ask *top_ask;
    qsort(m->bids, m->num_bids, sizeof(Bid), cmp_offer); 
    qsort(m->asks, m->num_asks, sizeof(Ask), cmp_offer); 
    while (m->num_bids > 0 && m->num_asks > 0) {
        // Set up trade
        top_bid = m->bids[0];
        top_ask = m->asks[0];
        quantity = MIN2(top_bid->quantity, top_ask->quantity);
        price = (top_bid->price + top_ask->price) / 2.0;

        // Exchange
        top_bid->quantity -= quantity;
        top_ask->quantity -= quantity;
        top_bid->agent->good_quantity[top_bid->good] += quantity;
        top_ask->agent->good_quantity[top_ask->good] -= quantity;
        top_bid->agent->currency -= price * quantity;
        top_ask->agent->currency += price * quantity;

        // Update price models
        update_price_model(top_bid->agent, price, TRADE_SUCCESS);
        update_price_model(top_ask->agent, price, TRADE_SUCCESS);

        if (top_bid->quantity == 0) {
            remove_top_bid(m);
        }
        if (top_ask->quantity == 0) {
            remove_top_ask(m);
        }
    }

    // Deal with failed offers
    while (m->num_bids > 0) {
        update_price_model(top_bid->agent, price, TRADE_FAILURE);
        remove_top_bid(m);
    }

    while (m->num_asks > 0) {
        update_price_model(top_bid->agent, price, TRADE_FAILURE);
        remove_top_ask(m);
    }
}

void update_market(Market *m)
{
    int i;
    for (i = 0; i < MAX_AGENTS; i++) {
        perform_consumption(&(m->agents[i]));
        perform_production(&(m->agents[i]));
        create_bids(m, &(m->agents[i]));
        create_asks(m, &(m->agents[i]));
    }
    resolve_offers(m);
}

int main()
{
    return EXIT_SUCCESS;
}
