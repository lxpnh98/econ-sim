#include <stdlib.h>

#include "market_init.h"
#include "market.h"

#define MAX_LBOUND                  5.0

#define MAX_MEAN                    15.0

PriceBelief rand_price_belief()
{
    PriceBelief r;
    r.lower_bound = ((float)rand() / (float)RAND_MAX) * MAX_LBOUND;
    r.upper_bound = r.lower_bound + ((float)rand() / (float)RAND_MAX) * MAX_MEAN;
    return r;
}

void create_agent(Market *m)
{
    int i;
    Agent *new;
    if (m->num_agents == MAX_AGENTS)
        return;
    new = m->agents[m->num_agents] = malloc(sizeof(Agent));
    new->role = rand() % NUM_ROLES;
    for (i = 0; i < NUM_GOODS; i++) {
        new->p[i] = rand_price_belief();
        new->good_quantity[i] = 5.0;
    }
    new->currency = INIT_CURRENCY;
    m->num_agents++;
}

void market_init(State *s)
{
    int i;
    Market *m = (Market *)malloc(sizeof(Market));
    for (i = 0; i < NUM_GOODS; i++) {
        m->num_bids[i] = 0;
        m->num_asks[i] = 0;
        m->mean[i] = (MAX_LBOUND + MAX_MEAN / 2.0);
    }
    m->num_agents = 0;
    while (m->num_agents < MAX_AGENTS) {
        create_agent(m);
    }
    m->next_round = NULL;
    s->first_round = m;
    s->current_round = s->first_round;
}

