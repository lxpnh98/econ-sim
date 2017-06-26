#include <stdio.h>
#include <stdlib.h>

#include "market.h"

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
    /*
    printf("mean: %f\n", mean);
    printf("avg_b: %f\n", avg_belief);
    printf("diff: %f\n", diff);
    printf("fav: %f\n", favorability);
    printf("excess: %f\n", excess);
    */
    return favorability * excess;
}

float determine_purchase_quantity(float mean, PriceBelief p, float available)
{
    float avg_belief = (p.lower_bound + p.upper_bound) / 2.0;
    float diff = p.upper_bound - p.lower_bound;
    float favorability = 0.5 - (mean - avg_belief) / diff;
    favorability = BETWEEN(favorability, 0.0, 1.0);
    /*
    printf("mean: %f\n", mean);
    printf("avg_b: %f\n", avg_belief);
    printf("diff: %f\n", diff);
    printf("fav: %f\n", favorability);
    printf("available: %f\n", available);
    */
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
    if (m->num_bids < MAX_REQUESTS && quantity > 0) {
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
    //printf("Purchase quantity: %f\n", determine_purchase_quantity(m->mean, a->p[FOOD], MAX_FOOD));
    switch(a->role) {
        default:
            create_bid(m, a, FOOD, price_guess(a->p[FOOD]), determine_purchase_quantity(m->mean, a->p[FOOD], MAX_FOOD));
            break;
    }
}

void create_ask(Market *m, Agent *a, Good g, float price, float quantity)
{
    if (m->num_asks < MAX_REQUESTS && quantity > 0) {
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
    //printf("Sale quantity: %f\n", determine_sale_quantity(m->mean, a->p[FOOD], a->good_quantity[FOOD]));
    switch(a->role) {
        default:
            create_ask(m, a, FOOD, price_guess(a->p[FOOD]), determine_sale_quantity(m->mean, a->p[FOOD], a->good_quantity[FOOD]));
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

void update_price_model(Agent *a, Good g, float price, int success)
{
    float m = (a->p[g].lower_bound + a->p[g].upper_bound) / 2.0;
    a->p[g].lower_bound += (price - m) * MEAN_SHIFT;
    a->p[g].upper_bound += (price - m) * MEAN_SHIFT;
    if (success) {
        a->p[g].lower_bound += (m - a->p[g].lower_bound) * INTERVAL_ADJUSTMENT;
        a->p[g].upper_bound += (m - a->p[g].upper_bound) * INTERVAL_ADJUSTMENT;
    } else {
        a->p[g].lower_bound -= (m - a->p[g].lower_bound) * INTERVAL_ADJUSTMENT;
        a->p[g].upper_bound -= (m - a->p[g].upper_bound) * INTERVAL_ADJUSTMENT;
    }
}

void resolve_offers(Market *m)
{
    float quantity;
    float price, total_price = 0.0, total_quantity = 0.0;
    Bid *top_bid;
    Ask *top_ask;
    qsort(m->bids, m->num_bids, sizeof(Bid), cmp_offer); 
    qsort(m->asks, m->num_asks, sizeof(Ask), cmp_offer); 
    while (m->num_bids > 0 && m->num_asks > 0) {
        printf("Bids: %d\nAsks: %d\n", m->num_bids, m->num_asks);
        // Set up trade
        top_bid = m->bids[0];
        top_ask = m->asks[0];
        quantity = MIN2(top_bid->quantity, top_ask->quantity);
        price = (top_bid->price + top_ask->price) / 2.0;
        total_price += price;
        total_quantity += quantity;

        printf("===\n");
        printf("Top bid quantity: %f\n", top_bid->quantity);
        printf("Top ask quantity: %f\n", top_ask->quantity);
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
        printf("Top bid quantity: %f\n", top_bid->quantity);
        printf("Top ask quantity: %f\n", top_ask->quantity);
        printf("===\n");

        // Exchange
        //top_bid->quantity -= quantity;
        //top_ask->quantity -= quantity;
        top_bid->agent->good_quantity[top_bid->good] += quantity;
        top_ask->agent->good_quantity[top_ask->good] -= quantity;
        top_bid->agent->currency -= price * quantity;
        top_ask->agent->currency += price * quantity;

        // Update price models
        update_price_model(top_bid->agent, top_bid->good, price, TRADE_SUCCESS);
        update_price_model(top_ask->agent, top_ask->good, price, TRADE_SUCCESS);

        if (top_bid->quantity == 0) {
            remove_top_bid(m);
        }
        if (top_ask->quantity == 0) {
            remove_top_ask(m);
        }
    }

    // Deal with failed offers
    if (total_quantity != 0) {
        m->mean = price = total_price / total_quantity;
    } else {
        // price not inicialized
        price = 0.0;
    }
    while (m->num_bids > 0) {
        top_bid = m->bids[0];
        update_price_model(top_bid->agent, top_bid->good, price, TRADE_FAILURE);
        remove_top_bid(m);
    }
    while (m->num_asks > 0) {
        top_ask = m->asks[0];
        update_price_model(top_ask->agent, top_ask->good, price, TRADE_FAILURE);
        remove_top_ask(m);
    }
}

void update_market(Market *m)
{
    int i;
    for (i = 0; i < MAX_AGENTS; i++) {
        perform_consumption(m->agents[i]);
        perform_production(m->agents[i]);
        create_bids(m, m->agents[i]);
        create_asks(m, m->agents[i]);
    }
    resolve_offers(m);
    printf("Updated market.\n");
}
