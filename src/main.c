#include <stdio.h>
#include <stdlib.h>

#include "market.h"
#include "market_init.h"
#include "display.h"

#define ROUNDS                      5

#define SEED                        1

int main()
{
    int n;
    Market m;
    srand(SEED);
    market_init(&m);
    print_market_info(&m, 1);
    for (n = 0; n < ROUNDS; n++) {
        update_market(&m);
        //print_market_info(&m);
        printf("\n---------------\n");
        printf("Round complete.");
        printf("\n---------------\n\n");
    }
    return EXIT_SUCCESS;
}
