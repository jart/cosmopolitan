#ifndef __FILEIO_H__
#define __FILEIO_H__

/** @file fileio.h - does standard C I/O

  Implementation of a FILE* based TidyInputSource and
  TidyOutputSink.

  (c) 1998-2007 (W3C) MIT, ERCIM, Keio University
  See tidy.h for the copyright notice.

*/

#include "third_party/tidy/access.h"
#include "third_party/tidy/tidybuffio.h"
#ifdef __cplusplus
extern "C" {
#endif

/** Allocate and initialize file input source */
int TY_(initFileSource)( TidyAllocator *allocator, TidyInputSource* source, FILE* fp );

/** Free file input source */
void TY_(freeFileSource)( TidyInputSource* source, Bool closeIt );

#if SUPPORT_POSIX_MAPPED_FILES
/** Allocate and initialize file input source using Standard C I/O */
int TY_(initStdIOFileSource)( TidyAllocator *allocator, TidyInputSource* source, FILE* fp );

/** Free file input source using Standard C I/O */
void TY_(freeStdIOFileSource)( TidyInputSource* source, Bool closeIt );
#endif

/** Initialize file output sink */
void TY_(initFileSink)( TidyOutputSink* sink, FILE* fp );

/* Needed for internal declarations */
void TY_(filesink_putByte)( void* sinkData, byte bv );

#ifdef __cplusplus
}
#endif
#endif /* __FILEIO_H__ */
