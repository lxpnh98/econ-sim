#ifndef MARKET
#define MARKET

#define MAX_AGENTS                  10

#define NUM_GOODS                   4

#define NUM_ROLES                   4

#define MAX_REQUESTS                500

typedef struct bid Bid;

typedef struct ask Ask;

typedef enum role {
    WOODWORKER,
    MINER,
    FARMER,
    FACTORY_WORKER
} Role;

typedef struct {
    float upper_bound;
    float lower_bound;
} PriceBelief;

typedef struct agent {
    Role role;
    PriceBelief p[NUM_GOODS];
    float good_quantity[NUM_GOODS];
    float currency;
} Agent;

typedef struct {
    int num_agents;
    Agent *agents[MAX_AGENTS];
    int num_bids;
    Bid *bids[MAX_REQUESTS];
    int num_asks;
    Ask *asks[MAX_REQUESTS];
    float mean;
} Market;

void update_market(Market *m);

#endif
