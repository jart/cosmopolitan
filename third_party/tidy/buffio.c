/* buffio.c -- Treat buffer as an I/O stream.

  (c) 1998-2007 (W3C) MIT, ERCIM, Keio University
  See tidy.h for the copyright notice.

  Requires buffer to automatically grow as bytes are added.
  Must keep track of current read and write points.

*/

#include "third_party/tidy/tidy.h"
#include "third_party/tidy/tidybuffio.h"
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/assert.h"
#include "third_party/tidy/forward.h"

/**************
   TIDY
**************/

static int insrc_getByte( void* appData )
{
  TidyBuffer* buf = (TidyBuffer*) appData;
  return tidyBufGetByte( buf );
}
static Bool insrc_eof( void* appData )
{
  TidyBuffer* buf = (TidyBuffer*) appData;
  return tidyBufEndOfInput( buf );
}
static void insrc_ungetByte( void* appData, byte bv )
{
  TidyBuffer* buf = (TidyBuffer*) appData;
  tidyBufUngetByte( buf, bv );
}

void tidyInitInputBuffer( TidyInputSource* inp, TidyBuffer* buf )
{
  inp->getByte    = insrc_getByte;
  inp->eof        = insrc_eof;
  inp->ungetByte  = insrc_ungetByte;
  inp->sourceData = buf;
}

static void outsink_putByte( void* appData, byte bv )
{
  TidyBuffer* buf = (TidyBuffer*) appData;
  tidyBufPutByte( buf, bv );
}

void tidyInitOutputBuffer( TidyOutputSink* outp, TidyBuffer* buf )
{
  outp->putByte  = outsink_putByte;
  outp->sinkData = buf;
}


void tidyBufInit( TidyBuffer* buf )
{
    assert( buf != NULL );
    tidyBufInitWithAllocator( buf, NULL );
}

void tidyBufAlloc( TidyBuffer* buf, uint allocSize )
{
    tidyBufAllocWithAllocator( buf, NULL, allocSize );
}

void tidyBufInitWithAllocator( TidyBuffer* buf,
                                         TidyAllocator *allocator )
{
    assert( buf != NULL );
    TidyClearMemory( buf, sizeof(TidyBuffer) );
    buf->allocator = allocator ? allocator : &TY_(g_default_allocator);
}

void tidyBufAllocWithAllocator( TidyBuffer* buf,
                                          TidyAllocator *allocator,
                                          uint allocSize )
{
    tidyBufInitWithAllocator( buf, allocator );
    tidyBufCheckAlloc( buf, allocSize, 0 );
    buf->next = 0;
}

void tidyBufFree( TidyBuffer* buf )
{
    assert( buf != NULL );
    TidyFree(  buf->allocator, buf->bp );
    tidyBufInitWithAllocator( buf, buf->allocator );
}

void tidyBufClear( TidyBuffer* buf )
{
    assert( buf != NULL );
    if ( buf->bp )
    {
        TidyClearMemory( buf->bp, buf->allocated );
        buf->size = 0;
    }
    buf->next = 0;
}

/* Many users do not call tidyBufInit() or tidyBufAlloc() or their allocator
   counterparts. So by default, set the default allocator.
*/
static void setDefaultAllocator( TidyBuffer* buf )
{
    buf->allocator = &TY_(g_default_allocator);
}

/* Avoid thrashing memory by doubling buffer size
** until larger than requested size.
   buf->allocated is bigger than allocSize+1 so that a trailing null byte is
   always available.
*/
void tidyBufCheckAlloc( TidyBuffer* buf, uint allocSize, uint chunkSize )
{
    assert( buf != NULL );

    if ( !buf->allocator )
        setDefaultAllocator( buf );

    if ( 0 == chunkSize )
        chunkSize = 256;
    if ( allocSize+1 > buf->allocated )
    {
        byte* bp;
        uint allocAmt = chunkSize;
        if ( buf->allocated > 0 )
            allocAmt = buf->allocated;
        while ( allocAmt < allocSize+1 )
            allocAmt *= 2;

        bp = (byte*)TidyRealloc( buf->allocator, buf->bp, allocAmt );
        if ( bp != NULL )
        {
            TidyClearMemory( bp + buf->allocated, allocAmt - buf->allocated );
            buf->bp = bp;
            buf->allocated = allocAmt;
        }
    }
}

/* Attach buffer to a chunk O' memory w/out allocation */
void  tidyBufAttach( TidyBuffer* buf, byte* bp, uint size )
{
    assert( buf != NULL );
    buf->bp = bp;
    buf->size = buf->allocated = size;
    buf->next = 0;
    if ( !buf->allocator )
        setDefaultAllocator( buf );
}

/* Clear pointer to memory w/out deallocation */
void tidyBufDetach( TidyBuffer* buf )
{
    tidyBufInitWithAllocator( buf, buf->allocator );
}


/**************
   OUTPUT
**************/

void tidyBufAppend( TidyBuffer* buf, void* vp, uint size )
{
    assert( buf != NULL );
    if ( vp != NULL && size > 0 )
    {
        tidyBufCheckAlloc( buf, buf->size + size, 0 );
        memcpy( buf->bp + buf->size, vp, size );
        buf->size += size;
    }
}

void tidyBufPutByte( TidyBuffer* buf, byte bv )
{
    assert( buf != NULL );
    tidyBufCheckAlloc( buf, buf->size + 1, 0 );
    buf->bp[ buf->size++ ] = bv;
}


int tidyBufPopByte( TidyBuffer* buf )
{
    int bv = EOF;
    assert( buf != NULL );
    if ( buf->size > 0 )
      bv = buf->bp[ --buf->size ];
    return bv;
}

/**************
   INPUT
**************/

int tidyBufGetByte( TidyBuffer* buf )
{
    int bv = EOF;
    if ( ! tidyBufEndOfInput(buf) )
      bv = buf->bp[ buf->next++ ];
    return bv;
}

Bool tidyBufEndOfInput( TidyBuffer* buf )
{
    return ( buf->next >= buf->size );
}

void tidyBufUngetByte( TidyBuffer* buf, byte bv )
{
    if ( buf->next > 0 )
    {
        --buf->next;
        assert( bv == buf->bp[ buf->next ] );
    }
}

/*
 * local variables:
 * mode: c
 * indent-tabs-mode: nil
 * c-basic-offset: 4
 * end:
 */
