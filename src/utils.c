#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

char waitkey(void) {
    while(kbhit()) {
        cgetc();
    }
    return cgetc();
}

unsigned int drand(unsigned int max) {
    
    unsigned int x;

    do {
        x= rand();
    } while (x >= (RAND_MAX - RAND_MAX % max));

    x %= max;

    return x;
}