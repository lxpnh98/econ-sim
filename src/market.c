#include <stdio.h>
#include <stdlib.h>

#include "market.h"
#include "market_init.h"
#include "display.h"
#include "interface.h"

#define MAX_FOOD                    50

#define TRADE_SUCCESS               1

#define TRADE_FAILURE               0

#define MEAN_SHIFT                  0.10

#define INTERVAL_ADJUSTMENT         0.10

#define MEAN_ROLLING_AVG            10

#define BETWEEN(X, A, B)            ((X < A) ? A : ((X > B) ? B : X))

#define MAX2(A, B)                  ((A > B) ? A : B)

#define MIN2(A, B)                  ((A < B) ? A : B)

float price_guess(PriceBelief p)
{
    float random = (float)rand() / (float)RAND_MAX;
    float diff = p.upper_bound - p.lower_bound;
    return p.lower_bound + random * diff;
}

float determine_sale_quantity(float mean, PriceBelief p, float excess)
{
    float avg_belief = (p.lower_bound + p.upper_bound) / 2.0;
    float diff = p.upper_bound - p.lower_bound;
    float favorability = 0.5 + (mean - avg_belief) / diff;
    favorability = BETWEEN(favorability, 0.0, 1.0);
    return favorability * excess;
}

float determine_purchase_quantity(float mean, PriceBelief p, float available)
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
    if (m->num_bids[g] < MAX_REQUESTS && quantity > 0) {
        if ((m->bids[g][m->num_bids[g]] = (Bid *)malloc(sizeof(Bid))) == NULL) {
            perror("unable to create bid");
            exit(1);
        }
        m->bids[g][m->num_bids[g]]->agent = a;
        m->bids[g][m->num_bids[g]]->price = price;
        m->bids[g][m->num_bids[g]]->init_quantity = quantity;
        m->bids[g][m->num_bids[g]]->quantity = quantity;
        m->bids[g][m->num_bids[g]]->status = UNDETERMINED;
        m->num_bids[g]++;
    }
}

float get_max_quantity(float currency, float price)
{
    return currency / price;
}

void create_bids(Market *m, Agent *a)
{
    switch(a->role) {
        default:
            create_bid(m, a, FOOD, price_guess(a->p[FOOD]), determine_purchase_quantity(m->mean[FOOD], a->p[FOOD], get_max_quantity(a->currency, m->mean[FOOD])));
            break;
    }
}

void create_ask(Market *m, Agent *a, Good g, float price, float quantity)
{
    if (m->num_asks[g] < MAX_REQUESTS && quantity > 0) {
        if ((m->asks[g][m->num_asks[g]] = (Ask *)malloc(sizeof(Ask))) == NULL) {
            perror("unable to create ask");
            exit(1);
        }
        m->asks[g][m->num_asks[g]]->agent = a;
        m->asks[g][m->num_asks[g]]->price = price;
        m->asks[g][m->num_asks[g]]->init_quantity = quantity;
        m->asks[g][m->num_asks[g]]->quantity = quantity;
        m->asks[g][m->num_asks[g]]->status = UNDETERMINED;
        m->num_asks[g]++;
    }
}

void create_asks(Market *m, Agent *a)
{
    switch(a->role) {
        default:
            create_ask(m, a, FOOD, price_guess(a->p[FOOD]), determine_sale_quantity(m->mean[FOOD], a->p[FOOD], a->good_quantity[FOOD]));
            break;
    }
}

int cmp_bid(const void *o1, const void *o2)
{
    float p1 = (*(Bid **)o1)->price;
    float p2 = (*(Bid **)o2)->price;
    float r = (p2 - p1);
    if (r < 0.0)
        return -1;
    else if (r == 0.0)
        return 0;
    else
        return 1;
}

int cmp_ask(const void *o1, const void *o2)
{
    float p1 = (*(Ask **)o1)->price;
    float p2 = (*(Ask **)o2)->price;
    float r = (p1 - p2);
    if (r < 0.0)
        return -1;
    else if (r == 0.0)
        return 0;
    else
        return 1;
}

void update_price_model(Agent *a, Good g, float price, int success)
{
    float m = (a->p[g].lower_bound + a->p[g].upper_bound) / 2.0;
    if (success) {
        a->p[g].lower_bound += (m - a->p[g].lower_bound) * INTERVAL_ADJUSTMENT;
        a->p[g].upper_bound += (m - a->p[g].upper_bound) * INTERVAL_ADJUSTMENT;
    } else {
        a->p[g].lower_bound += (price - m) * MEAN_SHIFT;
        a->p[g].upper_bound += (price - m) * MEAN_SHIFT;
        a->p[g].lower_bound -= (m - a->p[g].lower_bound) * INTERVAL_ADJUSTMENT;
        a->p[g].upper_bound -= (m - a->p[g].upper_bound) * INTERVAL_ADJUSTMENT;
    }
}

