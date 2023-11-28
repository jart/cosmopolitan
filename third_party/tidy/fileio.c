/* fileio.c -- does standard I/O

  (c) 1998-2007 (W3C) MIT, ERCIM, Keio University
  See tidy.h for the copyright notice.

  Default implementations of Tidy input sources
  and output sinks based on standard C FILE*.

*/


#include "third_party/tidy/forward.h"
#include "third_party/tidy/fileio.h"
#include "third_party/tidy/tidy.h"
#include "third_party/tidy/sprtf.h"

typedef struct _fp_input_source
{
    FILE*        fp;
    TidyBuffer   unget;
} FileSource;

static int filesrc_getByte( void* sourceData )
{
  FileSource* fin = (FileSource*) sourceData;
  int bv;
  if ( fin->unget.size > 0 )
    bv = tidyBufPopByte( &fin->unget );
  else
    bv = fgetc( fin->fp );
  return bv;
}

static Bool filesrc_eof( void* sourceData )
{
  FileSource* fin = (FileSource*) sourceData;
  Bool isEOF = ( fin->unget.size == 0 );
  if ( isEOF )
    isEOF = feof( fin->fp ) != 0;
  return isEOF;
}

static void filesrc_ungetByte( void* sourceData, byte bv )
{
  FileSource* fin = (FileSource*) sourceData;
  tidyBufPutByte( &fin->unget, bv );
}

#if SUPPORT_POSIX_MAPPED_FILES
#  define initFileSource initStdIOFileSource
#  define freeFileSource freeStdIOFileSource
#endif
int TY_(initFileSource)( TidyAllocator *allocator, TidyInputSource* inp, FILE* fp )
{
  FileSource* fin = NULL;

  fin = (FileSource*) TidyAlloc( allocator, sizeof(FileSource) );
  if ( !fin )
      return -1;
  TidyClearMemory( fin, sizeof(FileSource) );
  fin->unget.allocator = allocator;
  fin->fp = fp;

  inp->getByte    = filesrc_getByte;
  inp->eof        = filesrc_eof;
  inp->ungetByte  = filesrc_ungetByte;
  inp->sourceData = fin;

  return 0;
}

void TY_(freeFileSource)( TidyInputSource* inp, Bool closeIt )
{
    FileSource* fin = (FileSource*) inp->sourceData;
    if ( closeIt && fin && fin->fp )
      fclose( fin->fp );
    tidyBufFree( &fin->unget );
    if (fin)
        TidyFree( fin->unget.allocator, fin );
}

void TY_(filesink_putByte)( void* sinkData, byte bv )
{
  FILE* fout = (FILE*) sinkData;
  fputc( bv, fout );
#if defined(ENABLE_DEBUG_LOG)
  if (fileno(fout) != 2)
  {
      if (bv != 0x0d)
      {
          /*\
           * avoid duplicate newline - SPRTF will translate an 0x0d to CRLF,
           *  and do the same with the following 0x0a
          \*/
          SPRTF("%c",bv);
      }
  }
#endif
}

void TY_(initFileSink)( TidyOutputSink* outp, FILE* fp )
{
  outp->putByte  = TY_(filesink_putByte);
  outp->sinkData = fp;
}

/*
 * local variables:
 * mode: c
 * indent-tabs-mode: nil
 * c-basic-offset: 4
 * end:
 */
