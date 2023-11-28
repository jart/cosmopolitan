/*
  ziperr.h - Zip 3

  Copyright (c) 1990-2007 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2007-Mar-4 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/*
 *  ziperr.h by Mark Adler
 */


/*
 * VMS message file ident string.  (The "-xxx" suffix should be
 * incremented when messages are changed for a particular program
 * version.)  Used only when generating the VMS message source file, but
 * that can be done on a non-VMS system.
 */
#define VMS_MSG_IDENT "V3.0-000"

/* VMS-compatible "severity" values (bits 2:0): */
#define ZE_S_WARNING 0x00
#define ZE_S_SUCCESS 0x01
#define ZE_S_ERROR   0x02
#define ZE_S_INFO    0x03
#define ZE_S_SEVERE  0x04
#define ZE_S_UNUSED  0x07

/* Flags: */
#define ZE_S_PERR    0x10


 /* Error return values.  The values 0..4 and 12..18 follow the conventions
   of PKZIP.   The values 4..10 are all assigned to "insufficient memory"
   by PKZIP, so the codes 5..10 are used here for other purposes. */
#define ZE_MISS         -1      /* used by procname(), zipbare() */
#define ZE_OK           0       /* success */
#define ZE_EOF          2       /* unexpected end of zip file */
#define ZE_FORM         3       /* zip file structure error */
#define ZE_MEM          4       /* out of memory */
#define ZE_LOGIC        5       /* internal logic error */
#define ZE_BIG          6       /* entry too large to split, read, or write */
#define ZE_NOTE         7       /* invalid comment format */
#define ZE_TEST         8       /* zip test (-T) failed or out of memory */
#define ZE_ABORT        9       /* user interrupt or termination */
#define ZE_TEMP         10      /* error using a temp file */
#define ZE_READ         11      /* read or seek error */
#define ZE_NONE         12      /* nothing to do */
#define ZE_NAME         13      /* missing or empty zip file */
#define ZE_WRITE        14      /* error writing to a file */
#define ZE_CREAT        15      /* couldn't open to write */
#define ZE_PARMS        16      /* bad command line */
#define ZE_OPEN         18      /* could not open a specified file to read */
#define ZE_COMPERR      19      /* error in compilation options */
#define ZE_ZIP64        20      /* Zip64 not supported */

#define ZE_MAXERR       20      /* the highest error number */


/* Error messages for the ziperr() function in the zip programs. */

#ifdef GLOBALS
struct
{
    char *name;
    char *string;
    int severity;
} ziperrors[ZE_MAXERR + 1] = {
/*  0 */ { "OK",      "Normal successful completion", ZE_S_SUCCESS },
/*  1 */ { "",        "",                             ZE_S_UNUSED },
/*  2 */ { "EOF",     "Unexpected end of zip file",   ZE_S_SEVERE },
/*  3 */ { "FORM",    "Zip file structure invalid",   ZE_S_ERROR },
/*  4 */ { "MEM",     "Out of memory",                ZE_S_SEVERE },
/*  5 */ { "LOGIC",   "Internal logic error",         ZE_S_SEVERE },
/*  6 */ { "BIG",     "Entry too big to split, read, or write",
                                                      ZE_S_ERROR },
/*  7 */ { "NOTE",    "Invalid comment format",       ZE_S_ERROR },
/*  8 */ { "TEST",    "Zip file invalid, could not spawn unzip, or wrong unzip",
                                                      ZE_S_SEVERE },
/*  9 */ { "ABORT",   "Interrupted",                  ZE_S_ERROR },
/* 10 */ { "TEMP",    "Temporary file failure",       ZE_S_SEVERE | ZE_S_PERR },
/* 11 */ { "READ",    "Input file read failure",      ZE_S_SEVERE | ZE_S_PERR },
/* 12 */ { "NONE",    "Nothing to do!",               ZE_S_WARNING },
/* 13 */ { "NAME",    "Missing or empty zip file",    ZE_S_ERROR },
/* 14 */ { "WRITE",   "Output file write failure",    ZE_S_SEVERE | ZE_S_PERR },
/* 15 */ { "CREAT",   "Could not create output file", ZE_S_SEVERE | ZE_S_PERR },
/* 16 */ { "PARMS",   "Invalid command arguments",    ZE_S_ERROR },
/* 17 */ { "",        "",                             ZE_S_UNUSED },
/* 18 */ { "OPEN",    "File not found or no read permission",
                                                      ZE_S_ERROR | ZE_S_PERR },
/* 19 */ { "COMPERR", "Not supported",                ZE_S_SEVERE },
/* 20 */ { "ZIP64",   "Attempt to read unsupported Zip64 archive",
                                                      ZE_S_SEVERE }
#  ifdef AZTEC_C
          ,     /* extremely lame compiler bug workaround */
#  endif
};
#else /* !GLOBALS */
/* Error messages for ziperr() */
extern struct
{
    char *name;
    char *string;
    int severity;
} ziperrors[ZE_MAXERR + 1];
#endif /* ?GLOBALS */

/* Macro to determine whether to call perror() or not. */
#define PERR(e) (ziperrors[e].severity & ZE_S_PERR)

/* Macro for easy access to the message string. */
#define ZIPERRORS(e) ziperrors[e].string
