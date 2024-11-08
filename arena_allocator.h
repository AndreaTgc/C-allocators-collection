#ifndef ARENA_H
#define ARENA_H

/**
 * @author Andrea Colombo
 * STB-style implementation of a fixed size arena allocator, it currently uses
 * the standard malloc and free functions to handle the allocation and deletion
 * of the arena itself, this may change in the future once i implement my own
 * functions through syscalls.
 *
 * @note This library comes with an extremely simple API, here's a brief
 * introduction to what you'll be able to do with the following code (the code
 * is also commented in case you have any doubts)
 *
 * 'arena_init' -> Allocates a new arena on the heap and returnst the pointer to
 * it. You need to pass the number of bytes as a parameter for the allocation.
 * Please note that, in case the allocation fails, the function will return NULL
 * (the user is heavily encouraged to handle this case in their code)
 *
 * 'arena_alloc' -> Reserves the amount of bytes passed as a parameter and
 * returns the pointer to it. Please note that you won't be able to exceed the
 * number of bytes specified in the 'arena_init' call. Example: the user
 * allocates an arena by calling 'arena_init(10)' and then calls
 * 'arena_alloc(11)'. The 'arena_alloc' function call will return NULL because
 * the user is trying to exceed the number of bytes preiviously specified.
 *
 * 'arena_reset' -> This function is extremely straight forward, it sets
 * arena->size (basically the allocation counter) to 0.
 *
 * 'arena_deinit' -> Frees the arena->data memory and the arena itself since
 * 'arena_init' allocates it on the heap.
 */

#include <stdint.h>
#include <stdlib.h>

/*
 * @param data memory reserved for the arena
 * @param size bytes currently used in the arena (sum of the allocations)
 * @param capacity maximum number of bytes the user can allocate inside the
 * arena
 * @note The user isn't supposed access any of the struct members manually,
 * stick to using the provided functions.
 */
typedef struct {
  uint8_t *data;
  size_t size;
  size_t capacity;
} arena_t;

/**
 * Heap allocates a new arena and returns the pointer to it
 * @param capacity bytes reserved for the arena allocations
 */
arena_t *arena_init(size_t capacity);

/**
 * Public interface for allocating bytes in the arena.
 * @param arena pointer to the arena we want to use for the allocation
 * @param bytes number of bytes we want to allocate inside the arena
 */
void *arena_alloc(arena_t *arena, size_t bytes);

/**
 * Resets the arena state, basically setting it to a new arena allocated
 * with 'arena_init'
 * @param arena pointer to the arena we want to reset
 */
void arena_reset(arena_t *arena);

/**
 * Frees the arena previously allocated with 'arena_init'
 * @param arena pointer to the arena we want to free from the heap.
 */
void arena_deinit(arena_t *arena);

#endif // ARENA_H

#ifdef ARENA_IMPL

arena_t *arena_init(size_t capacity) {
  arena_t *new_arena = malloc(sizeof(arena_t));
  if (new_arena == NULL) {
    return NULL;
  }
  new_arena->size = 0;
  new_arena->capacity = capacity;
  new_arena->data = calloc(capacity, sizeof(uint8_t));
  if (new_arena->data == NULL) {
    free(new_arena);
    return NULL;
  }
  return new_arena;
}

void *arena_alloc(arena_t *arena, size_t bytes) {
  if (arena == NULL) {
    // Something went really wrong here
    return NULL;
  }
  if (bytes > arena->capacity || arena->size > arena->capacity - bytes) {
    return NULL;
  }
  void *ptr = (void *)arena->data + arena->size;
  arena->size += bytes;
  return ptr;
}

void arena_reset(arena_t *arena) {
  if (arena != NULL) {
    arena->size = 0;
  }
}

void arena_deinit(arena_t *arena) {
  if (arena != NULL) {
    free(arena->data);
    free(arena);
    arena = NULL;
  }
}

#endif
