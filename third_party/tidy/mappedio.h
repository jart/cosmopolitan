#ifndef __TIDY_MAPPED_IO_H__
#define __TIDY_MAPPED_IO_H__

/* Interface to mmap style I/O

   (c) 2006 (W3C) MIT, ERCIM, Keio University
   See tidy.h for the copyright notice.

*/

#if defined(_WIN32)
int TY_(DocParseFileWithMappedFile)( TidyDocImpl* doc, ctmbstr filnam );
#endif

#endif /* __TIDY_MAPPED_IO_H__ */
