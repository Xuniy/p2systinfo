
typedef struct {
    int libre;
} tas_t;

static inline void tas_init(tas_t *verrou)
{
    verrou->libre = 0;
}

inline int tas_changement(int* adr, int valeur){

    asm volatile(
        "xchgl %0, %1"
        : "+r" (valeur), "+m" (*adr)
    );


    return valeur;
}

inline void lock(tas_t*verrou){
    while(tas_changement(&verrou->libre, 1) != 0){
        ;
    }


}

inline void unlock(tas_t* verrou){
    int cc = tas_changement(&verrou->libre,0);
    (void) cc;
}