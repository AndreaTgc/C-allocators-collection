#ifndef STACK_ALLOC_H
#define STACK_ALLOC_H

/**
 * STB-Style implementation of a stack allocator, it is extremely similar to the
 * arena allocator implementation in 'arena_allocator.h" but with the ability to
 * pop bytes from the top of the stack.
 *
 * 'mem_stack_init' -> Allocates a new stack allocator on the heap and returns
 * the pointer to it. Please note that this implementation considers the number
 * of bytes as a hard cap and it won't allow you to exceed that limit.
 *
 * 'mem_stack_alloc' -> Reserves a certain amount of bytes (function parameters)
 * and returns the pointer to that chunk of memory to the user
 *
 * 'mem_stack_free' -> This function is effectively the pop operation for the
 * stack and it's used to free n bytes from the top, making them available for
 * future allocations
 *
 * 'mem_stack_deinit' -> Frees all the memory associated with the stack (the
 * stack itself was heap allocated by 'mem_stack_init' so it gets freed too)
 */

#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint8_t *data;
  size_t size;
  size_t capacity;
} MemStack;

/**
 * Allocates a new memory stack on the heap and returns the pointer to it
 * @param bytes number of bytes used as the stack size
 * @return pointer to newly allocated memory stack
 */
MemStack *mem_stack_init(size_t bytes);

/**
 * Takes a chunk of memory of the requested size from the stack and returns the
 * pointer to it to the user
 * @param stack pointer to the stack used for the allocation
 * @param bytes number of bytes to be reserved
 * @returns pointer to the reserved bytes
 */
void *mem_stack_alloc(MemStack *stack, size_t bytes);

/**
 * Gives some bytes back to the stack, making them available for future
 * allocations
 * @param stack pointer to the stack used for the allocation
 * @param bytes amount of bytes given back to the stack
 * @return 1 if the call succeded, 0 otherwise.
 */
int mem_stack_pop(MemStack *stack, size_t bytes);

/**
 * Frees all the memory related to the memory stack passed as parameter
 * @param stack memory stack you want to free
 */
void mem_stack_deinit(MemStack *stack);

#endif // STACK_ALLOC_H

#ifdef MEM_STACK_IMPL

MemStack *mem_stack_init(size_t bytes) {
  MemStack *new_stack = malloc(sizeof(MemStack));
  if (new_stack == NULL) {
    return NULL;
  }
  new_stack->data = calloc(bytes, sizeof(uint8_t));
  if (new_stack->data == NULL) {
    free(new_stack);
    return NULL;
  }
  new_stack->size = 0;
  new_stack->capacity = bytes;
  return new_stack;
}

void *mem_stack_alloc(MemStack *stack, size_t bytes) {
  if (stack == NULL) {
    return;
  }
  if (stack->size + bytes > stack->capacity) {
    return NULL;
  }
  void *ptr = stack->data + stack->size;
  stack->size += bytes;
  return ptr;
}

void mem_stack_pop(MemStack *stack, size_t bytes) {
  if (stack == NULL) {
    return;
  }
  if (bytes > stack->size) {
    // Trying to free too many bytes
    return 0;
  }
  stack->size -= bytes;
  return 1;
}

void mem_stack_free(MemStack *stack) {
  if (stack != NULL) {
    free(stack->data);
    free(stack);
    stack = NULL;
  }
}

#endif // MEM_STACK_IMPL
