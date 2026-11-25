#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "tas.h"

#define CYCLES 1000000

int num_philosophers;
tas_t* forks;

void *philosopher(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < CYCLES; i++) {
        // Thinking
        printf("Philosopher %d is thinking.\n", id);
        // Pick up forks
        lock(&forks[id]);
        lock(&forks[(id + 1) % num_philosophers]);
        // Eating
        printf("Philosopher %d is eating.\n", id);
        // Put down forks
        unlock(&forks[(id + 1) % num_philosophers]);
        unlock(&forks[id]);
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number_of_philosophers>\n", argv[0]);
        return EXIT_FAILURE;
    }

    num_philosophers = atoi(argv[1]);
    forks = malloc(num_philosophers * sizeof(tas_t));
    pthread_t* philosophers = malloc(num_philosophers * sizeof(pthread_t));
    int* ids = malloc(num_philosophers * sizeof(int));

    for (int i = 0; i < num_philosophers; i++) {
        tas_init(&forks[i]);
        ids[i] = i;
        pthread_create(&philosophers[i], NULL, philosopher, &ids[i]);
    }

    for (int i = 0; i < num_philosophers; i++) {
        pthread_join(philosophers[i], NULL);
    }

    free(forks);
    free(philosophers);
    free(ids);

    return 0;
}
