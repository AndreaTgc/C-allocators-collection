#ifndef POOL_H
#define POOL_H

/**
 * @author Andrea Colombo
 * Simple STB-style implementation of a memory pool allocator.
 * The way it works is extremely straight forward, you request a certain amount
 * of memory during initialization that gets split into chunks of the desired
 * size. After that you're able to request chunks from the pool or give them
 * back when you don't need them anymore. Here's a quick explanation of the
 * functions you'll have to use in order to interact with this library.
 *
 * 'pool_init' -> takes two parameters, chunk_size and n_chunks, it allocates a
 * new memory pool on the heap containing the "raw" memory you'll then be able
 * to use for the initalization. It returns the pointer to the newly allocated
 * pool or NULL if any of the allocations fail.
 *
 * 'pool_alloc' -> finds the first free chunk of memory inside the pool and
 * returns a void pointer that points to that chunk
 *
 * 'pool_free' -> gives back a pointer (chunk) to the memory pool, making it a
 * candidate for future allocations
 *
 * 'pool_deinit' -> frees all the memory related to the pool (the pool itself
 * was heap allocated so it frees it too)
 */

#include <stdint.h>
#include <stdlib.h>

/**
 * @param chunk_size number of bytes occupied by each chunk
 * @param n_chunks number of chunks alloacted at initialization
 * @param data pointer to the "raw" memory allocated for the pool
 * @oaram ledger bitmap to check for free chunks
 * @note The user isn't supposed access any of the struct members manually,
 * stick to using the provided functions.
 */
typedef struct {
  size_t chunk_size;
  size_t n_chunks;
  uint8_t *data;
  uint8_t *ledger;
} pool_t;

/**
 * Heap allocates a new memory pool and returns the pointer to it
 * @param chunk_size number of bytes required for a single chunk
 * @param n_chunks number of chunks our pool must hold
 * @return pointer to the initialized memory pool, or NULL on failure
 */
pool_t *pool_init(size_t chunk_size, size_t n_chunks);

/**
 * Gets the first free chunk in the pool and returns the pointer to it
 * @param pool memory pool we want to get a chunk from
 * @return pointer to a free chunk, or NULL if no free chunk is available
 */
void *pool_alloc(pool_t *pool);

/**
 * Gives a chunk of memory back to the pool, allowing it to be used
 * for future allocations
 * @param pool memory pool we are giving the memory back to.
 * @param chunk pointer to the chunk of memory we are giving back
 */
void pool_free(pool_t *pool, void *chunk);

/**
 * Frees the memory related to the pool passed as parameter
 * @param pool memory pool we are freeing
 */
void pool_deinit(pool_t *pool);

#endif // POOL_H

#ifdef POOL_IMPL

// Macros to manipulate the bitmap ledger
#define SET_BIT(bitmap, index) (bitmap[(index) / 8] |= (1 << ((index) % 8)))
#define CLEAR_BIT(bitmap, index) (bitmap[(index) / 8] &= ~(1 << ((index) % 8)))
#define CHECK_BIT(bitmap, index) (bitmap[(index) / 8] & (1 << ((index) % 8)))

pool_t *pool_init(size_t chunk_size, size_t n_chunks) {
  pool_t *new_pool = malloc(sizeof(pool_t));
  if (new_pool == NULL) {
    return NULL;
  }

  new_pool->data = calloc(n_chunks, chunk_size);
  if (new_pool->data == NULL) {
    free(new_pool);
    return NULL;
  }

  // Calculate ledger size in bytes, rounding up to cover all bits for chunks
  size_t ledger_size = (n_chunks + 7) / 8;
  new_pool->ledger = calloc(ledger_size, sizeof(uint8_t));
  if (new_pool->ledger == NULL) {
    free(new_pool->data);
    free(new_pool);
    return NULL;
  }

  new_pool->chunk_size = chunk_size;
  new_pool->n_chunks = n_chunks;
  return new_pool;
}

void *pool_alloc(pool_t *pool) {
  for (size_t i = 0; i < pool->n_chunks; ++i) {
    if (!CHECK_BIT(pool->ledger, i)) { // Check if chunk i is free
      SET_BIT(pool->ledger, i);        // Mark chunk i as allocated
      return pool->data + (i * pool->chunk_size);
    }
  }
  return NULL; // No free chunks available
}

void pool_free(pool_t *pool, void *chunk) {
  if (chunk == NULL || pool == NULL || pool->data == NULL ||
      pool->ledger == NULL) {
    return;
  }

  // Calculate the index of the chunk being deallocated
  size_t index = ((uint8_t *)chunk - pool->data) / pool->chunk_size;
  if (index < pool->n_chunks) {
    CLEAR_BIT(pool->ledger, index); // Mark chunk as free
  }
}

void pool_deinit(pool_t *pool) {
  if (pool != NULL) {
    free(pool->data);
    free(pool->ledger);
    free(pool);
  }
}

#undef SET_BIT
#undef CLEAR_BIT
#undef CHECK_BIT

#endif // POOL_IMPL
