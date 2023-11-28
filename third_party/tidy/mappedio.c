/* Interface to mmap style I/O

   (c) 2006-2008 (W3C) MIT, ERCIM, Keio University
   See tidy.h for the copyright notice.

   Originally contributed by Cory Nelson and Nuno Lopes

*/

/* keep these here to keep file non-empty */
#include "third_party/tidy/forward.h"
#include "libc/assert.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/stat.h"
#include "libc/sysv/consts/prot.h"
#include "libc/calls/calls.h"
#include "libc/sysv/consts/map.h"
#include "libc/runtime/runtime.h"
#include "third_party/tidy/mappedio.h"

#if SUPPORT_POSIX_MAPPED_FILES

#include "third_party/tidy/fileio.h"




typedef struct
{
    TidyAllocator *allocator;
    const byte *base;
    size_t pos, size;
} MappedFileSource;

static int mapped_getByte( void* sourceData )
{
    MappedFileSource* fin = (MappedFileSource*) sourceData;
    return fin->base[fin->pos++];
}

static Bool mapped_eof( void* sourceData )
{
    MappedFileSource* fin = (MappedFileSource*) sourceData;
    return (fin->pos >= fin->size);
}

static void mapped_ungetByte( void* sourceData, byte ARG_UNUSED(bv) )
{
    MappedFileSource* fin = (MappedFileSource*) sourceData;
    fin->pos--;
}

int TY_(initFileSource)( TidyAllocator *allocator, TidyInputSource* inp, FILE* fp )
{
    MappedFileSource* fin;
    struct stat sbuf;
    int fd;

    fin = (MappedFileSource*) TidyAlloc( allocator, sizeof(MappedFileSource) );
    if ( !fin )
        return -1;

    fd = fileno(fp);
    if ( fstat(fd, &sbuf) == -1
         || sbuf.st_size == 0
         || (fin->base = mmap(0, fin->size = sbuf.st_size, PROT_READ,
                              MAP_SHARED, fd, 0)) == MAP_FAILED)
    {
        TidyFree( allocator, fin );
        /* Fallback on standard I/O */
        return TY_(initStdIOFileSource)( allocator, inp, fp );
    }

    fin->pos = 0;
    fin->allocator = allocator;
    fclose(fp);

    inp->getByte    = mapped_getByte;
    inp->eof        = mapped_eof;
    inp->ungetByte  = mapped_ungetByte;
    inp->sourceData = fin;

    return 0;
}

void TY_(freeFileSource)( TidyInputSource* inp, Bool closeIt )
{
    if ( inp->getByte == mapped_getByte )
    {
        MappedFileSource* fin = (MappedFileSource*) inp->sourceData;
        munmap( (void*)fin->base, fin->size );
        TidyFree( fin->allocator, fin );
    }
    else
        TY_(freeStdIOFileSource)( inp, closeIt );
}

#endif /* SUPPORT_POSIX_MAPPED_FILES */


#if defined(_WIN32)
#  if defined(_MSC_VER) && (_MSC_VER < 1300)  /* less than msvc++ 7.0 */
#    pragma warning(disable:4115) /* named type definition in parentheses in windows headers */
#  endif
#  include "streamio.h"
#  include "tidy-int.h"
#  include "message.h"

typedef struct _fp_input_mapped_source
{
    TidyAllocator *allocator;
    LONGLONG size, pos;
    HANDLE file, map;
    byte *view, *iter, *end;
    unsigned int gran;
} MappedFileSource;

static int mapped_openView( MappedFileSource *data )
{
    DWORD numb = ( ( data->size - data->pos ) > data->gran ) ?
        data->gran : (DWORD)( data->size - data->pos );

    if ( data->view )
    {
        UnmapViewOfFile( data->view );
        data->view = NULL;
    }

    data->view = MapViewOfFile( data->map, FILE_MAP_READ,
                                (DWORD)( data->pos >> 32 ),
                                (DWORD)data->pos, numb );

    if ( !data->view ) return -1;

    data->iter = data->view;
    data->end = data->iter + numb;

    return 0;
}

static int mapped_getByte( void *sourceData )
{
    MappedFileSource *data = sourceData;

    if ( !data->view || data->iter >= data->end )
    {
        data->pos += data->gran;

        if ( data->pos >= data->size || mapped_openView(data) != 0 )
            return EndOfStream;
    }

    return *( data->iter++ );
}

static Bool mapped_eof( void *sourceData )
{
    MappedFileSource *data = sourceData;
    return ( data->pos >= data->size );
}

static void mapped_ungetByte( void *sourceData, byte ARG_UNUSED(bt) )
{
    MappedFileSource *data = sourceData;

    if ( data->iter >= data->view )
    {
        --data->iter;
        return;
    }

    if ( data->pos < data->gran )
    {
        assert(0);
        return;
    }

    data->pos -= data->gran;
    mapped_openView( data );
}

