/*
  api.c - Zip 3

  Copyright (c) 1990-2007 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2007-Mar-4 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/*---------------------------------------------------------------------------

  api.c

  This module supplies a Zip dll engine for use directly from C/C++
  programs.

  The entry points are:

    ZpVer *ZpVersion(void);
    int EXPENTRY ZpInit(LPZIPUSERFUNCTIONS lpZipUserFunc);
    int EXPENTRY ZpArchive(ZCL C, LPZPOPT Opts);

  This module is currently only used by the Windows dll, and is not used at
  all by any of the other platforms, although it should be easy enough to
  implement this on most platforms.

  ---------------------------------------------------------------------------*/
#define __API_C

#include <malloc.h>
#ifdef WINDLL
#  include <windows.h>
#  include "windll/windll.h"
#endif

#ifdef OS2
#  define  INCL_DOSMEMMGR
#  include <os2.h>
#endif

#ifdef __BORLANDC__
#include <dir.h>
#endif
#include <direct.h>
#include <ctype.h>
#include "api.h"                /* this includes zip.h */
#include "crypt.h"
#include "revision.h"
#ifdef USE_ZLIB
#  include "zlib.h"
#endif


DLLPRNT *lpZipPrint;
DLLPASSWORD *lpZipPassword;
extern DLLCOMMENT *lpComment;
ZIPUSERFUNCTIONS ZipUserFunctions, far * lpZipUserFunctions;

int ZipRet;
char szOrigDir[PATH_MAX];
BOOL fNo_int64 = FALSE; /* flag for DLLSERVICE_NO_INT64 */

/* Local forward declarations */
extern int  zipmain OF((int, char **));
int AllocMemory(unsigned int, char *, char *, BOOL);
int ParseString(LPSTR, unsigned int);
void FreeArgVee(void);

ZPOPT Options;
char **argVee;
unsigned int argCee;

/*---------------------------------------------------------------------------
    Local functions
  ---------------------------------------------------------------------------*/

char szRootDir[PATH_MAX], szExcludeList[PATH_MAX], szIncludeList[PATH_MAX], szTempDir[PATH_MAX];

