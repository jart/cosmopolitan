/*
  win32/win32zip.h - Zip 3

  Copyright (c) 1990-2007 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2007-Mar-4 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
#ifndef _WIN32ZIP_H
#define _WIN32ZIP_H

/*
 * NT specific functions for ZIP.
 */

int GetFileMode(char *name);
#ifdef UNICODE_SUPPORT
int GetFileModeW(wchar_t *name);
#endif
long GetTheFileTime(char *name, iztimes *z_times);

int IsFileNameValid(char *name);
int IsFileSystemOldFAT(char *dir);
#ifdef UNICODE_SUPPORT
int IsFileSystemOldFATW(wchar_t *dir);
#endif
void ChangeNameForFAT(char *name);

char *getVolumeLabel(int drive, ulg *vtime, ulg *vmode, time_t *vutim);

#if 0 /* never used ?? */
char *StringLower(char *);
#endif

char *GetLongPathEA(char *name);
#ifdef UNICODE_SUPPORT
wchar_t *GetLongPathEAW(wchar_t *name);
#endif

#endif /* _WIN32ZIP_H */
