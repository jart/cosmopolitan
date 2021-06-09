/*
  Copyright (c) 1990-2001 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2000-Apr-09 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/*---------------------------------------------------------------------------

  helpers.c

  Some useful functions Used by unzip and zip.

  ---------------------------------------------------------------------------*/

/*****************************************************************************/
/*  Includes                                                                 */
/*****************************************************************************/

#include "zip.h"
#include <ctype.h>
#include <time.h>
#include <sound.h>

#include "macstuff.h"
#include "helpers.h"
#include "pathname.h"


/*****************************************************************************/
/*  Global Vars                                                              */
/*****************************************************************************/


extern int noisy;
extern char MacPathEnd;
extern char *zipfile;   /* filename of the Zipfile */
extern char *tempzip;   /* Temporary zip file name */
extern ZCONST unsigned char MacRoman_to_WinCP1252[128];


static char         argStr[1024];
static char         *argv[MAX_ARGS + 1];



/*****************************************************************************/
/*  Functions                                                                */
/*****************************************************************************/


/*
**  Copy a C string to a Pascal string
**
*/

unsigned char *CToPCpy(unsigned char *pstr, char *cstr)
{
    register char *dptr;
    register unsigned len;

        len=0;
        dptr=(char *)pstr+1;
    while (len<255 && (*dptr++ = *cstr++)!='\0') ++len;
    *pstr= (unsigned char)len;
  return pstr;
}


/*
**  Copy a Pascal string to a C string
**
*/

char *PToCCpy(unsigned char *pstr, char *cstr)
{
strncpy(cstr, (char *) &pstr[1], *pstr);
    cstr[pstr[0]] = '\0';  /* set endmarker for c-string */
return cstr;
}


/*
**  strcpy() and strcat() work-alikes which allow overlapping buffers.
*/

char *sstrcpy(char *to,const char *from)
{
    memmove(to, from, 1+strlen(from));
    return to;
}

char *sstrcat(char *to,const char *from)
{
    sstrcpy(to + strlen(to), from);
    return to;
}



/*
**  Alloc memory and init it
**
*/

char *StrCalloc(unsigned short size)
{
char *strPtr = NULL;

if ((strPtr = calloc(size, sizeof(char))) == NULL)
    printerr("StrCalloc failed:", -1, size, __LINE__, __FILE__, "");

Assert_it(strPtr,"strPtr == NULL","")
return strPtr;
}



/*
**  Release only non NULL pointers
**
*/

char *StrFree(char *strPtr)
{

if (strPtr != NULL)
    {
    free(strPtr);
    }

return NULL;
}




/*
**  Return a value in a binary string
**
*/

char *sBit2Str(unsigned short value)
{
  static char str[sizeof(value)*8];
  int biz    = 16;
  int strwid = 16;
  int i, j;
  char *tempPtr = str;

      j = strwid - (biz + (biz >> 2)- (biz % 4 ? 0 : 1));

      for (i = 0; i < j; i++) {
            *tempPtr++ = ' ';
      }
      while (--biz >= 0)
      {
            *tempPtr++ = ((value >> biz) & 1) + '0';
            if (!(biz % 4) && biz) {
                  *tempPtr++ = ' ';
            }
      }
      *tempPtr = '\0';

  return str;
}




/*
**  Parse commandline style arguments
**
*/

int ParseArguments(char *s, char ***arg)
{
    int  n = 1, Quote = 0;
    char *p = s, *p1, c;

    argv[0] = GetAppName();

    *arg = argv;

    p1 = (char *) argStr;
    while ((c = *p++) != 0) {
        if (c==' ') continue;
        argv[n++] = p1;
        if (n > MAX_ARGS)
            return (n-1);
        do {
            if (c=='\\' && *p++)
                c = *p++;
            else
                if ((c=='"') || (c == '\'')) {
                    if (!Quote) {
                        Quote = c;
                        continue;
                    }
                    if (c == Quote) {
                        Quote = 0;
                        continue;
                    }
                }
            *p1++ = c;
        } while (*p && ((c = *p++) != ' ' || Quote));
        *p1++ = '\0';
    }
    return n;
}



/*
**  Print commandline style arguments
**
*/

void PrintArguments(int argc, char **argv)
{

printf("\n Arguments:");
printf("\n --------------------------");

while(--argc >= 0)
    printf("\n argc: %d  argv: [%s]", argc, &*argv[argc]);

printf("\n --------------------------\n\n");
return;
}



/*
**  return some error-msg on file-system
**
*/

