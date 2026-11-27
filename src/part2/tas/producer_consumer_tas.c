#include <pthread.h>
#include "sem_tas.h"
#include <stdio.h>
#include <stdlib.h>

int buffer[8];
tas_t mutex_buffer;

sem_tas_t places_libres;
sem_tas_t places_occupees;

int N_CONSO;
int N_PRODU;

void *producteur(void *arg);
void *consommateur(void *arg);

int wejsc;
int wyjsc;

int nombre_production = 0;
int MAX_production = 131072;
int theori_conso = 0;

int main(int argc, char *argv[]) {
    if (argc != 3) {
    printf("Mauvais arguments");
    return 1;
}
    // argc : nombre d’arguments, nom du programme compris
    // argv : tableau des arguments sous forme de chaînes de caractères
    char* tp1;
    char* tp2;
    long temp1 = strtol(argv[1],&tp1, 10);
    long temp2 = strtol(argv[2], &tp2,10);
    N_CONSO =  (int) temp1;
    N_PRODU = (int) temp2;

    tas_init(&mutex_buffer);
    sem_tas_init(&places_libres, 8); // 8 places disponibles
    sem_tas_init(&places_occupees, 0);
          // Aucun élément à consommer
    pthread_t tproducteur[N_PRODU];
    pthread_t tconsommateur[N_CONSO];
    int les_ids[N_PRODU];


    for (int i = 0; i < N_CONSO; i++){
        pthread_create(&tconsommateur[i], NULL, consommateur, NULL);
    }

    for (int i = 0; i < N_PRODU; i++){
        les_ids[i] = 67 + i;
        pthread_create(&tproducteur[i], NULL, producteur, &les_ids[i]);

    }

    for (int i = 0; i < N_CONSO; i++){
        pthread_join(tconsommateur[i], NULL);
    }

    for (int i = 0; i < N_PRODU; i++){
        
        pthread_join(tproducteur[i], NULL);
    }
        

        return 0;
    }


    void *producteur(void *arg){

        int id = *(int*)arg;
        while (1)
        {
        for (int i = 0; i < 10000; i++);

        sem_tas_wait(&places_libres);
        lock(&mutex_buffer);
        if (nombre_production >= MAX_production){
            unlock(&mutex_buffer);
            sem_tas_post(&places_libres);
            break;
        }

        buffer[wejsc]= id;
        wejsc ++;
        if (wejsc > 7) {wejsc = 0;}
        nombre_production ++;

        unlock(&mutex_buffer);
        sem_tas_post(&places_occupees);
        }
        

        return NULL;
    }


    void *consommateur(void *arg){
        while (1)
        {
            lock(&mutex_buffer);
            if (theori_conso >= MAX_production){
                unlock(&mutex_buffer);
                break;
            }

            theori_conso++;
            unlock(&mutex_buffer);

            sem_tas_wait(&places_occupees);

            lock(&mutex_buffer);

            int valeur = buffer[wyjsc];
            wyjsc++; 
            if (wyjsc > 7){wyjsc = 0;}


            unlock(&mutex_buffer);

            sem_tas_post(&places_libres);
            for (int i = 0; i < 10000; i++);


        }
        


        return NULL;
    }
