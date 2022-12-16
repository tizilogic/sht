//-----------------------------------------------------------------------------
// simplehashtable was written by Tiziano Bettio, and is placed in the
// public domain. The author hereby disclaims copyright to this source
// code.

#pragma once

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sht sht_t;

sht_t *sht_init_alloc(uint32_t item_size, uint32_t reserve, uint32_t seed,
                      void *(*alloc)(size_t), void (*free)(void *));
sht_t *sht_init(uint32_t item_size, uint32_t reserve, uint32_t seed);
void sht_destroy(sht_t *sht);
void sht_set(sht_t *sht, const void *key, int len, void *element);
void *sht_get(sht_t *sht, const void *key, int len);
void sht_del(sht_t *sht, const void *key, int len);
uint32_t sht_size(sht_t *sht);

typedef struct sht_it sht_it_t;
sht_it_t *sht_iter(sht_t *sht);
void *sht_iter_next(sht_it_t *it);

#define sht_foreach(sht, it, item)                                             \
	for ((it) = sht_iter((sht)), (item) = sht_iter_next((it)); (item) != NULL; \
	     (item) = sht_iter_next((it)))

#ifdef __cplusplus
}
#endif
