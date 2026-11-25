#include "tas.h"

typedef struct {
    int valeur;
    tas_t verrou;
} sem_tas_t;


static inline void sem_tas_init(sem_tas_t* sem, int valeur){
    sem->valeur = valeur;
    tas_init(&sem->verrou);
}

static inline void sem_tas_wait(sem_tas_t* sem){
    while (1){
        lock(&sem->verrou);

        if (sem->valeur > 0){
            sem->valeur --;
            unlock(&sem->verrou);
            return;
        }

        unlock(&sem->verrou);


    }
    
}

static inline void sem_tas_post(sem_tas_t *sem){
    lock(&sem->verrou);
    sem->valeur++;
    unlock(&sem->verrou);
}