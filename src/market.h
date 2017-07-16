#ifndef MARKET
#define MARKET

#include "interface.h"

#define MAX_AGENTS                  10

#define NUM_GOODS                   4

#define NUM_ROLES                   4

#define MAX_REQUESTS                500

typedef enum {UNDETERMINED, SUCCESSFUL, UNSUCCESSFUL} OfferStatus;

typedef struct agent Agent;

typedef enum good {
    WOOD,
    IRON,
    FOOD,
    PAPER
} Good;

typedef struct bid {
    Agent *agent;
    float price;
    float init_quantity;
    float quantity;
    OfferStatus status;
} Bid;

typedef struct ask {
    Agent *agent;
    float price;
    float init_quantity;
    float quantity;
    OfferStatus status;
} Ask;

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

typedef struct market {
    int num_agents;
    Agent *agents[MAX_AGENTS];
    int num_bids[NUM_GOODS];
    Bid *bids[NUM_GOODS][MAX_REQUESTS];
    int num_asks[NUM_GOODS];
    Ask *asks[NUM_GOODS][MAX_REQUESTS];
    double mean[NUM_GOODS];
    struct market *next_round;
} Market;

void update_market(State *s);

#endif
