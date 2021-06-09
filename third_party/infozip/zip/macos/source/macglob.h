/*
  Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 1999-Oct-05 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, both of these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html
*/
#ifndef _MACGLOBAL_
#define _MACGLOBAL_

#include <time.h>

/*
all my Global vars are defined here.
*/

#define ResourceFork    -1
#define DataFork        1
#define NoFork          0

/*
all my Global vars are defined here.
*/
typedef struct {
    short       CurrentFork;
    short       MacZipMode;

    Boolean     isMacStatValid;
    Boolean     HaveGMToffset;

    short CurrTextEncodingBase;

    /* info about the current file */
    Boolean         isDirectory;
    char            FullPath[NAME_MAX];
    char            FileName[NAME_MAX];
    FSSpec          fileSpec;

    long            dirID;
    CInfoPBRec      fpb;

    /* time infos about the current file */
    time_t          CreatDate;
    time_t          ModDate;
    time_t          BackDate;
    long            Cr_UTCoffs; /* offset "local time - UTC" for CreatDate */
    long            Md_UTCoffs; /* offset "local time - UTC" for ModDate */
    long            Bk_UTCoffs; /* offset "local time - UTC" for BackDate */

    /* some statistics over all*/
    unsigned long   FoundFiles;
    unsigned long   FoundDirectories;
    unsigned long   RawCountOfItems;
    unsigned long   BytesOfData;

    unsigned long   attrsize;

    /* some switches and user parameters */
    Boolean         DataForkOnly;
    Boolean         StoreFullPath;
    Boolean         StoreFoldersAlso;  /* internal switch is true if '-r' is set */
    unsigned short  SearchLevels;
    char            Pattern[NAME_MAX];
    Boolean         IncludeInvisible;
    Boolean         StatingProgress;

    char            SearchDir[NAME_MAX];
    char            CurrentPath[NAME_MAX];

    /* current zip / tempzip file info */
    char            ZipFullPath[NAME_MAX];

    FSSpec          ZipFileSpec;
    unsigned long   ZipFileType;
    char            TempZipFullPath[NAME_MAX];
    FSSpec          TempZipFileSpec;

} MacZipGlobals;



void UserStop(void);


#endif
