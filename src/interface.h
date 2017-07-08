#define DEFAULT_SEED                1

void run_interface();

typedef struct state {
    int seed;
} *State;

void state_init(State s);

