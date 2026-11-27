#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define CYCLES 1000000

int num_philosophers;
pthread_mutex_t* forks;

void* philosopher(void* arg) {
    int id = *(int*)arg;
    free(arg);

    int left = id;
    int right = (id + 1) % num_philosophers;

    for (int i = 0; i < CYCLES; i++) {
        if (id % 2 == 0) {
            pthread_mutex_lock(&forks[left]);
            pthread_mutex_lock(&forks[right]);
        } else {
            pthread_mutex_lock(&forks[right]);
            pthread_mutex_lock(&forks[left]);
        }

        pthread_mutex_unlock(&forks[right]);
        pthread_mutex_unlock(&forks[left]);
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number_of_philosophers>\n", argv[0]);
        return EXIT_FAILURE;
    }

    num_philosophers = atoi(argv[1]);
    if (num_philosophers <= 0) {
        fprintf(stderr, "Number of philosophers must be > 0\n");
        return EXIT_FAILURE;
    }

    forks = malloc((size_t)num_philosophers * sizeof(pthread_mutex_t));
    pthread_t* philosophers = malloc((size_t)num_philosophers * sizeof(pthread_t));

    if (!forks || !philosophers) {
        perror("malloc");
        free(forks);
        free(philosophers);
        return EXIT_FAILURE;
    }

    for (int i = 0; i < num_philosophers; i++) {
        pthread_mutex_init(&forks[i], NULL);
    }

    for (int i = 0; i < num_philosophers; i++) {
        int* id_ptr = malloc(sizeof(int));
        if (!id_ptr) {
            perror("malloc");
            return EXIT_FAILURE;
        }
        *id_ptr = i;
        pthread_create(&philosophers[i], NULL, philosopher, id_ptr);
    }

    for (int i = 0; i < num_philosophers; i++) {
        pthread_join(philosophers[i], NULL);
        pthread_mutex_destroy(&forks[i]);
    }

    free(forks);
    free(philosophers);
    return 0;
}