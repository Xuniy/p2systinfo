typedef struct{
    int libre;
} tatas_t;

static inline void tatas_init(tatas_t*verrou){
    verrou->libre = 0;
}

static inline int tatas_changement(int*adr, int valeur){
    asm volatile(
        "xchgl %0, %1"
        : "+r" (valeur), "+m" (*adr)
        :
        :"memory"

    );
    return valeur;
}



static inline void tatas_lock(tatas_t *verrou){
    while (1)
    {
        /* code */
    }
    
}