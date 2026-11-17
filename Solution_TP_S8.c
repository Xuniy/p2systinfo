//On met la même logique du reader dans le writer

//On bloque les sémaphores des writers
//On écrit dans le fichier
//On libère les sémaphores des writers

//Quand il a finit son boulot, on décrémente le compteur de writers actifs et on dit que les readers peuvent de nouveau lire le fichier.

//Pour reader, 
//Encapsulation avec sem_wait() pour qu'un seul reader lise à la fois
//On lit le fichier
//On décrémente le compteur de readers actifs


void* reader(void* arg){
    while(true){
        phtread_mutex_lock(&mutex_current_readers);
        sem_wait(&sem_readers);
    }
    sem_wait(&sem_readers);
    //critical section
    pthread_mutex_lock(&mutex_current_readers);
    current_readers++;  
    if (current_readers == 1) {
        sem_wait(&sem_writers);
    }
    pthread_mutex_unlock(&mutex_current_readers);

    //read from the file
    atomic_fetch_add_explicit(&read_count, 1, memory_order_relaxed);
    print_status();

    pthread_mutex_lock(&mutex_current_readers);
    current_readers--;
    if (current_readers == 0) {
        sem_post(&sem_writers);
    }
    pthread_mutex_unlock(&mutex_current_readers);
    sem_post(&sem_readers);
}



void* writer(void* arg){
    pthread_mutex_lock(&mutex_current_writers);
    //critical section
    current_writers++;
    if (current_writers == 1) {
        sem_wait(&sem_readers);
    }
    pthread_mutex_unlock(&mutex_current_writers);
    //One writer can write at a time
    sem_wait(&sem_writers);

    //write to the file
    atomic_fetch_add_explicit(&write_count, 1, memory_order_relaxed);
    print_status();

    sem_post(&sem_writers);
    pthread_mutex_lock(&mutex_current_writers);
    current_writers--;
    if (current_writers == 0) {
        sem_post(&sem_readers);
    }
    pthread_mutex_unlock(&mutex_current_writers);
}