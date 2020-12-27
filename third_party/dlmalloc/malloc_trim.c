#include "libc/mem/mem.h"
#include "third_party/dlmalloc/dlmalloc.internal.h"

/**
 * If possible, gives memory back to the system (via negative arguments
 * to sbrk) if there is unused memory at the `high` end of the malloc
 * pool or in unused MMAP segments. You can call this after freeing
 * large blocks of memory to potentially reduce the system-level memory
 * requirements of a program. However, it cannot guarantee to reduce
 * memory. Under some allocation patterns, some large free blocks of
 * memory will be locked between two used chunks, so they cannot be
 * given back to the system.
 *
 * The `pad` argument to malloc_trim represents the amount of free
 * trailing space to leave untrimmed. If this argument is zero, only the
 * minimum amount of memory to maintain internal data structures will be
 * left. Non-zero arguments can be supplied to maintain enough trailing
 * space to service future expected allocations without having to
 * re-obtain memory from the system.
 *
 * @return 1 if it actually released any memory, else 0
 */
int dlmalloc_trim(size_t pad) {
  int result = 0;
  ensure_initialization();
  if (!PREACTION(g_dlmalloc)) {
    result = dlmalloc_sys_trim(g_dlmalloc, pad);
    POSTACTION(g_dlmalloc);
  }
  return result;
}
