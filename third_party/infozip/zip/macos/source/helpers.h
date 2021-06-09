/*
  Copyright (c) 1990-2001 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2000-Apr-09 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
#ifndef HELPERS_H
#define HELPERS_H       1

 /* Convert a C string to a Pascal string */
unsigned char *CToPCpy(unsigned char *pstr, char *cstr);

 /* Convert a Pascal string to a C string */
char *PToCCpy(unsigned char *pstr, char *cstr);

char *sstrcpy(char *to,const char *from);
char *sstrcat(char *to,const char *from);

char *StrCalloc(unsigned short size);
char *StrFree(char *strPtr);

char *sBit2Str(unsigned short value);

void print_extra_info(void);

int ParseArguments(char *s, char ***arg);
void PrintArguments(int argc, char **argv);

Boolean IsZipFile(char *name);
OSErr printerr(const char *msg, int cond, int err, int line, char *file,
               const char *msg2);
int PrintUserHFSerr(int cond, int err, char *msg2);

short CheckMountedVolumes(char *FullPath);
void DoWarnUserDupVol(char *path);

void PrintFileInfo(void);

int stricmp(const char *p1, const char *p2);
void leftStatusString(char *status);
void rightStatusString(char *status);

Boolean isZipFile(FSSpec *fileToOpen);

unsigned long MacFileDate_to_UTime(unsigned long mactime);
Boolean CheckForSwitch(char *Switch, int argc, char **argv);

void MakeCompatibleString(char *MacOS_Str,
            const char SpcChar1, const char SpcChar2,
            const char SpcChar3, const char SpcChar4,
            short CurrTextEncodingBase);

#define     MAX_ARGS    25

#endif   /*  HELPERS_H   */
