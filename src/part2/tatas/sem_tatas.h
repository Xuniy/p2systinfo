#include "tatas.h"

typedef struct {
    int valeur;
    tatas_t verrou;
} sem_tatas_t;

static inline void sem_tatas_init(sem_tatas_t *sem, int valeur)
{
    sem->valeur = valeur;
    tatas_init(&sem->verrou);
}

static inline void sem_tatas_wait(sem_tatas_t *sem)
{
    while (1) {
        tatas_lock(&sem->verrou);

        if (sem->valeur > 0) {
            sem->valeur--;
            tatas_unlock(&sem->verrou);
            return;
        }

        tatas_unlock(&sem->verrou);
    }
}

static inline void sem_tatas_post(sem_tatas_t *sem)
{
    tatas_lock(&sem->verrou);
    sem->valeur++;
    tatas_unlock(&sem->verrou);
}
