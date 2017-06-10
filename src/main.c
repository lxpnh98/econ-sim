#include <stdlib.h>

#define BETWEEN(X, A, B) ((X < A) ? A : ((X > B) ? B : X))

struct bid {
    float price;
    int quantity;
};

struct ask {
    float price;
    int quantity;
};

struct price_belief {
    float upper_bound;
    float lower_bound;
};

float price_guess(struct price_belief p)
{
    float random = (float)rand() / (float)RAND_MAX;
    float diff = p.upper_bound - p.lower_bound;
    return p.lower_bound + random * diff;
}

int determine_sale_quantity(float mean, struct price_belief p, int excess)
{
    float avg_belief = (p.lower_bound + p.upper_bound) / 2.0;
    float diff = p.upper_bound - p.lower_bound;
    float favorability = 0.5 + (mean - avg_belief) / diff;
    favorability = BETWEEN(favorability, 0.0, 1.0);
    return favorability * excess;
}

int determine_purchase_quantity(float mean, struct price_belief p, int available)
{
    float avg_belief = (p.lower_bound + p.upper_bound) / 2.0;
    float diff = p.upper_bound - p.lower_bound;
    float favorability = 0.5 - (mean - avg_belief) / diff;
    favorability = BETWEEN(favorability, 0.0, 1.0);
    return favorability * available;
}

int main()
{
    return EXIT_SUCCESS;
}
