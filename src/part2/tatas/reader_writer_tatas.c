#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "sem_tatas.h"


int N_READERS;
int N_WRITERS;
int temp_ecriture = 0;
int temp_lecture = 0;

tatas_t mutex_current_readers;
tatas_t  mutex_current_writers;
tatas_t  mutex_compteurs;

tatas_t  z; // only one waiting reader
sem_tatas_t wsem;        // exclusive access to the ressource (init at 1)
sem_tatas_t rsem;        // to block readers (init at 1)

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

    tatas_init(&mutex_current_readers);
    tatas_init(&mutex_current_writers);
    tatas_init(&mutex_compteurs);
    tatas_init(&z);
   
    
    N_WRITERS =  atoi(argv[1]);
    N_READERS = atoi(argv[2]);
    sem_tatas_init(&wsem, 1);
    sem_tatas_init(&rsem, 1);

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
        tatas_lock(&mutex_compteurs);
        if (compteur_lecture >= MAX_LECTURE) {
            tatas_unlock(&mutex_compteurs);
            break;
        }
        compteur_lecture++; // réserve une lecture avant de libérer le verrou
        tatas_unlock(&mutex_compteurs);
        tatas_lock(&z);
        // only one reader waiting on rsem
        sem_tatas_wait(&rsem);
        {
            tatas_lock(&mutex_current_readers);
            current_readers++;
            if (current_readers == 1) {
                sem_tatas_wait(&wsem);
            }
            tatas_unlock(&mutex_current_readers);
        }
        sem_tatas_post(&rsem);  // free next reader/writer
        tatas_unlock(&z);

        // read ressource...
        for (int i=0; i<10000; i++);

        tatas_lock(&mutex_current_readers);
        current_readers--;
        if (current_readers == 0) {
            sem_tatas_post(&wsem);  // last reader
        }
        tatas_unlock(&mutex_current_readers);
    }
    return NULL;
}

void* writer(void* arg) {



    while (1) {
        tatas_lock(&mutex_compteurs);
        if (compteur_ecrivain >= MAX_ECRIVAIN) {
            tatas_unlock(&mutex_compteurs);
            break;
        }
        compteur_ecrivain++; // réserve une écriture avant de libérer le verrou
        tatas_unlock(&mutex_compteurs);
        tatas_lock(&mutex_current_writers);
        // critical section - current_writers
        current_writers++;
        if (current_writers == 1) {
            sem_tatas_wait(&rsem);
        }
        tatas_unlock(&mutex_current_writers);
        sem_tatas_wait(&wsem);
        // critical section, only one writer at a time
        
        // write ressource...
        for (int i=0; i<10000; i++);
        sem_tatas_post(&wsem);
        tatas_lock(&mutex_current_writers);
        // critical section - current_writers
        current_writers--;
        if (current_writers == 0) {
            sem_tatas_post(&rsem);  // last writer
        }
        tatas_unlock(&mutex_current_writers);
    }
    return NULL;
}
