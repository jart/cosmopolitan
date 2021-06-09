/*
  Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 1999-Oct-05 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, both of these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html
*/
/*
 Example header file
*/
#ifndef _EXAMPLE_H
#define _EXAMPLE_H

#include <windows.h>
#include <assert.h>    /* required for all Windows applications */
#include <stdlib.h>
#include <stdio.h>
#include <commdlg.h>
#include <dlgs.h>
#include <windowsx.h>

#ifndef EXPENTRY
#define EXPENTRY WINAPI
#endif

#include "structs.h"

/* Defines */
#ifndef MSWIN
#define MSWIN
#endif

typedef int (WINAPI * _DLL_ZIP)(ZCL);
typedef int (WINAPI * _ZIP_USER_FUNCTIONS)(LPZIPUSERFUNCTIONS);
typedef BOOL (WINAPI * ZIPSETOPTIONS)(LPZPOPT);

/* Global variables */

extern LPZIPUSERFUNCTIONS lpZipUserFunctions;

extern HINSTANCE hZipDll;

extern int hFile;                 /* file handle             */

/* Global functions */

int WINAPI DisplayBuf(char far *, unsigned long int);
extern _DLL_ZIP ZipArchive;
extern _ZIP_USER_FUNCTIONS ZipInit;
extern ZIPSETOPTIONS ZipSetOptions;

#endif /* _EXAMPLE_H */

