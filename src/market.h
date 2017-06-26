#ifndef MARKET
#define MARKET

#define NUM_GOODS                   4

#define NUM_ROLES                   4

#define MAX_AGENTS                  10

#define MAX_REQUESTS                500

#define MAX_FOOD                    50

#define TRADE_SUCCESS               1

#define TRADE_FAILURE               0

#define MEAN_SHIFT                  0.05

#define INTERVAL_ADJUSTMENT         0.05

#define BETWEEN(X, A, B) ((X < A) ? A : ((X > B) ? B : X))

#define MAX2(A, B) ((A > B) ? A : B)

#define MIN2(A, B) ((A < B) ? A : B)

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
    float quantity;
} Bid;

typedef struct {
    Agent *agent;
    Good good;
    float price;
    float quantity;
} Ask;

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