int ParseString(LPSTR s, unsigned int ArgC)
{
unsigned int i;
int root_flag, m, j;
char *str1, *str2, *str3;
size_t size;

i = ArgC;
str1 = (char *) malloc(lstrlen(s)+4);
lstrcpy(str1, s);
lstrcat(str1, " @");

if ((szRootDir != NULL) && (szRootDir[0] != '\0'))
    {
    root_flag = TRUE;
    if (szRootDir[lstrlen(szRootDir)-1] != '\\')
        lstrcat(szRootDir, "\\");
    }
else
    root_flag = FALSE;

str2 = strchr(str1, '\"'); /* get first occurance of double quote */

while ((str3 = strchr(str1, '\t')) != NULL)
    {
    str3[0] = ' '; /* Change tabs into a single space */
    }

/* Note that if a quoted string contains multiple adjacent spaces, they
   will not be removed, because they could well point to a valid
   folder/file name.
*/
while ((str2 = strchr(str1, '\"')) != NULL)
    /* Found a double quote if not NULL */
    {
    str3 = strchr(str2+1, '\"'); /* Get the second quote */
    if (str3 == NULL)
        {
        free(str1);
        return ZE_PARMS; /* Something is screwy with the
                            string, bail out */
        }
    str3[0] = '\0';  /* terminate str2 with a NULL */

    /* strip unwanted fully qualified path from entry */
    if (root_flag)
        if ((_strnicmp(szRootDir, str2+1, lstrlen(szRootDir))) == 0)
            {
            m = 0;
            str2++;
            for (j = lstrlen(szRootDir); j < lstrlen(str2); j++)
                str2[m++] = str2[j];
            str2[m] = '\0';
            str2--;
            }
    size = _msize(argVee);
    if ((argVee = (char **)realloc(argVee, size + sizeof(char *))) == NULL)
        {
        fprintf(stdout, "Unable to allocate memory in zip dll\n");
        return ZE_MEM;
        }
    /* argCee is incremented in AllocMemory */
    if (AllocMemory(i, str2+1, "Creating file list from string", TRUE) != ZE_OK)
        {
        free(str1);
        return ZE_MEM;
        }
    i++;
    str3+=2;        /* Point past the whitespace character */
    str2[0] = '\0'; /* Terminate str1 */
    lstrcat(str1, str3);
    }    /* end while */

/* points to first occurance of a space */
str2 = strchr(str1, ' ');

/*  Go through the string character by character, looking for instances
    of two spaces together. Terminate when you find the trailing @
*/
while ((str2[0] != '\0') && (str2[0] != '@'))
    {
    while ((str2[0] == ' ') && (str2[1] == ' '))
        {
        str3 = &str2[1];
        str2[0] = '\0';
        lstrcat(str1, str3);
        }
    str2++;
    }

/* Do we still have a leading space? */
if (str1[0] == ' ')
    {
    str3 = &str1[1];
    lstrcpy(str1, str3); /* Dump the leading space */
    }


/* Okay, now we have gotten rid of any tabs and replaced them with
   spaces, and have replaced multiple spaces with a single space. We
   couldn't do this before because the folder names could have actually
   contained these characters.
*/

str2 = str3 = str1;

while ((str2[0] != '\0') && (str3[0] != '@'))
    {
    str3 = strchr(str2+1, ' ');
    str3[0] = '\0';
    /* strip unwanted fully qualified path from entry */
    if (root_flag)
        if ((_strnicmp(szRootDir, str2, lstrlen(szRootDir))) == 0)
           {
            m = 0;
            for (j = lstrlen(Options.szRootDir); j < lstrlen(str2); j++)
            str2[m++] = str2[j];
            str2[m] = '\0';
            }
    size = _msize(argVee);
    if ((argVee = (char **)realloc(argVee, size + sizeof(char *))) == NULL)
        {
        fprintf(stdout, "Unable to allocate memory in zip dll\n");
        return ZE_MEM;
        }
    if (AllocMemory(i, str2, "Creating file list from string", TRUE) != ZE_OK)
        {
        free(str1);
        return ZE_MEM;
        }
    i++;
    str3++;
    str2 = str3;
    }
free(str1);
return ZE_OK;
}

int AllocMemory(unsigned int i, char *cmd, char *str, BOOL IncrementArgCee)
{
if ((argVee[i] = (char *) malloc( sizeof(char) * strlen(cmd)+1 )) == NULL)
   {
   if (IncrementArgCee)
       argCee++;
   FreeArgVee();
   fprintf(stdout, "Unable to allocate memory in zip library at %s\n", str);
   return ZE_MEM;
   }
strcpy( argVee[i], cmd );
argCee++;
return ZE_OK;
}

void FreeArgVee(void)
{
unsigned i;

/* Free the arguments in the array */
for (i = 0; i < argCee; i++)
    {
    free (argVee[i]);
    argVee[i] = NULL;
    }
/* Then free the array itself */
free(argVee);

/* Restore the original working directory */
chdir(szOrigDir);
#ifdef __BORLANDC__
setdisk(toupper(szOrigDir[0]) - 'A');
#endif

}


/*---------------------------------------------------------------------------
    Documented API entry points
  ---------------------------------------------------------------------------*/

int EXPENTRY ZpInit(LPZIPUSERFUNCTIONS lpZipUserFunc)
{
ZipUserFunctions = *lpZipUserFunc;
lpZipUserFunctions = &ZipUserFunctions;

if (!lpZipUserFunctions->print ||
    !lpZipUserFunctions->comment)
    return FALSE;

return TRUE;
}

