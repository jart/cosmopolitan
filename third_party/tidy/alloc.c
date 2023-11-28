/* alloc.c -- Default memory allocation routines.

  (c) 1998-2006 (W3C) MIT, ERCIM, Keio University
  See tidy.h for the copyright notice.

*/

/* #define DEBUG_MEMORY very NOISY extra DEBUG of memory allocation, reallocation and free */

#include "third_party/tidy/tidy.h"
#include "third_party/tidy/forward.h"
#include "libc/stdio/stdio.h"
#include "libc/assert.h"
#include "libc/runtime/runtime.h"
#include "libc/mem/mem.h"
#include "third_party/tidy/sprtf.h"

static TidyMalloc  g_malloc  = NULL;
static TidyRealloc g_realloc = NULL;
static TidyFree    g_free    = NULL;
static TidyPanic   g_panic   = NULL;

#if defined(ENABLE_DEBUG_LOG) && defined(DEBUG_MEMORY)
static int alloccnt = 0;
static int realloccnt = 0;
static int freecnt = 0;
#endif


Bool tidySetMallocCall( TidyMalloc fmalloc )
{
  g_malloc  = fmalloc;
  return yes;
}
Bool tidySetReallocCall( TidyRealloc frealloc )
{
  g_realloc = frealloc;
  return yes;
}
Bool tidySetFreeCall( TidyFree ffree )
{
  g_free    = ffree;
  return yes;
}
Bool tidySetPanicCall( TidyPanic fpanic )
{
  g_panic   = fpanic;
  return yes;
}

static void defaultPanic( TidyAllocator* ARG_UNUSED(allocator), ctmbstr msg )
{
  if ( g_panic )
    g_panic( msg );
  else
  {
    /* 2 signifies a serious error */
    fprintf( stderr, "Fatal error: %s\n", msg );
#ifdef _DEBUG
    assert(0);
#endif
    exit(2);
  }
}

static void* defaultAlloc( TidyAllocator* allocator, size_t size )
{
    void *p = ( g_malloc ? g_malloc(size) : malloc(size) );
    if ( !p )
        defaultPanic( allocator,"Out of memory!");
#if defined(ENABLE_DEBUG_LOG) && defined(DEBUG_MEMORY)
    alloccnt++;
    SPRTF("%d: alloc   MEM %p, size %d\n", alloccnt, p, (int)size );
    if (size == 0) {
        SPRTF("NOTE: An allocation of ZERO bytes!!!!!!\n");
    }
#endif
    return p;
}

static void* defaultRealloc( TidyAllocator* allocator, void* mem, size_t newsize )
{
    void *p;
    if ( mem == NULL )
        return defaultAlloc( allocator, newsize );

    p = ( g_realloc ? g_realloc(mem, newsize) : realloc(mem, newsize) );
    if (!p)
        defaultPanic( allocator, "Out of memory!");
#if defined(ENABLE_DEBUG_LOG) && defined(DEBUG_MEMORY)
    realloccnt++;
    SPRTF("%d: realloc MEM %p, size %d\n", realloccnt, p, (int)newsize );
#endif
    return p;
}

static void defaultFree( TidyAllocator* ARG_UNUSED(allocator), void* mem )
{
    if ( mem )
    {
#if defined(ENABLE_DEBUG_LOG) && defined(DEBUG_MEMORY)
        freecnt++;
        SPRTF("%d: free    MEM %p\n", freecnt, mem );
#endif
        if ( g_free )
            g_free( mem );
        else
            free( mem );
    }
}

static const TidyAllocatorVtbl defaultVtbl = {
    defaultAlloc,
    defaultRealloc,
    defaultFree,
    defaultPanic
};

TidyAllocator TY_(g_default_allocator) = {
    &defaultVtbl
};

/*
 * local variables:
 * mode: c
 * indent-tabs-mode: nil
 * c-basic-offset: 4
 * end:
 */
