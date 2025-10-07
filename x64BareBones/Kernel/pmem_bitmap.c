#include <pmem.h>

#define ALIGN_UP(x,a) (((x) + ((a)-1)) & ~((a)-1)) //sirve para redondear a siguiente multiplo de a (4096 para las paginas que definimos)

static uint8_t *pool_base; //direccion donde empieza el pool (mismo que en .ld)
static size_t pool_len; //largo del pool en bytes
static size_t  page_count; //cantidad de paginas en el pool

static uint8_t *bitmap; //1 bit por pag (0 libre, 1 usada)
static uint32_t *runlen; // longitud del bloque en la pagina “cabeza”

static size_t free_pages;
static size_t used_pages;

// Marca la pagina i como ocupada (pone el bit en 1)
static inline void bm_set(size_t i) {
    size_t byte_index = i / 8;       // qué byte dentro del bitmap
    size_t bit_index  = i % 8;       // qué bit dentro de ese byte

    uint8_t mask = (uint8_t)(1 << bit_index);
    bitmap[byte_index] = bitmap[byte_index] | mask;
}

// Marca la pagina i como libre (pone el bit en 0)
static inline void bm_clear(size_t i) {
    size_t byte_index = i / 8;
    size_t bit_index  = i % 8;

    uint8_t mask = (uint8_t)(1 << bit_index);
    bitmap[byte_index] = bitmap[byte_index] & (~mask);
}

// Devuelve 1 si la pagina i esta ocupada o 0 si esta libre
static inline int bm_test(size_t i) {
    size_t byte_index = i / 8;
    size_t bit_index  = i % 8;

    uint8_t mask = (uint8_t)(1 << bit_index);
    return (bitmap[byte_index] & mask) != 0;
}


void pm_init(void *pool_start, size_t pool_length) {
    //inicializa variables globales
    pool_base = (uint8_t*)pool_start;
    pool_len = pool_length;
    page_count = pool_len / PM_PAGE_SIZE; //calcula cuabtas paginas hay

    //tamaños de metadatos
    size_t bm_bytes = (page_count + 7) / 8; //tamaño del bitmap en bytes (uno por pag)
    size_t bm_bytes_al = ALIGN_UP(bm_bytes, PM_PAGE_SIZE); //tamaño de bitmap alineado a pagina (es decir, si bm_bytes es 4099 por ejemplo, bm_bytes_al va a ser 4096*2)

    size_t rl_bytes = page_count * sizeof(uint32_t); //tamaño de la tabla de longitudes (4 bytes por pag)
    size_t rl_bytes_al = ALIGN_UP(rl_bytes, PM_PAGE_SIZE); //tamaño de la tabla de longitudes alineado a pagina

    // Layout dentro del pool
    bitmap  = pool_base; //ubico bitmap al inicio del pool
    runlen  = (uint32_t*)(pool_base + bm_bytes_al); //ubico tabla de longitudes inmediatamente despues de bitmap alineado
    uint8_t *data_base = pool_base + bm_bytes_al + rl_bytes_al; //direccion de primera de las paginas entregables
    // -> ya ubique estas 3 secciones y tengo las direcciones

    // Chequeo mínimo: debe quedar al menos 1 página para data
    size_t meta_bytes = bm_bytes_al + rl_bytes_al; //tamaño total de metadatos alineado a paginas
    size_t meta_pages = meta_bytes / PM_PAGE_SIZE; //cantidad de paginas usadas por metadatos
    if (page_count <= meta_pages) { //no entran los metadatos y al menos 1 pag de data
        return -1;
    }

    // inicializo bitmap y tabla de longitudes
    for (size_t i = 0; i < bm_bytes_al; i++){
        bitmap[i] = 0;
    }
    size_t rl_count = rl_bytes_al / sizeof(uint32_t);
    for (size_t i = 0; i < rl_count; i++){
        runlen[i] = 0;
    }

    //marco paginas que usan los metadatos como usadas
    for (size_t p = 0; p < meta_pages; p++){
        bm_set(p);
    }

    //inicializo contadores
    used_pages = meta_pages;
    free_pages = (page_count > meta_pages) ? (page_count - meta_pages) : 0;
}

//falta implementar pm_alloc y pm_free que las hacemos despues

void pm_get_stats(pm_stats_t *out) {
    out->total = (uint64_t)page_count * PM_PAGE_SIZE;
    out->used  = (uint64_t)used_pages * PM_PAGE_SIZE;
    out->free  = (uint64_t)free_pages * PM_PAGE_SIZE;
}