int EXPENTRY ZpArchive(ZCL C, LPZPOPT Opts)
/* Add, update, freshen, or delete zip entries in a zip file.  See the
   command help in help() zip.c */
{
int k, j, m;
size_t size;

Options = *Opts; /* Save off options, and make them available locally */
szRootDir[0] = '\0';
szExcludeList[0] = '\0';
szIncludeList[0] = '\0';
szTempDir[0] = '\0';
if (Options.szRootDir) lstrcpy(szRootDir, Options.szRootDir);
if (Options.szExcludeList) lstrcpy(szExcludeList, Options.szExcludeList);
if (Options.szIncludeList) lstrcpy(szIncludeList, Options.szIncludeList);
if (Options.szTempDir) lstrcpy(szTempDir, Options.szTempDir);

getcwd(szOrigDir, PATH_MAX); /* Save current drive and directory */

if ((szRootDir != NULL) && (szRootDir[0] != '\0'))
   {
   /* Make sure there isn't a trailing slash */
   if (szRootDir[lstrlen(szRootDir)-1] == '\\')
       szRootDir[lstrlen(szRootDir)-1] = '\0';

   chdir(szRootDir);
#ifdef __BORLANDC__
   setdisk(toupper(szRootDir[0]) - 'A');
#endif
   }

argCee = 0;

/* malloc additional 40 to allow for additional command line arguments. Note
   that we are also adding in the count for the include lists as well as the
   exclude list. */
if ((argVee = (char **)malloc((C.argc+40)*sizeof(char *))) == NULL)
   {
   fprintf(stdout, "Unable to allocate memory in zip dll\n");
   return ZE_MEM;
   }
if ((argVee[argCee] = (char *) malloc( sizeof(char) * strlen("wiz.exe")+1 )) == NULL)
   {
   free(argVee);
   fprintf(stdout, "Unable to allocate memory in zip dll\n");
   return ZE_MEM;
   }
strcpy( argVee[argCee], "wiz.exe" );
argCee++;


/* Set compression level efficacy -0...-9 */
if (AllocMemory(argCee, "-0", "Compression", FALSE) != ZE_OK)
    return ZE_MEM;

/* Check to see if the compression level is set to a valid value. If
 not, then set it to the default.
*/
if ((Options.fLevel < '0') || (Options.fLevel > '9'))
    {
    Options.fLevel = '6';
    if (!Options.fDeleteEntries)
        fprintf(stdout, "Compression level set to invalid value. Setting to default\n");
    }

argVee[argCee-1][1] = Options.fLevel;

if (Options.fOffsets)    /* Update offsets for SFX prefix */
   {
   if (AllocMemory(argCee, "-A", "Offsets", FALSE) != ZE_OK)
        return ZE_MEM;
    }
if (Options.fDeleteEntries)    /* Delete files from zip file -d */
   {
   if (AllocMemory(argCee, "-d", "Delete", FALSE) != ZE_OK)
        return ZE_MEM;
   }
if (Options.fNoDirEntries) /* Do not add directory entries -D */
   {
        if (AllocMemory(argCee, "-D", "No Dir Entries", FALSE) != ZE_OK)
            return ZE_MEM;
   }
if (Options.fFreshen) /* Freshen zip file--overwrite only -f */
   {
   if (AllocMemory(argCee, "-f", "Freshen", FALSE) != ZE_OK)
        return ZE_MEM;
   }
if (Options.fRepair)  /* Fix archive -F or -FF */
   {
   if (Options.fRepair == 1)
      {
      if (AllocMemory(argCee, "-F", "Repair", FALSE) != ZE_OK)
          return ZE_MEM;
      }
   else
      {
      if (AllocMemory(argCee, "-FF", "Repair", FALSE) != ZE_OK)
        return ZE_MEM;
      }
   }
if (Options.fGrow) /* Allow appending to a zip file -g */
   {
   if (AllocMemory(argCee, "-g", "Appending", FALSE) != ZE_OK)
        return ZE_MEM;
   }
if (Options.fJunkDir) /* Junk directory names -j */
   {
   if (AllocMemory(argCee, "-j", "Junk Dir Names", FALSE) != ZE_OK)
        return ZE_MEM;
   }
if (Options.fEncrypt) /* encrypt -e */
   {
   if (AllocMemory(argCee, "-e", "Encrypt", FALSE) != ZE_OK)
        return ZE_MEM;
   }
if (Options.fJunkSFX) /* Junk sfx prefix */
   {
   if (AllocMemory(argCee, "-J", "Junk SFX", FALSE) != ZE_OK)
        return ZE_MEM;
   }

if (Options.fForce) /* Make entries using DOS names (k for Katz) -k */
   {
   if (AllocMemory(argCee, "-k", "Force DOS", FALSE) != ZE_OK)
        return ZE_MEM;
   }

if (Options.fLF_CRLF) /* Translate LF_CRLF -l */
   {
   if (AllocMemory(argCee, "-l", "LF-CRLF", FALSE) != ZE_OK)
        return ZE_MEM;
   }
if (Options.fCRLF_LF) /* Translate CR/LF to LF -ll */
   {
   if (AllocMemory(argCee, "-ll", "CRLF-LF", FALSE) != ZE_OK)
        return ZE_MEM;
   }
if (Options.fMove) /* Delete files added to or updated in zip file -m */
   {
   if (AllocMemory(argCee, "-m", "Move", FALSE) != ZE_OK)
        return ZE_MEM;
   }

if (Options.fLatestTime) /* Set zip file time to time of latest file in it -o */
   {
   if (AllocMemory(argCee, "-o", "Time", FALSE) != ZE_OK)
        return ZE_MEM;
   }

if (Options.fComment) /* Add archive comment "-z" */
   {
   if (AllocMemory(argCee, "-z", "Comment", FALSE) != ZE_OK)
        return ZE_MEM;
   }

if (Options.fQuiet) /* quiet operation -q */
   {
   if (AllocMemory(argCee, "-q", "Quiet", FALSE) != ZE_OK)
        return ZE_MEM;
   }
if (Options.fRecurse == 1) /* recurse into subdirectories -r */
   {
   if (AllocMemory(argCee, "-r", "Recurse -r", FALSE) != ZE_OK)
        return ZE_MEM;
   }
else if (Options.fRecurse == 2) /* recurse into subdirectories -R */
   {
   if (AllocMemory(argCee, "-R", "Recurse -R", FALSE) != ZE_OK)
        return ZE_MEM;
   }
if (Options.fSystem)  /* include system and hidden files -S */
   {
   if (AllocMemory(argCee, "-S", "System", FALSE) != ZE_OK)
        return ZE_MEM;
   }
if (Options.fExcludeDate)    /* Exclude files newer than specified date -tt */
   {
     if ((Options.Date != NULL) && (Options.Date[0] != '\0'))
        {
        if (AllocMemory(argCee, "-tt", "Date", FALSE) != ZE_OK)
            return ZE_MEM;
        if (AllocMemory(argCee, Options.Date, "Date", FALSE) != ZE_OK)
            return ZE_MEM;
        }
   }

if (Options.fIncludeDate)    /* include files newer than specified date -t */
   {
     if ((Options.Date != NULL) && (Options.Date[0] != '\0'))
        {
        if (AllocMemory(argCee, "-t", "Date", FALSE) != ZE_OK)
            return ZE_MEM;
       if (AllocMemory(argCee, Options.Date, "Date", FALSE) != ZE_OK)
            return ZE_MEM;
        }
   }

if (Options.fUpdate) /* Update zip file--overwrite only if newer -u */
    {
    if (AllocMemory(argCee, "-u", "Update", FALSE) != ZE_OK)
        return ZE_MEM;
    }
if (Options.fVerbose)  /* Mention oddities in zip file structure -v */
    {
    if (AllocMemory(argCee, "-v", "Verbose", FALSE) != ZE_OK)
        return ZE_MEM;
    }
if (Options.fVolume)  /* Include volume label -$ */
    {
    if (AllocMemory(argCee, "-$", "Volume", FALSE) != ZE_OK)
        return ZE_MEM;
    }
if (Options.szSplitSize != NULL)   /* Turn on archive splitting */
    {
    if (AllocMemory(argCee, "-s", "Splitting", FALSE) != ZE_OK)
        return ZE_MEM;
    if (AllocMemory(argCee, Options.szSplitSize, "Split size", FALSE) != ZE_OK)
        return ZE_MEM;
    }
if (lpZipUserFunctions->split != NULL)   /* Turn on archive split destinations select */
    {
    if (AllocMemory(argCee, "-sp", "Split Pause Select Destination", FALSE) != ZE_OK)
        return ZE_MEM;
    }
#ifdef WIN32
if (Options.fPrivilege)  /* Use privileges -! */
   {
   if (AllocMemory(argCee, "-!", "Privileges", FALSE) != ZE_OK)
        return ZE_MEM;
   }
#endif
if (Options.fExtra)  /* Exclude extra attributes -X */
    {
    if (AllocMemory(argCee, "-X", "Extra", FALSE) != ZE_OK)
        return ZE_MEM;
    }
if (Options.IncludeList != NULL) /* Include file list -i */
    {
    if (AllocMemory(argCee, "-i", "Include file list", FALSE) != ZE_OK)
        return ZE_MEM;
    k = 0;
    if (Options.IncludeListCount > 0)
        while ((Options.IncludeList[k] != NULL) && (Options.IncludeListCount != k+1))
            {
            size = _msize(argVee);
            if ((argVee = (char **)realloc(argVee, size + sizeof(char *))) == NULL)
                {
                fprintf(stdout, "Unable to allocate memory in zip dll\n");
                return ZE_MEM;
                }
            if (AllocMemory(argCee, Options.IncludeList[k], "Include file list array", TRUE) != ZE_OK)
                {
                return ZE_MEM;
                }
            k++;
            }
    else
        while (Options.IncludeList[k] != NULL)
            {
            size = _msize(argVee);
            if ((argVee = (char **)realloc(argVee, size + sizeof(char *))) == NULL)
                {
                FreeArgVee();
                fprintf(stdout, "Unable to allocate memory in zip dll\n");
                return ZE_MEM;
                }
            if (AllocMemory(argCee, Options.IncludeList[k], "Include file list array", TRUE) != ZE_OK)
                return ZE_MEM;
            k++;
            }

    if (AllocMemory(argCee, "@", "End of Include List", FALSE) != ZE_OK)
        return ZE_MEM;
    }
if (Options.ExcludeList != NULL)  /* Exclude file list -x */
    {
    if (AllocMemory(argCee, "-x", "Exclude file list", FALSE) != ZE_OK)
        return ZE_MEM;
    k = 0;
    if (Options.ExcludeListCount > 0)
        while ((Options.ExcludeList[k] != NULL) && (Options.ExcludeListCount != k+1))
            {
            size = _msize(argVee);
            if ((argVee = (char **)realloc(argVee, size + sizeof(char *))) == NULL)
                {
                fprintf(stdout, "Unable to allocate memory in zip dll\n");
                return ZE_MEM;
                }
            if (AllocMemory(argCee, Options.ExcludeList[k], "Exclude file list array", TRUE) != ZE_OK)
                return ZE_MEM;
            k++;
            }
    else
        while (Options.ExcludeList[k] != NULL)
            {
            size = _msize(argVee);
            if ((argVee = (char **)realloc(argVee, size + sizeof(char *))) == NULL)
                {
                FreeArgVee();
                fprintf(stdout, "Unable to allocate memory in zip dll\n");
                return ZE_MEM;
                }
            if (AllocMemory(argCee, Options.ExcludeList[k], "Exclude file list array", TRUE) != ZE_OK)
                return ZE_MEM;
            k++;
            }
   if (AllocMemory(argCee, "@", "End of Exclude List", FALSE) != ZE_OK)
        return ZE_MEM;
    }

if (szIncludeList != NULL && szIncludeList[0] != '\0') /* Include file list -i */
    {
    if (AllocMemory(argCee, "-i", "Include file list", FALSE) != ZE_OK)
        return ZE_MEM;
    if ((k = ParseString(szIncludeList, argCee)) != ZE_OK)
        return k;  /* Something was screwy with the parsed string
                      bail out */
    if (AllocMemory(argCee, "@", "End of Include List", FALSE) != ZE_OK)
        return ZE_MEM;
    }
if (szExcludeList != NULL && szExcludeList[0] != '\0')  /* Exclude file list -x */
    {
    if (AllocMemory(argCee, "-x", "Exclude file list", FALSE) != ZE_OK)
        return ZE_MEM;

    if ((k = ParseString(szExcludeList, argCee)) != ZE_OK)
        return k;  /* Something was screwy with the parsed string
                      bail out */

    if (AllocMemory(argCee, "@", "End of Exclude List", FALSE) != ZE_OK)
        return ZE_MEM;
    }

if ((szTempDir != NULL) && (szTempDir[0] != '\0')
     && Options.fTemp) /* Use temporary directory -b */
    {
    if (AllocMemory(argCee, "-b", "Temp dir switch command", FALSE) != ZE_OK)
        return ZE_MEM;
    if (AllocMemory(argCee, szTempDir, "Temporary directory", FALSE) != ZE_OK)
        return ZE_MEM;
    }

if (AllocMemory(argCee, C.lpszZipFN, "Zip file name", FALSE) != ZE_OK)
    return ZE_MEM;

if ((szRootDir != NULL) && (szRootDir[0] != '\0'))
    {
    if (szRootDir[lstrlen(szRootDir)-1] != '\\')
         lstrcat(szRootDir, "\\"); /* append trailing \\ */
    if (C.FNV != NULL)
        {
        for (k = 0; k < C.argc; k++)
            {
            if (AllocMemory(argCee, C.FNV[k], "Making argv", FALSE) != ZE_OK)
                return ZE_MEM;
            if ((_strnicmp(szRootDir, C.FNV[k], lstrlen(szRootDir))) == 0)
                {
                m = 0;
                for (j = lstrlen(szRootDir); j < lstrlen(C.FNV[k]); j++)
                    argVee[argCee-1][m++] = C.FNV[k][j];
                argVee[argCee-1][m] = '\0';
                }
            }
        }

    }
else
  if (C.FNV != NULL)
    for (k = 0; k < C.argc; k++)
        {
        if (AllocMemory(argCee, C.FNV[k], "Making argv", FALSE) != ZE_OK)
            return ZE_MEM;
        }

if (C.lpszAltFNL != NULL)
    {
    if ((k = ParseString(C.lpszAltFNL, argCee)) != ZE_OK)
        return k;  /* Something was screwy with the parsed string
                      bail out
                    */
    }



argVee[argCee] = NULL;

ZipRet = zipmain(argCee, argVee);

/* Free the arguments in the array. Note this also restores the
   current directory
 */
FreeArgVee();

return ZipRet;
}

