#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include <assert.h>
#include <cosmo.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// lockless push / pop tutorial
//
// this file demonstrates how to create a singly linked list that can be
// pushed and popped across multiple threads, using only atomics. atomic
// operations (rather than using a mutex) make push/pop go faster and it
// ensures asynchronous signal safety too. therefore it will be safe for
// use in a variety of contexts, such as signal handlers.

#define THREADS    128
#define ITERATIONS 10000

// adjust mask based on alignment of list struct
//
// - 0x00fffffffffffff0 may be used if List* is always 16-byte aligned.
//   We know that's the case here, because we call malloc() to create
//   every List* object, and malloc() results are always max aligned.
//
// - 0x00fffffffffffff8 may be used if List* is always 8-byte aligned.
//   This might be the case if you're pushing and popping stuff that was
//   allocated from an array, to avoid malloc() calls. This has one
//   fewer byte of safeguards against the ABA problem though.
//
// - 0x00fffffffffff000 may be used if List* is always page aligned.
//   This is a good choice if you use mmap() to allocate each List*
//   element, since it offers maximum protection against ABA.
//
// - only the highest byte of a 64-bit pointer is safe to use on our
//   supported platforms. on most x86 and arm systems, it's possible to
//   use the top sixteen bits. however that's not the case on more
//   recent high end x86-64 systems that have pml5t.
//
#define MASQUE 0x00fffffffffffff0

#define PTR(x)    ((uintptr_t)(x) & MASQUE)
#define TAG(x)    ROL((uintptr_t)(x) & ~MASQUE, 8)
#define ABA(p, t) ((uintptr_t)(p) | (ROR((uintptr_t)(t), 8) & ~MASQUE))
#define ROL(x, n) (((x) << (n)) | ((x) >> (64 - (n))))
#define ROR(x, n) (((x) >> (n)) | ((x) << (64 - (n))))

struct List {
  struct List* next;
  int count;
};

atomic_uintptr_t list;

void push(struct List* elem) {
  uintptr_t tip;
  assert(!TAG(elem));
  for (tip = atomic_load_explicit(&list, memory_order_relaxed);;) {
    elem->next = (struct List*)PTR(tip);
    if (atomic_compare_exchange_weak_explicit(
            &list, &tip, ABA(elem, TAG(tip) + 1), memory_order_release,
            memory_order_relaxed))
      break;
    pthread_pause_np();
  }
}

struct List* pop(void) {
  uintptr_t tip;
  struct List* elem;
  tip = atomic_load_explicit(&list, memory_order_relaxed);
  while ((elem = (struct List*)PTR(tip))) {
    if (atomic_compare_exchange_weak_explicit(
            &list, &tip, ABA(elem->next, TAG(tip) + 1), memory_order_acquire,
            memory_order_relaxed))
      break;
    pthread_pause_np();
  }
  return elem;
}

void* tester(void* arg) {
  struct List* elem;
  for (int i = 0; i < ITERATIONS; ++i) {
    while (!(elem = pop())) {
      elem = malloc(sizeof(*elem));
      elem->count = 0;
      push(elem);
    }
    elem->count++;
    push(elem);
  }
  return 0;
}

int main() {
  printf("testing aba problem...");
  fflush(stdout);
  pthread_t th[THREADS];
  for (int i = 0; i < THREADS; ++i)
    pthread_create(&th[i], 0, tester, 0);
  for (int i = 0; i < THREADS; ++i)
    pthread_join(th[i], 0);
  int sum = 0;
  struct List* elem;
  while ((elem = pop())) {
    printf(" %d", elem->count);
    sum += elem->count;
    free(elem);
  }
  printf("\n");
  assert(sum == ITERATIONS * THREADS);
  printf("you are the dancing queen\n");
  CheckForMemoryLeaks();
}
