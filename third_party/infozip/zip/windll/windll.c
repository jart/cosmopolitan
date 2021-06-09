/*
  windll/windll.c - Zip 3

  Copyright (c) 1990-2004 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2003-May-08 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/*
 *  windll.c by Mike White loosly based on Mark Adler's zip.c
 */
#include <windows.h>
#include <process.h>
#include <signal.h>
#include <stdarg.h>
#include <ctype.h>
#include "../zip.h"
#include "windll.h"

HINSTANCE hCurrentInst;
#ifdef ZIPLIB
/*  DLL Entry Point */
#ifdef __BORLANDC__
#pragma argsused
/* Borland seems to want DllEntryPoint instead of DllMain like MSVC */
#define DllMain DllEntryPoint
#endif
#ifdef WIN32
BOOL WINAPI DllMain( HINSTANCE hInstance,
                     DWORD dwReason,
                     LPVOID plvReserved)
#else
int WINAPI LibMain( HINSTANCE hInstance,
                        WORD wDataSegment,
                        WORD wHeapSize,
                        LPSTR lpszCmdLine )
#endif
{
#ifndef WIN32
/* The startup code for the DLL initializes the local heap(if there is one)
 with a call to LocalInit which locks the data segment. */

if ( wHeapSize != 0 )
   {
   UnlockData( 0 );
   }
hCurrentInst = hInstance;
return 1;   /* Indicate that the DLL was initialized successfully. */
#else
BOOL rc = TRUE;
switch( dwReason )
   {
   case DLL_PROCESS_ATTACH:
      // DLL is loaded. Do your initialization here.
      // If cannot init, set rc to FALSE.
      hCurrentInst = hInstance;
      break;

   case DLL_PROCESS_DETACH:
      // DLL is unloaded. Do your cleanup here.
      break;
   default:
      break;
   }
return rc;
#endif
}

#ifdef __BORLANDC__
#pragma argsused
#endif
int FAR PASCAL WEP ( int bSystemExit )
{
return 1;
}
#endif /* ZIPLIB */

LPSTR szCommentBuf;
HANDLE hStr;

void comment(unsigned int comlen)
{
unsigned int i;
if (comlen > 65534L)
   comlen = (unsigned int) 65534L;
hStr = GlobalAlloc( GPTR, (DWORD)65535L);
if ( !hStr )
   {
   hStr = GlobalAlloc( GPTR, (DWORD) 2);
   szCommentBuf = GlobalLock(hStr);
   szCommentBuf[0] = '\0';
   return;
   }

szCommentBuf = GlobalLock(hStr);
if (comlen)
   {
   for (i = 0; i < comlen; i++)
       szCommentBuf[i] = zcomment[i];
   szCommentBuf[comlen] = '\0';
   }
else
   szCommentBuf[0] = '\0';
free(zcomment);
zcomment = malloc(1);
*zcomment = 0;
lpZipUserFunctions->comment(szCommentBuf);
return;
}

#define STDIO_BUF_SIZE 16384

int __far __cdecl printf(const char *format, ...)
{
va_list argptr;
HANDLE hMemory;
LPSTR pszBuffer;
int len;

va_start(argptr, format);
hMemory = GlobalAlloc(GMEM_MOVEABLE, STDIO_BUF_SIZE);
WinAssert(hMemory);
if (!hMemory)
   {
   return 0;
   }
pszBuffer = (LPSTR)GlobalLock(hMemory);
WinAssert(pszBuffer);
len = wvsprintf(pszBuffer, format, argptr);
va_end(argptr);
WinAssert(strlen(pszBuffer) < STDIO_BUF_SIZE);
len = lpZipUserFunctions->print(pszBuffer, len);
GlobalUnlock(hMemory);
GlobalFree(hMemory);
return len;
}

/* fprintf clone for code in zip.c, etc. */
int __far __cdecl fprintf(FILE *file, const char *format, ...)
{
va_list argptr;
HANDLE hMemory;
LPSTR pszBuffer;
int len;

va_start(argptr, format);
hMemory = GlobalAlloc(GMEM_MOVEABLE, STDIO_BUF_SIZE);
WinAssert(hMemory);
if (!hMemory)
   {
   return 0;
   }
pszBuffer = GlobalLock(hMemory);
WinAssert(pszBuffer);
len = wvsprintf(pszBuffer, format, argptr);
va_end(argptr);
WinAssert(strlen(pszBuffer) < STDIO_BUF_SIZE);
if ((file == stderr) || (file == stdout))
   {
   len = lpZipUserFunctions->print(pszBuffer, len);
   }
else
   len = write(fileno(file),(char far *)(pszBuffer), len);
GlobalUnlock(hMemory);
GlobalFree(hMemory);
return len;
}

void __far __cdecl perror(const char *parm1)
{
printf("%s", parm1);
}


