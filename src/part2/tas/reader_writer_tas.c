#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "sem_tas.h"


int N_READERS;
int N_WRITERS;
int temp_ecriture = 0;
int temp_lecture = 0;

tas_t mutex_current_readers;
tas_t  mutex_current_writers;
tas_t  mutex_compteurs;

tas_t  z; // only one waiting reader
sem_tas_t wsem;        // exclusive access to the ressource (init at 1)
sem_tas_t rsem;        // to block readers (init at 1)

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

    tas_init(&mutex_current_readers);
    tas_init(&mutex_current_writers);
    tas_init(&mutex_compteurs);
    tas_init(&z);
   
    
    N_WRITERS =  atoi(argv[1]);
    N_READERS = atoi(argv[2]);
    sem_tas_init(&wsem, 1);
    sem_tas_init(&rsem, 1);

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
        lock(&mutex_compteurs);
        if (compteur_lecture >= MAX_LECTURE) {
            unlock(&mutex_compteurs);
            break;
        }
        compteur_lecture++; // réserve une lecture avant de libérer le verrou
        unlock(&mutex_compteurs);
        lock(&z);
        // only one reader waiting on rsem
        sem_tas_wait(&rsem);
        {
            lock(&mutex_current_readers);
            current_readers++;
            if (current_readers == 1) {
                sem_tas_wait(&wsem);
            }
            unlock(&mutex_current_readers);
        }
        sem_tas_post(&rsem);  // free next reader/writer
        unlock(&z);

        // read ressource...
        for (int i=0; i<10000; i++);

        lock(&mutex_current_readers);
        current_readers--;
        if (current_readers == 0) {
            sem_tas_post(&wsem);  // last reader
        }
        unlock(&mutex_current_readers);
    }
    return NULL;
}

void* writer(void* arg) {



    while (1) {
        lock(&mutex_compteurs);
        if (compteur_ecrivain >= MAX_ECRIVAIN) {
            unlock(&mutex_compteurs);
            break;
        }
        compteur_ecrivain++; // réserve une écriture avant de libérer le verrou
        unlock(&mutex_compteurs);
        lock(&mutex_current_writers);
        // critical section - current_writers
        current_writers++;
        if (current_writers == 1) {
            sem_tas_wait(&rsem);
        }
        unlock(&mutex_current_writers);
        sem_tas_wait(&wsem);
        // critical section, only one writer at a time
        
        // write ressource...
        for (int i=0; i<10000; i++);
        sem_tas_post(&wsem);
        lock(&mutex_current_writers);
        // critical section - current_writers
        current_writers--;
        if (current_writers == 0) {
            sem_tas_post(&rsem);  // last writer
        }
        unlock(&mutex_current_writers);
    }
    return NULL;
}
