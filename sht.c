#include "sht.h"
#include "murmur3.h"

#include <assert.h>
#include <string.h>

typedef struct sht {
	uint8_t *table;
	uint32_t item_size;
	uint32_t size;
	uint32_t capacity;
	uint32_t seed;
	void *(*myalloc)(size_t);
	void (*myfree)(void *);
} sht_t;

static uint32_t next_power_of_two(uint32_t v) {
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;
	return v;
}

sht_t *sht_init_alloc(uint32_t item_size, uint32_t reserve, uint32_t seed,
                      void *(*custom_alloc)(size_t),
                      void (*custom_free)(void *)) {
	sht_t *sht = (sht_t *)custom_alloc(sizeof(sht_t));
	assert(sht != NULL);
	sht->myalloc = custom_alloc;
	sht->myfree = custom_free;
	// ensure 4 byte alignment and reserve additional space for hash value
	sht->item_size = item_size + (item_size % 4) + sizeof(uint32_t);
	sht->seed = seed;
	sht->size = 0;
	sht->capacity = next_power_of_two(reserve);
	if (sht->capacity < 2) sht->capacity = 2;
	sht->table = (uint8_t *)custom_alloc(sht->capacity * sht->item_size);
	assert(sht->table != NULL);
	memset(sht->table, 0, sht->capacity * sht->item_size);
	return sht;
}

sht_t *sht_init(uint32_t item_size, uint32_t reserve, uint32_t seed) {
	return sht_init_alloc(item_size, reserve, seed, malloc, free);
}

void sht_destroy(sht_t *sht) {
	assert(sht != NULL);
	if (sht->table != NULL) sht->myfree(sht->table);
	sht->myfree(sht);
}

static uint32_t insert(uint8_t *table, uint32_t k, uint32_t cap, const void *el,
                       uint32_t size) {
	uint32_t id = k & (cap - 1);
	for (;;) {
		uint32_t cmp = *(uint32_t *)&table[id * size];
		if (cmp > 0 && cmp != k) {
			++id;
			id = id % cap;
			continue;
		}
		memcpy(&table[id * size], &k, sizeof(uint32_t));
		memcpy(&table[id * size + sizeof(uint32_t)], el,
		       size - sizeof(uint32_t));
		if (cmp == 0) return 1;
		return 0;
	}
}

static void grow(sht_t *sht) {
	if ((sht->size << 1) < sht->capacity) return;
	sht_t tmp;
	memcpy(&tmp, sht, sizeof(sht_t));
	tmp.capacity = sht->capacity << 1;
	tmp.table = (uint8_t *)sht->myalloc(tmp.capacity * sht->item_size);
	assert(tmp.table != NULL);
	memset(tmp.table, 0, tmp.capacity * tmp.item_size);
	for (uint32_t i = 0; i < sht->capacity; ++i) {
		uint32_t *k = (uint32_t *)&sht->table[i * sht->item_size];
		if (*k == 0) continue;
		insert(tmp.table, *k, tmp.capacity,
		       &sht->table[i * tmp.item_size + sizeof(uint32_t)],
		       tmp.item_size);
	}
	sht->myfree(sht->table);
	memcpy(sht, &tmp, sizeof(sht_t));
}

static uint32_t comp_key(const void *key, int len, uint32_t seed) {
	uint32_t k;
	MurmurHash3_x86_32(key, len, seed, &k);
	if (k == 0) k = 1;
	return k;
}

uint32_t sht_set(sht_t *sht, const void *key, int len, const void *element) {
	assert(sht != NULL);
	grow(sht);
	uint32_t hash = comp_key(key, len, sht->seed);
	if (insert(sht->table, hash, sht->capacity, element, sht->item_size) > 0)
		++(sht->size);
	return hash;
}

void *sht_get(sht_t *sht, const void *key, int len) {
	assert(sht != NULL);
	uint32_t k = comp_key(key, len, sht->seed);
	return sht_get_by_hash(sht, k);
}

void *sht_get_by_hash(sht_t *sht, uint32_t hash) {
	assert(sht != NULL);
	uint32_t id = hash & (sht->capacity - 1);
	for (;;) {
		uint32_t cmp = *(uint32_t *)&sht->table[id * sht->item_size];
		if (cmp == 0) return NULL;
		if (cmp == hash) break;
		++id;
		id = id % sht->capacity;
	}
	return (void *)&sht->table[id * sht->item_size + sizeof(uint32_t)];
}

static void move_left(sht_t *sht, uint32_t start) {
	uint32_t k = *(uint32_t *)&sht->table[start * sht->item_size];
	if (k == 0) return;
	uint32_t id = k & (sht->capacity - 1);
	if (id == start) return;
	uint32_t dest = (start == 0) ? sht->capacity - 1 : start - 1;
	memcpy(&sht->table[dest * sht->item_size],
	       &sht->table[start * sht->item_size], sht->item_size);
	memset(&sht->table[start * sht->item_size], 0, sht->item_size);
	move_left(sht, (start + 1) % sht->capacity);
}

void sht_del(sht_t *sht, const void *key, int len) {
	assert(sht != NULL);
	uint32_t k = comp_key(key, len, sht->seed);
	uint32_t id = k & (sht->capacity - 1);
	for (;;) {
		uint32_t cmp = *(uint32_t *)&sht->table[id * sht->item_size];
		if (cmp == 0) return;
		if (cmp == k) break;
		++id;
		id = id % sht->capacity;
	}
	--(sht->size);
	memset(&sht->table[id * sht->item_size], 0, sht->item_size);

	move_left(sht, (id + 1) % sht->capacity);
}

uint32_t sht_size(sht_t *sht) {
	assert(sht != NULL);
	return sht->size;
}

typedef struct sht_it {
	sht_t *sht;
	uint32_t cur;
	int valid;
} sht_it_t;

sht_it_t *sht_iter(sht_t *sht) {
	assert(sht != NULL);
	sht_it_t *it = (sht_it_t *)sht->myalloc(sizeof(sht_it_t));
	assert(it != NULL);
	it->sht = sht;
	it->cur = 0;
	it->valid = 1;
	return it;
}

void *sht_iter_next(sht_it_t *it) {
	assert(it != NULL && it->valid > 0);
	sht_t *sht = it->sht;
	for (; it->cur < sht->capacity;) {
		uint32_t cmp = *(uint32_t *)&sht->table[it->cur * sht->item_size];
		++(it->cur);
		if (cmp != 0)
			return (void *)&sht
			    ->table[(it->cur - 1) * sht->item_size + sizeof(uint32_t)];
	}
	it->valid = 0;
	return NULL;
}

void sht_iter_destroy(sht_it_t *it) {
	assert(it != NULL);
	it->sht->myfree(it);
}
