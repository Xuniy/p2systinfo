
typedef struct {
    int libre;
} tas_t;

static inline void tas_init(tas_t *verrou)
{
    verrou->libre = 0;
}

static inline int tas_changement(int* adr, int valeur){

    asm volatile(
        "xchg %0, %1"
        : "+r" (valeur), "+m" (*adr)
        :
        : "memory"
    );


    return valeur;
}

static inline void lock(tas_t*verrou){
    while(tas_changement(&verrou->libre, 1) != 0){
        ;
    }


}

static inline void unlock(tas_t* verrou){
    int cc = tas_changement(&verrou->libre,0);
    (void) cc;
}