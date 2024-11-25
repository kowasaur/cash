#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//TODO: Make this actually good like exec_gamble in main.c but for now this is fine for now
int coinflip(void)
{
    // Seed the random number generator
    srand(time(NULL));
    
    return rand() % 2;  // Random number: 0 or 1 (heads or tails)
}
