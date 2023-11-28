#ifndef __TIDY_BUFFIO_H__
#define __TIDY_BUFFIO_H__

/**************************************************************************//**
 * @file
 * Treat buffer as a stream that Tidy can use for I/O operations. It offers
 * the ability for the buffer to grow as bytes are added, and keeps track
 * of current read and write points.
 *
 * @author
 *     HTACG, et al (consult git log)
 *
 * @copyright
 *     Copyright (c) 1998-2017 World Wide Web Consortium (Massachusetts
 *     Institute of Technology, European Research Consortium for Informatics
 *     and Mathematics, Keio University).
 * @copyright
 *     See tidy.h for license.
 *
 * @date
 *     Consult git log.
 ******************************************************************************/

#include "third_party/tidy/tidyplatform.h"
#include "third_party/tidy/tidy.h"

#ifdef __cplusplus
extern "C" {
#endif

/** A TidyBuffer is chunk of memory that can be used for multiple I/O purposes
 ** within Tidy.
 ** @ingroup IO
 */
TIDY_STRUCT
struct _TidyBuffer
{
    TidyAllocator* allocator;  /**< Memory allocator */
    byte* bp;                  /**< Pointer to bytes */
    uint  size;                /**< Number of bytes currently in use */
    uint  allocated;           /**< Number of bytes allocated */
    uint  next;                /**< Offset of current input position */
};

/** Initialize data structure using the default allocator */
void tidyBufInit( TidyBuffer* buf );

/** Initialize data structure using the given custom allocator */
void tidyBufInitWithAllocator( TidyBuffer* buf, TidyAllocator* allocator );

/** Free current buffer, allocate given amount, reset input pointer,
    use the default allocator */
void tidyBufAlloc( TidyBuffer* buf, uint allocSize );

/** Free current buffer, allocate given amount, reset input pointer,
    use the given custom allocator */
void tidyBufAllocWithAllocator( TidyBuffer* buf,
                                                      TidyAllocator* allocator,
                                                      uint allocSize );

/** Expand buffer to given size.
**  Chunk size is minimum growth. Pass 0 for default of 256 bytes.
*/
void tidyBufCheckAlloc( TidyBuffer* buf,
                                              uint allocSize, uint chunkSize );

/** Free current contents and zero out */
void tidyBufFree( TidyBuffer* buf );

/** Set buffer bytes to 0 */
void tidyBufClear( TidyBuffer* buf );

/** Attach to existing buffer */
void tidyBufAttach( TidyBuffer* buf, byte* bp, uint size );

/** Detach from buffer.  Caller must free. */
void tidyBufDetach( TidyBuffer* buf );


/** Append bytes to buffer.  Expand if necessary. */
void tidyBufAppend( TidyBuffer* buf, void* vp, uint size );

/** Append one byte to buffer.  Expand if necessary. */
void tidyBufPutByte( TidyBuffer* buf, byte bv );

/** Get byte from end of buffer */
int  tidyBufPopByte( TidyBuffer* buf );


/** Get byte from front of buffer.  Increment input offset. */
int  tidyBufGetByte( TidyBuffer* buf );

/** At end of buffer? */
Bool tidyBufEndOfInput( TidyBuffer* buf );

/** Put a byte back into the buffer.  Decrement input offset. */
void tidyBufUngetByte( TidyBuffer* buf, byte bv );


/**************
   TIDY
**************/

/* Forward declarations
*/

/** Initialize a buffer input source */
void tidyInitInputBuffer( TidyInputSource* inp, TidyBuffer* buf );

/** Initialize a buffer output sink */
void tidyInitOutputBuffer( TidyOutputSink* outp, TidyBuffer* buf );

#ifdef __cplusplus
}
#endif
#endif /* __TIDY_BUFFIO_H__ */

/*
 * local variables:
 * mode: c
 * indent-tabs-mode: nil
 * c-basic-offset: 4
 * end:
 */
