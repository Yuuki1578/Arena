#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef ARENA_IMPL
#define ARENA_IMPL

/*
 * Linear allocator using glibc backend (malloc, realloc).
 * This is a self-referential types or Linked List,
 * packed to 18 bytes
 * */
typedef struct Arena Arena;

struct Arena {
  Arena *next; // reference to the next Arena
  void *data;  // block of memory from realloc()
  bool isNull, isBind;
} __attribute__((packed));

/*
 * Initialize the Arena, creating first queue
 * */
void ArenaInit(void);

/*
 * Create a new Arena.
 * Appending it on the call of ArenaAlloc() or ArenaRealloc()
 * */
Arena ArenaNew(void);

/*
 * Check if the current member of the list have a NULL pointer
 * */
bool ArenaIsNull(void);

/*
 * Allocate size bytes of memory to Arena->data,
 * returning the pointer to it on success, while NULL
 * on error and set errno to non zero
 * */
void *ArenaAlloc(size_t size);

/*
 * Reallocating dst memory into size bytes.
 * While it return the pointer to Arena->data,
 * the pointer from previous call of this such function
 * is still exist in the last member of List before calling
 * ArenaAlloc() or ArenaRealloc()
 * */
void *ArenaRealloc(void *dst, size_t size);

/*
 * Deallocate all the Arena->data pointer,
 * ending the lifetime
 * */
void ArenaFree(void);

/*
 * Global Arena data structure,
 * while BaseAllocator is the first member of the List,
 * Allocator is the pointer to the current member of the List.
 * Most all of this function works with Allocator
 * */
static Arena BaseAllocator, *Allocator;

/*
 * Total allocation from the current process
 * */
static size_t AllocTotal = 0;

void ArenaInit(void) {
  BaseAllocator = ArenaNew();
  Allocator = &BaseAllocator;
}

Arena ArenaNew(void) {
  return (Arena){
      .next = NULL,
      .data = NULL,
      .isNull = true,
      .isBind = false,
  };
}

bool ArenaIsNull(void) {
  if (Allocator == NULL) {
    return true;
  }

  return Allocator->isNull ? true : false;
}

void *ArenaAlloc(size_t size) { return ArenaRealloc(NULL, size); }

void *ArenaRealloc(void *dst, size_t size) {
  AllocTotal += 1;

  if (Allocator->isBind) {
    if (Allocator->next == NULL) {
      Allocator->next = sbrk(sizeof(Arena));
    }

    if (Allocator->next == (void *)-1) {
      exit(errno);
    }

    Allocator = Allocator->next;
    *Allocator = ArenaNew();
  }

  Allocator->isBind = true;
  Allocator->data = realloc(dst, size);

  if (Allocator->data == NULL) {
    Allocator->isNull = true;
    return NULL;
  }

  Allocator->isNull = false;
  return Allocator->data;
}

void ArenaFree(void) {
  Arena *context;

  for (size_t index = 0; index < AllocTotal; index++) {
    if (index == 0) {
      context = &BaseAllocator;
    } else {
      context = context->next;
    }

    if (context == NULL) {
      return;
    }

    if (!context->isNull) {
      free(context->data);
    }

    context->data = NULL;
    context->isNull = true;
    context->isBind = false;
  }

  AllocTotal = 0;
}

#endif
