#ifndef STACK_ALLOC_H
#define STACK_ALLOC_H

#include <stdint.h>
#include <stdlib.h>

typedef struct {
    uint8_t* data;
    size_t size;
    size_t capacity;
} mem_stack_t;

/**
 * Allocates a new memory stack on the heap and returns the pointer to it
 * @param bytes number of bytes used as the stack size
 * @return pointer to newly allocated memory stack
 */
mem_stack_t* mem_stack_init(size_t bytes);

/**
 * Takes a chunk of memory of the requested size from the stack and returns the
 * pointer to it to the user
 * @param stack pointer to the stack used for the allocation
 * @param bytes number of bytes to be reserved
 * @returns pointer to the reserved bytes
 */
void* mem_stack_alloc(mem_stack_t* stack, size_t bytes);

/**
 * Gives some bytes back to the stack, making them available for future allocations
 * @param stack pointer to the stack used for the allocation
 * @param bytes amount of bytes given back to the stack
 * @return 1 if the call succeded, 0 otherwise.
 */
int mem_stack_pop(mem_stack_t* stack, size_t bytes);
void mem_stack_deinit(mem_stack_t* stack);

#endif // STACK_ALLOC_H

#ifdef MEM_STACK_IMPL

mem_stack_t* mem_stack_init(size_t bytes){
  mem_stack_t* new_stack = malloc(sizeof(mem_stack_t));
  if(new_stack == NULL){
      return NULL;
  }
  new_stack->data = calloc(bytes, sizeof(uint8_t));
  if(new_stack->data == NULL){
      free(new_stack);
      return NULL;
  }
  new_stack->size = 0;
  new_stack->capacity = bytes;
  return new_stack;
}

void* mem_stack_alloc(mem_stack_t* stack, size_t bytes){
    if(stack == NULL){
        return;
    }
    if(stack->size + bytes > stack->capacity){
        return NULL;
    }
    void* ptr = stack->data + stack->size;
    stack->size += bytes;
    return ptr;
}

void mem_stack_pop(mem_stack_t* stack, size_t bytes){
    if(stack == NULL){
        return;
    }
    if(bytes > stack->size){
        // Trying to free too many bytes
        return 0; 
    }
    stack->size -= bytes;
    return 1;
}

void mem_stack_free(mem_stack_t* stack){
    if(stack != NULL){
        free(stack->data);
        free(stack);
        stack = NULL;
    }
}



#endif // MEM_STACK_IMPL
