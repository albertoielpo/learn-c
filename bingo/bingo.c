/**
 * This program has the aim to simulate the bingo extraction phase
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// The cage contains all numbers between 1 and 90 included (Italian Tombola)
#define CAGE_INIT_SIZE 90

/**
 * The place where all numbers are contained
 */
typedef struct
{
    uint8_t numbers[CAGE_INIT_SIZE];
    uint8_t len;
} Cage;

/**
 * init phase, create a cage with numbers from 1 to 90
 */
Cage *init_cage(uint8_t cage_size) {
    Cage *cage = malloc(sizeof(Cage));
    for (uint8_t ii = 0; ii < cage_size; ii++) {
        cage->numbers[ii] = ii + 1;
    }

    cage->len = cage_size;

    return cage;
}

/**
 * Free cage heap memory
 */
void destory_cage(Cage *cage) {
    free(cage);
}

/**
 * Extract a number from a cage and resize
 */
uint8_t extract_number(Cage *cage) {
    uint8_t idx = rand() % (cage->len);
    uint8_t extracted = cage->numbers[idx];

    // swap data
    cage->numbers[idx] = cage->numbers[(cage->len) - 1];

    // size -1
    cage->len--;

    return extracted;
}

int main(void) {
    printf("Bingo! \n");

    // Seed the random number generator
    srand(time(NULL));

    // init
    Cage *cage = init_cage(CAGE_INIT_SIZE);

    // extract phase, choose a random number and print to stdout
    while (cage->len > 0) {
        uint8_t extracted = extract_number(cage);
        printf("> %hu\n", extracted);
    }

    printf("All numbers have been extracted\n");

    // free memory
    destory_cage(cage);

    return EXIT_SUCCESS;
}
