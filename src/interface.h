#ifndef INTERFACE
#define INTERFACE

#define DEFAULT_SEED                1

#define DEFAULT_ROUNDS              5

typedef struct market Market;

void run_interface();

typedef struct state {
    int seed;
    int rounds;
    Market *first_round;
    Market *current_round;
} State;

void state_init(State *s);

#endif
