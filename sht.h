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

/**
 * @brief Initialize a new hashtable with custom allocator.
 *
 * @param item_size The size in bytes of an item
 * @param reserve Number of slots to reserve upon initialization (will be
 * rounded up to the next power of two)
 * @param seed Seed used by the hash function
 * @param alloc Pointer to the allocation function
 * @param free Pointer to the function for freeing allocated memory
 * @return sht_t*
 */
sht_t *sht_init_alloc(uint32_t item_size, uint32_t reserve, uint32_t seed,
                      void *(*alloc)(size_t), void (*free)(void *));

/**
 * @brief Initialize a new hashtable using the memory allocator from `stdlib.h`
 * 
 * @param item_size The size in bytes of an item
 * @param reserve Number of slots to reserve upon initialization (will be
 * rounded up to the next power of two)
 * @param seed Seed used by the hash function
 * @return sht_t* 
 */
sht_t *sht_init(uint32_t item_size, uint32_t reserve, uint32_t seed);

void sht_destroy(sht_t *sht);

/**
 * @brief Add or overwrite an item in the hashtable.
 * 
 * @param sht Pointer to the hashtable
 * @param key Pointer to the key to be used
 * @param len The length in bytes of the passed key
 * @param item Pointer to the item 
 */
void sht_set(sht_t *sht, const void *key, int len, const void *item);

/**
 * @brief Get an item if present.
 * 
 * @param sht Pointer to the hashtable
 * @param key Pointer to the key to be used
 * @param len The length in bytes of the passed key
 * @return void* of the requested item or NULL if not found
 */
void *sht_get(sht_t *sht, const void *key, int len);

/**
 * @brief Delete an item if present
 * 
 * @param sht Pointer to the hashtable
 * @param key Pointer to the key to be used
 * @param len The length in bytes of the passed key
 */
void sht_del(sht_t *sht, const void *key, int len);

/**
 * @brief Number of items stored in the hashtable
 * 
 * @param sht Pointer to the hashtable
 * @return uint32_t 
 */
uint32_t sht_size(sht_t *sht);

typedef struct sht_it sht_it_t;

/**
 * @brief Get a new iterator for the hashtable
 * 
 * @param sht Pointer to the hashtable
 * @return sht_it_t* 
 */
sht_it_t *sht_iter(sht_t *sht);

/**
 * @brief Get next item of the iterator. 
 * 
 * @param it Pointer to the iterator
 * @return void* of the next item or `NULL`
 */
void *sht_iter_next(sht_it_t *it);

void sht_iter_destroy(sht_it_t *it);

/**
 * @brief foreach macro to iterate over a hashtable that expects a pointer to the hashtable, a non initialized pointer to an iterator and a void * (or pointer to the item type).
 * 
 */
#define sht_foreach(sht, it, item)                                             \
	for ((it) = sht_iter((sht)), (item) = sht_iter_next((it)); (item) != NULL; \
	     (item) = sht_iter_next((it)))

#ifdef __cplusplus
}
#endif
