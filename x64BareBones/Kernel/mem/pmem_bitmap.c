#include "pmem.h"

#define ALIGN_UP(x, a)                                                                                                 \
	(((x) + ((a) - 1)) &                                                                                               \
	 ~((a) - 1)) // sirve para redondear a siguiente multiplo de a (4096 para las paginas que definimos)

static uint8_t *pool_base; // direccion donde empieza el pool (mismo que en .ld)
static size_t pool_len;	   // largo del pool en bytes
static size_t page_count;  // cantidad de paginas en el pool

static uint8_t *bitmap;	 // 1 bit por pag (0 libre, 1 usada)
static uint32_t *runlen; // longitud del bloque en la pagina “cabeza”

static size_t free_pages;
static size_t used_pages;

// Devuelve la direccion fisica de pag numero idx
static inline uint8_t *page_addr(size_t idx)
{
	return pool_base + idx * PM_PAGE_SIZE;
}

// Devuelve la cantidad de paginas que se necesitan para guardar bytes
static size_t pages_needed(size_t bytes)
{
	if (bytes == 0)
		return 1;
	size_t k = bytes / PM_PAGE_SIZE;
	if (bytes % PM_PAGE_SIZE != 0)
		k += 1;
	return k;
}

static void mark_page_used(size_t i)
{
	bitmap[i] = 1;
}
static void mark_page_free(size_t i)
{
	bitmap[i] = 0;
}
static int page_is_used(size_t i)
{
	return bitmap[i] != 0;
}

int mem_init(void *pool_start, size_t pool_length)
{
	// inicializa variables globales
	pool_base = (uint8_t *) pool_start;
	pool_len = pool_length;
	page_count = pool_len / PM_PAGE_SIZE; // calcula cuabtas paginas hay

	// tamaños de metadatos
	size_t bm_bytes = page_count * sizeof(uint8_t);		   // tamaño del bitmap en bytes
	size_t bm_bytes_al = ALIGN_UP(bm_bytes, PM_PAGE_SIZE); // tamaño de bitmap alineado a pagina (es decir, si bm_bytes
														   // es 4099 por ejemplo, bm_bytes_al va a ser 4096*2)

	size_t rl_bytes = page_count * sizeof(uint32_t);	   // tamaño de la tabla de longitudes (4 bytes por pag)
	size_t rl_bytes_al = ALIGN_UP(rl_bytes, PM_PAGE_SIZE); // tamaño de la tabla de longitudes alineado a pagina

	// Layout dentro del pool
	bitmap = (uint8_t *) pool_base; // ubico bitmap al inicio del pool
	runlen =
		(uint32_t *) (pool_base + bm_bytes_al); // ubico tabla de longitudes inmediatamente despues de bitmap alineado
	uint8_t *data_base = pool_base + bm_bytes_al + rl_bytes_al; // direccion de primera de las paginas entregables
	// -> ya ubique estas 3 secciones y tengo las direcciones

	// chequear que quede espacio para al menos 1 pagina de data
	size_t meta_bytes = bm_bytes_al + rl_bytes_al; // tamaño total de metadatos alineado a paginas
	size_t meta_pages = meta_bytes / PM_PAGE_SIZE; // cantidad de paginas usadas por metadatos
	if (page_count <= meta_pages)
	{ // no entran los metadatos y al menos 1 pag de data
		return -1;
	}

	// inicializo bitmap y tabla de longitudes
	for (size_t i = 0; i < bm_bytes_al; i++)
	{
		bitmap[i] = 0;
	}

	size_t rl_count = rl_bytes_al / sizeof(uint32_t);
	for (size_t i = 0; i < rl_count; i++)
	{
		runlen[i] = 0;
	}

	// marco paginas que usan los metadatos como usadas
	for (size_t p = 0; p < meta_pages; p++)
	{
		mark_page_used(p);
	}

	// inicializo contadores
	used_pages = meta_pages;
	free_pages = page_count - meta_pages;
	return 0;
}

void *mem_alloc(size_t bytes)
{
	size_t p_needed = pages_needed(bytes);
	if (p_needed > free_pages)
	{
		return NULL;
	}
	size_t streak = 0; // numero de paginas consecutivas libres
	size_t head;	   // primera de la racha

	for (int i = 0; i < page_count; i++)
	{ // empiezo de 0 porque pueden haber huecos o memoria que se libero
		if (!page_is_used(i))
		{
			streak++;
			if (streak == 1)
			{
				head = i;
			}
			if (streak == p_needed)
			{
				for (int p = head; p < head + p_needed; p++)
				{
					mark_page_used(p); // marcar pagina p como ocupada
				}
				runlen[head] = (uint32_t) p_needed; // guardo en la posicion head el tamaño del bloque en paginas
				used_pages += p_needed;				// contadores
				free_pages -= p_needed;

				void *addr = page_addr(head);
				return addr;
			}
		}
		else
		{
			streak = 0;
		}
	}
	return NULL; // no encontre hueco
}

void mem_free(void *p)
{
	size_t offset = (uint8_t *) p - pool_base;
	if (offset % PM_PAGE_SIZE != 0)
	{
		// me dieron un puntero que no apunta al prinicpio de una pagina, VER
	}
	size_t idx = offset / PM_PAGE_SIZE;

	uint32_t block_size = runlen[idx];
	if (block_size == 0)
	{
		// no me dieron el inicio de un bloque, si no una pagina intermedia -> no libero, VER
	}

	for (size_t i = idx; i < idx + block_size; i++)
	{
		mark_page_free(i);
	}

	runlen[idx] = 0;
	used_pages -= block_size;
	free_pages += block_size;
}

void mem_get_stats(pm_stats_t *out)
{
	out->total = (uint64_t) page_count * PM_PAGE_SIZE;
	out->used = (uint64_t) used_pages * PM_PAGE_SIZE;
	out->free = (uint64_t) free_pages * PM_PAGE_SIZE;
}