int PrintUserHFSerr(int cond, int err, char *msg2)
{
char *msg;

if (cond != 0)
    {
    switch (err)
        {
         case -35:
            msg = "No such Volume";
         break;

         case -56:
            msg = "No such Drive";
         break;

         case -37:
            msg = "Bad Volume Name";
         break;

         case -49:
            msg = "File is already open for writing";
         break;

         case -43:
            msg = "Directory/File not found";
         break;

         case -120:
            msg = "Directory/File not found or incomplete pathname";
         break;

        default: return err;
         }
    fprintf(stderr, "\n\n Error: %s ->%s", msg, msg2);
    exit(err);
    }

return 0;
}



/*
**  Check mounted volumes and return number of volumes
**  with the same name.
*/

short CheckMountedVolumes(char *FullPath)
{
FSSpec  volumes[50];        /* 50 Volumes should be enough */
char VolumeName[257], volume[257];
short actVolCount, volIndex = 1, VolCount = 0;
OSErr err;
int i;

GetVolumeFromPath(FullPath, VolumeName);

err = OnLine(volumes, 50, &actVolCount, &volIndex);
printerr("OnLine:", (err != -35) && (err != 0), err, __LINE__, __FILE__, "");

for (i=0; i < actVolCount; i++)
    {
    PToCCpy(volumes[i].name,volume);
    if (stricmp(volume, VolumeName) == 0) VolCount++;
    }
printerr("OnLine: ", (VolCount == 0), VolCount, __LINE__, __FILE__, FullPath);

return VolCount;
}








/*
**  compares strings, ignoring differences in case
**
*/

int stricmp(const char *p1, const char *p2)
{
int diff;

while (*p1 && *p2)
    {
    if (*p1 != *p2)
        {
        if (isalpha(*p1) && isalpha(*p2))
            {
            diff = toupper(*p1) - toupper(*p2);
            if (diff) return diff;
            }
            else break;
        }
        p1++;
        p2++;
    }
return *p1 - *p2;
}



/*
** Convert the MacOS-Strings (Filenames/Findercomments) to a most compatible.
** These strings will be stored in the public area of the zip-archive.
** Every foreign platform (outside macos) will access these strings
** for extraction.
*/

void MakeCompatibleString(char *MacOS_Str,
            const char SpcChar1, const char SpcChar2,
            const char SpcChar3, const char SpcChar4,
            short CurrTextEncodingBase)
{
    char *tmpPtr;
    register uch curch;

    Assert_it(MacOS_Str,"MakeCompatibleString MacOS_Str == NULL","")
    for (tmpPtr = MacOS_Str; (curch = *tmpPtr) != '\0'; tmpPtr++)
    {
        if (curch == SpcChar1)
            *tmpPtr = SpcChar2;
        else
        if (curch == SpcChar3)
            *tmpPtr = SpcChar4;
        else  /* default */
        /* now convert from MacRoman to ISO-8859-1 */
        /* but convert only if MacRoman is activ */
            if ((CurrTextEncodingBase == kTextEncodingMacRoman) &&
                (curch > 127))
                   {
                    *tmpPtr = (char)MacRoman_to_WinCP1252[curch - 128];
                   }
    }  /* end for */
}




Boolean CheckForSwitch(char *Switch, int argc, char **argv)
{
  char *p;              /* steps through option arguments */
  int i;                /* arg counter, root directory flag */

  for (i = 1; i < argc; i++)
  {
    if (argv[i][0] == '-')
    {
      if (argv[i][1])
        {
        for (p = argv[i]+1; *p; p++)
            {
            if (*p == Switch[0])
                {
                return true;
                }
            if ((Switch[1] != NULL) &&
                ((*p == Switch[0]) && (*p == Switch[1])))
                {
                return true;
                }
            }
         }
     }
  }

return false;
}







#if (defined(USE_SIOUX) || defined(MACUNZIP_STANDALONE))

/*
**  checks the condition and returns an error-msg
**  this function is for internal use only
*/

OSErr printerr(const char *msg, int cond, int err, int line, char *file,
              const char *msg2)
{

if (cond != 0)
    {
    fprintf(stderr, "\nint err: %d: %s %d [%d/%s] {%s}\n", clock(), msg, err,
            line, file, msg2);
    }

return cond;
}


/*
fake-functions:
Not Implemented for metrowerks SIOUX
*/

void leftStatusString(char *status)
{
status = status;
}


void rightStatusString(char *status)
{
status = status;
}



void DoWarnUserDupVol( char *FullPath )
{
  char VolName[257];
  GetVolumeFromPath(FullPath,  VolName);

  printf("\n There are more than one volume that has the same name !!\n");

  printf("\n Volume: %s\n",VolName);

  printf("\n This port has one weak point:");
  printf("\n It is based on pathnames. As you may be already know:");
  printf("\n Pathnames are not unique on a Mac !");
  printf("\n MacZip has problems to find the correct location of");
  printf("\n the archive or the files.\n");

  printf("\n My (Big) recommendation:  Name all your volumes with an");
  printf("\n unique name and MacZip will run without any problem.");
}



#endif
