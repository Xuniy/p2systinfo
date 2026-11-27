typedef struct{
    int libre;
} tatas_t;

static inline void tatas_init(tatas_t*verrou){
    verrou->libre = 0;
}

static inline int tatas_changement(int*adr, int valeur){
    asm volatile(
        "xchg %0, %1"
        : "+r" (valeur), "+m" (*adr)
        :
        :"memory"

    );
    return valeur;
}



static inline void tatas_lock(tatas_t *verrou)
{
    while (1) {
        while (__atomic_load_n(&verrou->libre,
                               __ATOMIC_RELAXED) != 0) {
        }

        if (tatas_changement(&verrou->libre, 1) == 0) {
            return;
        }
    }
}

static inline void tatas_unlock(tatas_t *verrou)
{
    __atomic_store_n(&verrou->libre, 0, __ATOMIC_RELEASE);
}
