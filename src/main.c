#include <stdio.h>
#include <stdlib.h>

#include "market.h"
#include "display.h"

#define INIT_CURRENCY               5.0

#define MAX_LBOUND                  5.0

#define MAX_MEAN                    10.0 

#define ROUNDS                      10

#define SEED                        1

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

void market_init(Market *m)
{
    m->num_bids = m->num_asks = 0;
    m->mean = (MAX_LBOUND + MAX_MEAN / 2.0);
    m->num_agents = 0;
    while (m->num_agents < MAX_AGENTS) {
        create_agent(m);
    }
}


int main()
{
    int n;
    Market m;
    srand(SEED);
    market_init(&m);
    print_market_info(&m);
    for (n = 0; n < ROUNDS; n++) {
        update_market(&m);
        print_market_info(&m);
        printf("Round complete.\n");
    }
    return EXIT_SUCCESS;
}
