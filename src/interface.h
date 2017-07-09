#define DEFAULT_SEED                1

#define DEFAULT_ROUNDS              5

void run_interface();

typedef struct state {
    int seed;
    int rounds;
} *State;

void state_init(State s);