void resolve_offers(Market *m)
{
    int i, j;
    int g;
    float quantity;
    float price;
    double total_price, total_quantity;
    Bid *top_bid;
    Ask *top_ask;
    for (g = 0; g < NUM_GOODS; g++) {
        qsort(m->bids[g], m->num_bids[g], sizeof(Bid *), cmp_bid);
        qsort(m->asks[g], m->num_asks[g], sizeof(Ask *), cmp_ask);
        total_price = 0.0;
        total_quantity = 0.0;
        for (i = j = 0; i < m->num_bids[g] && j < m->num_asks[g];) {
            // Set up trade
            top_bid = m->bids[g][i];
            top_bid->status = SUCCESSFUL;
            top_ask = m->asks[g][j];
            top_ask->status = SUCCESSFUL;
            price = (top_bid->price + top_ask->price) / 2.0;
            quantity = MIN2(MIN2(top_bid->quantity, top_ask->quantity), top_bid->agent->currency / price);
            total_price += price * quantity;
            total_quantity += quantity;

            // Exchange
            if (top_bid->quantity < top_ask->quantity) {
                top_ask->quantity -= top_bid->quantity;
                top_bid->quantity = 0.0;
            } else if (top_bid->quantity > top_ask->quantity) {
                top_bid->quantity -= top_ask->quantity;
                top_ask->quantity = 0.0;
            } else {
                top_bid->quantity = 0.0;
                top_ask->quantity = 0.0;
            }
            top_bid->agent->good_quantity[g] += quantity;
            top_ask->agent->good_quantity[g] -= quantity;
            top_bid->agent->currency -= price * quantity;
            top_ask->agent->currency += price * quantity;

            // Update price models
            update_price_model(top_bid->agent, g, price, TRADE_SUCCESS);
            update_price_model(top_ask->agent, g, price, TRADE_SUCCESS);

            if (top_bid->quantity == 0.0) {
                i++;
            }
            if (top_ask->quantity == 0.0) {
                j++;
            }
        }

        // Deal with failed offers
        if (total_quantity != 0) {
            m->mean[g] = (m->mean[g] * (MEAN_ROLLING_AVG - 1) + total_price / total_quantity) / MEAN_ROLLING_AVG;
        }
        while (i < m->num_bids[g]) {
            top_bid = m->bids[g][i];
            if (top_bid->status != SUCCESSFUL) {
                top_bid->status = UNSUCCESSFUL;
            }
            update_price_model(top_bid->agent, g, m->mean[g], TRADE_FAILURE);
            i++;
        }
        while (j < m->num_asks[g]) {
            top_ask = m->asks[g][j];
            if (top_ask->status != SUCCESSFUL) {
                top_ask->status = UNSUCCESSFUL;
            }
            update_price_model(top_ask->agent, g, m->mean[g], TRADE_FAILURE);
            j++;
        }
    }
}

void replace_agents(Market *m)
{
    int i, g;
    Agent *a;
    for (i = 0; i < MAX_AGENTS; i++) {
        a = m->agents[i];
        if (a->currency == 0.0 || a->good_quantity[FOOD] == 0.0) {
            a->role = rand() % NUM_ROLES;
            for (g = 0; g < NUM_GOODS; g++) {
                a->p[g] = rand_price_belief();
                a->good_quantity[g] = 5.0;
            }
            a->currency = INIT_CURRENCY;
        }
    }
}

void make_next_round(Market *m)
{
    int i, j;
    Agent *a;
    m->next_round = (Market *)malloc(sizeof(Market));
    for (i = 0; i < NUM_GOODS; i++) {
        m->next_round->num_bids[i] = 0;
        m->next_round->num_asks[i] = 0;
        m->next_round->mean[i] = m->mean[i];
    }
    m->next_round->num_agents = MAX_AGENTS;
    for (i = 0; i < MAX_AGENTS; i++) {
        a = (Agent *)malloc(sizeof(Agent));
        a->role = m->agents[i]->role;
        for (j = 0; j < NUM_GOODS; j++) {
            a->p[j] = m->agents[i]->p[j];
            a->good_quantity[j] = m->agents[i]->good_quantity[j];
        }
        a->currency = m->agents[i]->currency;
        m->next_round->agents[i] = a;
    }
    m->next_round->next_round = NULL;
}

void update_market(State *s)
{
    int i;
    Market *m;
    make_next_round(s->current_round);
    s->current_round = s->current_round->next_round;
    m = s->current_round;
    for (i = 0; i < MAX_AGENTS; i++) {
        perform_consumption(m->agents[i]);
        perform_production(m->agents[i]);
        create_bids(m, m->agents[i]);
        create_asks(m, m->agents[i]);
    }
    print_market_info(m, 1);
    resolve_offers(m);
    replace_agents(m);
}

