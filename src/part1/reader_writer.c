#include <pthread.h>
#include <semaphore.h>
#include <stdatomic.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>


int N_READERS;
int N_WRITERS;
int temp_ecriture = 0;
int temp_lecture = 0;

pthread_mutex_t mutex_current_readers = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_current_writers = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_compteurs = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t z = PTHREAD_MUTEX_INITIALIZER; // only one waiting reader
sem_t wsem;        // exclusive access to the ressource (init at 1)
sem_t rsem;        // to block readers (init at 1)

volatile size_t current_readers = 0;
volatile size_t current_writers = 0;

int compteur_lecture = 0;
int MAX_LECTURE = 25400;
int compteur_ecrivain = 0;
int MAX_ECRIVAIN = 6400;

void *reader(void *arg);
void *writer(void *arg);

int main(int argc, char *argv[]) {
    if (argc != 3) {
    printf("Mauvais arguments");
    return 1;
}
   
    
    N_WRITERS =  atoi(argv[1]);
    N_READERS = atoi(argv[2]);
    sem_init(&wsem, 0, 1);
    sem_init(&rsem, 0, 1);

    pthread_t ecrivains[N_WRITERS];
    pthread_t lecteurs[N_READERS];

    for (int i = 0; i < N_WRITERS; i++){
        pthread_create(&ecrivains[i], NULL, writer, NULL);
    }

    for (int i = 0; i < N_READERS; i++){
        pthread_create(&lecteurs[i], NULL, reader, NULL);

    }

    for (int i = 0; i < N_WRITERS; i++){
        pthread_join(ecrivains[i], NULL);
    }

    for (int i = 0; i < N_READERS; i++){
        
        pthread_join(lecteurs[i], NULL);
    }
        

        return 0;
    }

void* reader(void* arg) {
    


    while (1) {
        pthread_mutex_lock(&mutex_compteurs);
        if (compteur_lecture >= MAX_LECTURE) {
            pthread_mutex_unlock(&mutex_compteurs);
            break;
        }
        compteur_lecture++; // réserve une lecture avant de libérer le verrou
        pthread_mutex_unlock(&mutex_compteurs);
        pthread_mutex_lock(&z);
        // only one reader waiting on rsem
        sem_wait(&rsem);
        {
            pthread_mutex_lock(&mutex_current_readers);
            current_readers++;
            if (current_readers == 1) {
                sem_wait(&wsem);
            }
            pthread_mutex_unlock(&mutex_current_readers);
        }
        sem_post(&rsem);  // free next reader/writer
        pthread_mutex_unlock(&z);

        // read ressource...
        for (int i=0; i<10000; i++);

        pthread_mutex_lock(&mutex_current_readers);
        current_readers--;
        if (current_readers == 0) {
            sem_post(&wsem);  // last reader
        }
        pthread_mutex_unlock(&mutex_current_readers);
    }
    return NULL;
}

void* writer(void* arg) {



    while (1) {
        pthread_mutex_lock(&mutex_compteurs);
        if (compteur_ecrivain >= MAX_ECRIVAIN) {
            pthread_mutex_unlock(&mutex_compteurs);
            break;
        }
        compteur_ecrivain++; // réserve une écriture avant de libérer le verrou
        pthread_mutex_unlock(&mutex_compteurs);
        pthread_mutex_lock(&mutex_current_writers);
        // critical section - current_writers
        current_writers++;
        if (current_writers == 1) {
            sem_wait(&rsem);
        }
        pthread_mutex_unlock(&mutex_current_writers);
        sem_wait(&wsem);
        // critical section, only one writer at a time
        
        // write ressource...
        for (int i=0; i<10000; i++);
        sem_post(&wsem);
        pthread_mutex_lock(&mutex_current_writers);
        // critical section - current_writers
        current_writers--;
        if (current_writers == 0) {
            sem_post(&rsem);  // last writer
        }
        pthread_mutex_unlock(&mutex_current_writers);
    }
    return NULL;
}