#if CRYPT
int encr_passwd(int modeflag, char *pwbuf, int size, const char *zfn)
    {
    return (*lpZipUserFunctions->password)(pwbuf, size, ((modeflag == ZP_PW_VERIFY) ?
                  "Verify password: " : "Enter password: "),
                  (char *)zfn);
    }
#endif /* CRYPT */

void EXPENTRY ZpVersion(ZpVer far * p)   /* should be pointer to const struct */
    {
    p->structlen = ZPVER_LEN;

#ifdef BETA
    p->flag = 1;
#else
    p->flag = 0;
#endif
#ifdef CRYPT
    p->fEncryption = TRUE;
#else
    p->fEncryption = FALSE;
#endif
    lstrcpy(p->betalevel, Z_BETALEVEL);
    lstrcpy(p->date, REVDATE);

#ifdef ZLIB_VERSION
    lstrcpy(p->zlib_version, ZLIB_VERSION);
    p->flag |= 2;
#else
    p->zlib_version[0] = '\0';
#endif

#ifdef ZIP64_SUPPORT
    p->flag |= 4; /* Flag that ZIP64 was compiled in. */
#endif

    p->zip.major = Z_MAJORVER;
    p->zip.minor = Z_MINORVER;
    p->zip.patchlevel = Z_PATCHLEVEL;

#ifdef OS2
    p->os2dll.major = D2_MAJORVER;
    p->os2dll.minor = D2_MINORVER;
    p->os2dll.patchlevel = D2_PATCHLEVEL;
#endif
#ifdef WINDLL
    p->windll.major = DW_MAJORVER;
    p->windll.minor = DW_MINORVER;
    p->windll.patchlevel = DW_PATCHLEVEL;
#endif
    }