static int initMappedFileSource( TidyAllocator *allocator, TidyInputSource* inp, HANDLE fp )
{
    MappedFileSource* fin = NULL;

    inp->getByte    = mapped_getByte;
    inp->eof        = mapped_eof;
    inp->ungetByte  = mapped_ungetByte;

    fin = (MappedFileSource*) TidyAlloc( allocator, sizeof(MappedFileSource) );
    if ( !fin )
        return -1;

#  if defined(__MINGW32__)
    {
        DWORD lowVal, highVal;
        lowVal = GetFileSize(fp, &highVal);
        if ((lowVal == INVALID_FILE_SIZE) && (GetLastError() != NO_ERROR))
        {
            TidyFree(allocator, fin);
            return -1;
        }
        fin->size = highVal;
        fin->size = (fin->size << 32);
        fin->size += lowVal;
    }
#  else /* NOT a MinGW build */
#    if defined(_MSC_VER) && (_MSC_VER < 1300)  /* less than msvc++ 7.0 */
    {
        LARGE_INTEGER* pli = (LARGE_INTEGER *)&fin->size;
        (DWORD)pli->LowPart = GetFileSize( fp, (DWORD *)&pli->HighPart );
        if ( GetLastError() != NO_ERROR || fin->size <= 0 )
        {
            TidyFree(allocator, fin);
            return -1;
        }
    }
#    else
    if ( !GetFileSizeEx( fp, (LARGE_INTEGER*)&fin->size )
         || fin->size <= 0 )
    {
        TidyFree(allocator, fin);
        return -1;
    }
#    endif
#  endif /* MinGW y/n */

    fin->map = CreateFileMapping( fp, NULL, PAGE_READONLY, 0, 0, NULL );

    if ( !fin->map )
    {
        TidyFree(allocator, fin);
        return -1;
    }

    {
        SYSTEM_INFO info;
        GetSystemInfo( &info );
        fin->gran = info.dwAllocationGranularity;
    }

    fin->allocator = allocator;
    fin->pos    = 0;
    fin->view   = NULL;
    fin->iter   = NULL;
    fin->end    = NULL;

    if ( mapped_openView( fin ) != 0 )
    {
        CloseHandle( fin->map );
        TidyFree( allocator, fin );
        return -1;
    }

    fin->file = fp;
    inp->sourceData = fin;

    return 0;
}

static void freeMappedFileSource( TidyInputSource* inp, Bool closeIt )
{
    MappedFileSource* fin = (MappedFileSource*) inp->sourceData;
    if ( closeIt && fin && fin->file != INVALID_HANDLE_VALUE )
    {
        if ( fin->view )
            UnmapViewOfFile( fin->view );

        CloseHandle( fin->map );
        CloseHandle( fin->file );
    }
    TidyFree( fin->allocator, fin );
}

StreamIn* MappedFileInput ( TidyDocImpl* doc, HANDLE fp, int encoding )
{
    StreamIn *in = TY_(initStreamIn)( doc, encoding );
    if ( initMappedFileSource( doc->allocator, &in->source, fp ) != 0 )
    {
        TY_(freeStreamIn)( in );
        return NULL;
    }
    in->iotype = FileIO;
    return in;
}


int TY_(DocParseFileWithMappedFile)( TidyDocImpl* doc, ctmbstr filnam ) {
    int status = -ENOENT;
    HANDLE fin = CreateFileA( filnam, GENERIC_READ, FILE_SHARE_READ, NULL,
                              OPEN_EXISTING, 0, NULL );

#  if PRESERVE_FILE_TIMES
    LONGLONG actime, modtime;
    TidyClearMemory( &doc->filetimes, sizeof(doc->filetimes) );

    if ( fin != INVALID_HANDLE_VALUE && cfgBool(doc,TidyKeepFileTimes) &&
         GetFileTime(fin, NULL, (FILETIME*)&actime, (FILETIME*)&modtime) )
    {
#    define TY_I64(str) TYDYAPPEND(str,LL)
#    if _MSC_VER < 1300  && !defined(__GNUC__) /* less than msvc++ 7.0 */
#      undef TY_I64
#      define TY_I64(str) TYDYAPPEND(str,i64)
#    endif
        doc->filetimes.actime =
            (time_t)( ( actime  - TY_I64(116444736000000000)) / 10000000 );

        doc->filetimes.modtime =
            (time_t)( ( modtime - TY_I64(116444736000000000)) / 10000000 );
    }
#  endif /* PRESERVE_FILE_TIMES */

    if ( fin != INVALID_HANDLE_VALUE )
    {
        StreamIn* in = MappedFileInput( doc, fin,
                                        cfg( doc, TidyInCharEncoding ) );
        if ( !in )
        {
            CloseHandle( fin );
            return -ENOMEM;
        }

        status = TY_(DocParseStream)( doc, in );
        freeMappedFileSource( &in->source, yes );
        TY_(freeStreamIn)( in );
    }
    else /* Error message! */
        TY_(ReportFileError)( doc, filnam, FILE_CANT_OPEN );
    return status;
}

#endif /* defined(_WIN32) */

