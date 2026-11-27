#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tas/tas.h"
#include "tatas/tatas.h"



int Max = 32768;

static tas_t verrou;
static tatas_t verrou2;
static int sections_par_thread;

static void *un_thread(void *arg)
{
    (void)arg;
    for (int section = 0; section < sections_par_thread; section++) {
        lock(&verrou);
        /* Travail dans la section critique, compile avec -O0. */
        for (int i = 0; i < 10000; i++);
        unlock(&verrou);
    }
    return NULL;
}
static void *un_thread_tatas(void *arg)
{
    (void)arg;

    for (int section = 0; section < sections_par_thread; section++) {
        tatas_lock(&verrou2);

        for (int i = 0; i < 10000; i++);

        tatas_unlock(&verrou2);
    }

    return NULL;
}
int main(int argc, char *argv[]){
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <threads> <tas|tatas>\n", argv[0]);
        return 1;
    }

    int utilise_tatas;
    if (strcmp(argv[2], "tas") == 0) {
        utilise_tatas = 0;
    } else if (strcmp(argv[2], "tatas") == 0) {
        utilise_tatas = 1;
    } else {
        fprintf(stderr, "Verrou inconnu : %s\n", argv[2]);
        return 1;
    }

    int nombre = atoi(argv[1]);
    
    sections_par_thread = Max / nombre;
    pthread_t *threads = malloc((size_t)nombre * sizeof(*threads));
    if (threads == NULL) {
        return 1;
    }
    if (utilise_tatas) {
        tatas_init(&verrou2);
    } else {
        tas_init(&verrou);
    }

    int crees = 0;
    for (int i = 0; i < nombre; i++) {
        int erreur = pthread_create(&threads[i], NULL,
                                    utilise_tatas ? un_thread_tatas : un_thread,
                                    NULL);
        if (erreur != 0) {
            printf("erreur");
            return 1;
        }
        crees++;
    }
    for (int i = 0; i < crees; i++) {
        int erreur = pthread_join(threads[i], NULL);
        if (erreur != 0) {
            printf("erreur");
            return 1;
        }
    }
    free(threads);
    return 0;
}
