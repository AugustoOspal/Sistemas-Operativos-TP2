// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "pmem.h"

#define BLOCK_SIZE(o) ((size_t) 1 << (o))
#define ALIGN_DOWN(x, a) ((x) & ~((a) - 1))

typedef struct node
{
	struct node *next;
} node_t;

static uintptr_t base_addr; // comienzo de la region de memoria
static size_t total_len;
static int max_order; // orden maximo 2^max_order bytes

static node_t *free_list[BUDDY_MAX_ORDER]; // arreglo de listas. free_list[o] apunta a la lista con bloques de 2^o bytes
static uint8_t *order_map;				   // guarda el orden de cada bloque

static size_t managed_total = 0;

static uintptr_t buddy_of(uintptr_t addr, int order)
{
	return addr ^ BLOCK_SIZE(order);
}

// TODO: Chequear toda esta y ver si podemos retornar -1 en algun caso de error
int mem_init(void *base, size_t length)
{
	base_addr = ALIGN_DOWN((uintptr_t) base, BLOCK_SIZE(BUDDY_MIN_ORDER));
	total_len = length - (base_addr - (uintptr_t) base);
	max_order = BUDDY_MIN_ORDER;

	// itera hasta que ya no entra un bloque de 2^max_order+1 bytes en el total disponible
	while ((1UL << (max_order + 1)) <= total_len &&
		   max_order + 1 < BUDDY_MAX_ORDER) // 1UL << max_order+1 es equivalente a 2^(max_order+1)
		max_order++;

	// inicializo array de listas de bloques (1 para cada orden)
	for (int i = BUDDY_MIN_ORDER; i <= max_order; i++)
		free_list[i] = NULL;

	size_t blocks = total_len >> BUDDY_MIN_ORDER; // equivalente a dividir total_len por 2 BUDDY_MIN_ORDER veces
	order_map = (uint8_t *) base_addr;			  // usa primeras paginas para metadatos
	size_t meta_bytes = blocks;
	base_addr += (meta_bytes + BLOCK_SIZE(BUDDY_MIN_ORDER) - 1) & ~(BLOCK_SIZE(BUDDY_MIN_ORDER) - 1);
	total_len -= (base_addr - (uintptr_t) base);

	managed_total = 0;

	// insertar bloques alineados a su orden maximo
	uintptr_t cur = base_addr;
	uintptr_t end = base_addr + total_len;
	for (int i = max_order; i >= BUDDY_MIN_ORDER; i--)
	{
		size_t bs = BLOCK_SIZE(i);
		while (((cur + bs) <= end) && !(cur & (bs - 1)))
		{
			((node_t *) cur)->next = free_list[i];
			free_list[i] = (node_t *) cur;
			order_map[(cur - base_addr) >> BUDDY_MIN_ORDER] = i;

			managed_total += bs;
			cur += bs;
		}
	}
	return 0;
}

void *mem_alloc(size_t size)
{
	int order = BUDDY_MIN_ORDER;
	while (BLOCK_SIZE(order) < size)
		order++;
	if (order > max_order)
		return NULL;

	int o = order;
	while (o <= max_order && free_list[o] == NULL)
	{
		o++;
	}
	if (o > max_order)
		return NULL;

	while (o > order)
	{ // desde el actual al minimo
		node_t *block = free_list[o];
		free_list[o] = block->next;
		o--;
		uintptr_t addr = (uintptr_t) block;
		uintptr_t half = addr + BLOCK_SIZE(o);
		((node_t *) addr)->next = free_list[o];
		free_list[o] = (node_t *) addr;
		((node_t *) half)->next = free_list[o];
		free_list[o] = (node_t *) half;
	}

	node_t *b = free_list[order];
	free_list[order] = b->next;
	order_map[((uintptr_t) b - base_addr) >> BUDDY_MIN_ORDER] = order;
	return (void *) b;
}

void mem_free(void *ptr)
{
	if (!ptr)
		return;
	uintptr_t addr = (uintptr_t) ptr;
	int order = order_map[(addr - base_addr) >> BUDDY_MIN_ORDER];

	while (order < max_order)
	{
		uintptr_t buddy = buddy_of(addr, order);
		// buscar buddy en lista libre del mismo orden
		node_t **prev = &free_list[order], *n = free_list[order];
		while (n && (uintptr_t) n != buddy)
		{
			prev = &n->next;
			n = n->next;
		}
		if (!n)
			break;		 // buddy ocupado (no mergeo)
		*prev = n->next; // quitar buddy
		addr = (addr < buddy) ? addr : buddy;
		order++;
	}
	((node_t *) addr)->next = free_list[order];
	free_list[order] = (node_t *) addr;
}

void mem_get_stats(pm_stats_t *out)
{
	uint64_t free_bytes = 0;

	for (int i = BUDDY_MIN_ORDER; i <= max_order; i++)
	{
		size_t bs = BLOCK_SIZE(i);
		for (node_t *n = free_list[i]; n; n = n->next)
		{
			free_bytes += bs;
		}
	}

	out->total = (uint64_t) managed_total;
	out->free = free_bytes;
	out->used = out->total - out->free;
}