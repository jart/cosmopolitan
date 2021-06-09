/*
  Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 1999-Oct-05 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, both of these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html
*/
/*---------------------------------------------------------------------------

  macos.c

  Macintosh-specific routines for use with Info-ZIP's Zip 2.3 and later.

  ---------------------------------------------------------------------------*/


/*****************************************************************************/
/*  Includes                                                                 */
/*****************************************************************************/

#include "zip.h"

#include "revision.h"
#include "crypt.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sound.h>

#include <unistd.h>

#include <Strings.h>
#include <setjmp.h>

/* #include "charmap.h" */
#include "helpers.h"
#include "macstuff.h"
#include "pathname.h"
#include "recurse.h"


/*****************************************************************************/
/*  Macros, typedefs                                                         */
/*****************************************************************************/

#define PATH_END   MacPathEnd

/*****************************************************************************/
/*  Global Vars                                                              */
/*****************************************************************************/

int error_level;   /* used only in ziperr() */


/*  Note: sizeof() returns the size of this allusion
          13 is current length of "XtraStuf.mac:"      */
extern const char ResourceMark[13]; /* var is initialized in file pathname.c */


extern jmp_buf EnvForExit;
MacZipGlobals   MacZip;

unsigned long count_of_Zippedfiles = 0;


/*****************************************************************************/
/*  Module level Vars                                                        */
/*****************************************************************************/

static const char MacPathEnd = ':';   /* the Macintosh dir separator */

/*  Inform Progress vars */
long    estTicksToFinish;
long    createTime;
long    updateTicks;

static char *Time_Est_strings[] = {
        "Zipping Files; Items done:",
        "More than 24 hours",
        "More than %s hours",
        "About %s hours, %s minutes",
        "About an hour",
        "Less than an hour",
        "About %s minutes, %s seconds",
        "About a minute",
        "Less than a minute",
        "About %s seconds",
        "About a second",
        "About 1 minute, %s seconds"};



/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/

int DoCurrentDir(void);

void DoAboutBox(void);
void DoQuit(void);
void DoEventLoop(void);

void ZipInitAllVars(void);
void UserStop(void);
Boolean IsZipFile(char *name);

static long EstimateCompletionTime(const long progressMax,
                            const long progressSoFar, unsigned char percent);
static void UpdateTimeToComplete(void);




#ifdef USE_SIOUX
#include <sioux.h>
void DoWarnUserDupVol( char *FullPath );

/*****************************************************************************/
/*  Functions                                                                */
/*****************************************************************************/

/*
** Standalone Unzip with Metrowerks SIOUX starts here
**
*/
int main(int argc, char **argv)
{
    int return_code;

    SIOUXSettings.asktosaveonclose = FALSE;
    SIOUXSettings.showstatusline = TRUE;

    SIOUXSettings.columns = 100;
    SIOUXSettings.rows    = 40;

    /* 30 = MacZip Johnny Lee's; 40 = new (my) MacZip */
    MacZip.MacZipMode = NewZipMode_EF;

    argc = ccommand(&argv);
    if (verbose) PrintArguments(argc, argv);

    ZipInitAllVars();

    return_code = zipmain(argc, argv);

    if (verbose) printf("\n\n Finish");
    return return_code;
}



/*
** SIOUX needs no extra event handling
**
*/

void UserStop(void)
{
};




/*
** Password enter function '*' printed for each char
**
*/

int macgetch(void)
{
    WindowPtr whichWindow;
    EventRecord theEvent;
    char c;                     /* one-byte buffer for read() to use */

    do {
        SystemTask();
        if (!GetNextEvent(everyEvent, &theEvent))
            theEvent.what = nullEvent;
        else {
            switch (theEvent.what) {
            case keyDown:
                c = theEvent.message & charCodeMask;
                break;
            case mouseDown:
                if (FindWindow(theEvent.where, &whichWindow) ==
                    inSysWindow)
                    SystemClick(&theEvent, whichWindow);
                break;
            case updateEvt:
                break;
            }
        }
    } while (theEvent.what != keyDown);

    printf("*");
    fflush(stdout);

    return (int)c;
}

#endif




/******************************/
/*  Function version_local()  */
/******************************/

/*
** Print Compilers version and compile time/date
**
*/

void version_local()
{
/* prints e.g:
Compiled with  Metrowerks CodeWarrior version 2000 for  PowerPC Processor
 compile time:  Feb  4 1998 17:49:49.
*/

static ZCONST char CompiledWith[] =
                        "\n\nCompiled with %s %x for %s \n %s %s %s.\n\n";

    printf(CompiledWith,


#ifdef __MWERKS__
      " Metrowerks CodeWarrior version", __MWERKS__,
#endif


#ifdef __MC68K__
      " MC68K Processor",
#else
      " PowerPC Processor",
#endif

#ifdef __DATE__
      "compile time: ", __DATE__, __TIME__
#else
      "", "", ""
#endif
    );
} /* end function version_local() */





/*
** Deletes a dir if the switch '-m' is used
**
*/

int deletedir(char *path)
{
static char     Num = 0;
static FSSpec   trashfolder;
static Boolean  FirstCall = true;
static Boolean  Immediate_File_Deletion = false;
OSErr           err;
FSSpec          dirToDelete;
char            currpath[NAME_MAX], *envptr;
CInfoPBRec      fpb;

/* init this function */
if ((path == NULL) ||
    (strlen(path) == 0))
    {
    Num = 0;
    FirstCall = true;
    return -1;
    }

UserStop();

GetCompletePath(currpath,path,&dirToDelete, &err);

if (FirstCall == true)
    {
    FirstCall = false;
    envptr = getenv("Immediate_File_Deletion");
    if (!(envptr == (char *)NULL || *envptr == '\0'))
        {
        if (stricmp(envptr,"yes") == 0)
            Immediate_File_Deletion = true;
        else
            Immediate_File_Deletion = false;
        }
    err = FSpFindFolder(dirToDelete.vRefNum, kTrashFolderType,
                      kDontCreateFolder,&trashfolder);
    printerr("FSpFindFolder:",err,err,__LINE__,__FILE__,path);
    }

    fpb.dirInfo.ioNamePtr   = dirToDelete.name;
    fpb.dirInfo.ioVRefNum   = dirToDelete.vRefNum;
    fpb.dirInfo.ioDrDirID   = dirToDelete.parID;
    fpb.dirInfo.ioFDirIndex = 0;

    err = PBGetCatInfoSync(&fpb);
    printerr("PBGetCatInfo deletedir ", err, err,
             __LINE__, __FILE__, "");

if (fpb.dirInfo.ioDrNmFls > 0)
    {
    return 0;  /* do not move / delete folders which are not empty */
    }

if (Immediate_File_Deletion)
    {
    err = FSpDelete(&dirToDelete);
    return err;
    }

err = CatMove (dirToDelete.vRefNum, dirToDelete.parID,
               dirToDelete.name, trashfolder.parID, trashfolder.name);

/* -48 = file is already existing so we have to rename it before
   moving the file */
if (err == -48)
    {
    Num++;
    if (dirToDelete.name[0] >= 28) /* cut foldername if to long */
        dirToDelete.name[0] = 28;
    P2CStr(dirToDelete.name);
    sprintf(currpath,"%s~%d",(char *)dirToDelete.name,Num);
    C2PStr(currpath);
    C2PStr((char *)dirToDelete.name);
    err = HRename (dirToDelete.vRefNum, dirToDelete.parID,
                   dirToDelete.name, (unsigned char *) currpath);

    err = CatMove (dirToDelete.vRefNum, dirToDelete.parID,
                   (unsigned char *) currpath, trashfolder.parID,
                   trashfolder.name);
    }

return err;
}




/*
** Set the file-type so the archive will get the correct icon, type
** and creator code.
*/

void setfiletype(char *new_f, unsigned long Creator, unsigned long Type)
{
OSErr   err;

if (strcmp(zipfile, new_f) == 0)
    err = FSpChangeCreatorType(&MacZip.ZipFileSpec, Creator, Type);
printerr("FSpChangeCreatorType:", err, err, __LINE__, __FILE__, new_f);

return;
}





/*
** Convert the external (native) filename into Zip's internal Unix compatible
** name space.
*/

char *ex2in(char *externalFilen, int isdir, int *pdosflag)
/* char *externalFilen     external file name */
/* int isdir               input: externalFilen is a directory */
/* int *pdosflag           output: force MSDOS file attributes? */
/* Convert the external file name to a zip file name, returning the malloc'ed
   string or NULL if not enough memory. */
{
  char *internalFilen;              /* internal file name (malloc'ed) */
  char *t;                          /* shortened name */
  char *Pathname;
  char buffer[NAME_MAX];
  int dosflag;

  AssertStr(externalFilen, externalFilen)
  AssertBool(isdir,"")

  dosflag = dosify;  /* default for non-DOS and non-OS/2 */

  /* Find starting point in name before doing malloc */
  for (t = externalFilen; *t == PATH_END; t++)
      ;

  if (!MacZip.StoreFullPath)
        {
        Pathname = StripPartialDir(buffer, MacZip.SearchDir,t);
        }
  else
        {
        Pathname = t;
        }

  /* Make changes, if any, to the copied name (leave original intact) */
  if (!pathput)
    {
    t = last(Pathname, PATH_END);
    }
  else t = Pathname;

  /* Malloc space for internal name and copy it */
  if ((internalFilen = malloc(strlen(t) + 10 + strlen(ResourceMark) )) == NULL)
    return NULL;

  sstrcpy(internalFilen, t);

    /* we have to eliminate illegal chars:
     * The name space for Mac filenames and Zip filenames (unix style names)
     * do both include all printable extended-ASCII characters.  The only
     * difference we have to take care of is the single special character
     * used as path delimiter:
     * ':' on MacOS and '/' on Unix and '\' on Dos.
     * So, to convert between Mac filenames and Unix filenames without any
     * loss of information, we simply interchange ':' and '/'.  Additionally,
     * we try to convert the coding of the extended-ASCII characters into
     * InfoZip's standard ISO 8859-1 codepage table.
     */
  MakeCompatibleString(internalFilen, ':', '/', '/', ':',
                       MacZip.CurrTextEncodingBase);

  /* Returned malloc'ed name */
  if (pdosflag)
    *pdosflag = dosflag;

  if (isdir)
    {
    return internalFilen;      /* avoid warning on unused variable */
    }

  if (dosify)
    {
    msname(internalFilen);
    printf("\n ex2in: %s",internalFilen);
    }

  return internalFilen;
}



/*
** Collect all filenames. Go through all directories
**
*/

int wild(char *Pathpat)
                /* path/pattern to match */
/* If not in exclude mode, expand the pattern based on the contents of the
   file system.  Return an error code in the ZE_ class. */
{
FSSpec Spec;
char fullpath[NAME_MAX];
OSErr   err;

AssertStr(Pathpat, Pathpat);

if (noisy) printf("%s \n\n",GetZipVersionsInfo());

if (extra_fields == 0)
    {
    MacZip.DataForkOnly = true;
    }

/* for switch '-R' -> '.' means current dir */
if (strcmp(Pathpat,".") == 0) sstrcpy(Pathpat,"*");

sstrcpy(MacZip.Pattern,Pathpat);

if (recurse)
    {
    MacZip.StoreFoldersAlso = true;
    MacZip.SearchLevels = 0; /* if 0 we aren't checking levels */
    }
else
    {
    MacZip.StoreFoldersAlso = false;
    MacZip.SearchLevels = 1;
    }

/* make complete path */
GetCompletePath(fullpath, MacZip.Pattern, &Spec,&err);
err = PrintUserHFSerr((err != -43) && (err != 0), err, MacZip.Pattern);
printerr("GetCompletePath:", err, err, __LINE__, __FILE__, fullpath);

/* extract the filepattern */
GetFilename(MacZip.Pattern, fullpath);

/* extract Path and get FSSpec of search-path */
/* get FSSpec of search-path  ; we need a dir to start
   searching for filenames */
TruncFilename(MacZip.SearchDir, fullpath);
GetCompletePath(MacZip.SearchDir, MacZip.SearchDir, &Spec,&err);

if (noisy) {
    if (MacZip.SearchLevels == 0)
        {
        printf("\nSearch Pattern: [%s]   Levels: all", MacZip.Pattern);
        }
    else
        {
        printf("\nSearch Pattern: [%s]   Levels: %d", MacZip.Pattern,
               MacZip.SearchLevels);
        }
    printf("\nSearch Path:    [%s]", MacZip.SearchDir);
    printf("\nZip-File:       [%s] \n",MacZip.ZipFullPath);

}

/* we are working only with pathnames;
 * this can cause big problems on a mac ...
 */
if (CheckMountedVolumes(MacZip.SearchDir) > 1)
    DoWarnUserDupVol(MacZip.SearchDir);

/* start getting all filenames */
err = FSpRecurseDirectory(&Spec, MacZip.SearchLevels);
printerr("FSpRecurseDirectory:", err, err, __LINE__, __FILE__, "");

return ZE_OK;
}



/*
** Convert the internal filename into a external (native).
** The user will see this modified filename.
** For more performance:
** I do not completly switch back to the native macos filename.
** The user will still see directory separator '/' and the converted
** charset.
*/

char *in2ex(char *n)    /* internal file name */
/* Convert the zip file name to an external file name, returning the malloc'ed
   string or NULL if not enough memory. */
{
  char *x;              /* external file name */

  AssertStr(n,n)

  if ((x = malloc(strlen(n) + 1)) == NULL)
    return NULL;

  RfDfFilen2Real(x, n, MacZip.MacZipMode, MacZip.DataForkOnly,
                 &MacZip.CurrentFork);

  return x;
}




/*
** Process on filenames. This function will be called to collect
** the filenames.
*/

int procname(char *filename,         /* name to process */
             int caseflag)           /* true to force case-sensitive match
                                        (always false on a Mac) */
/* Process a name .  Return
   an error code in the ZE_ class. */
{
  int rc;                /* matched flag */

AssertBool(caseflag,"caseflag")
AssertStr(filename,filename)

        /* add or remove name of file */
rc = newname(filename, MacZip.isDirectory, caseflag);

return rc;
}




ulg filetime(
char *f,                /* name of file to get info on */
ulg *a,                 /* return value: file attributes */
long *n,                /* return value: file size */
iztimes *t)             /* return value: access, modific. and creation times */
/* If file *f does not exist, return 0.  Else, return the file's last
   modified date and time as an MSDOS date and time.  The date and
   time is returned in a long with the date most significant to allow
   unsigned integer comparison of absolute times.  Also, if a is not
   a NULL pointer, store the file attributes there, with the high two
   bytes being the Unix attributes, and the low byte being a mapping
   of that to DOS attributes.  If n is not NULL, store the file size
   there.  If t is not NULL, the file's access, modification and creation
   times are stored there as UNIX time_t values.
   If f is "-", use standard input as the file. If f is a device, return
   a file size of -1 */
{
  struct stat s;        /* results of stat() */

  AssertStr(f,f)

  if (strlen(f) == 0) return 0;

  if (SSTAT(f, &s) != 0)
             /* Accept about any file kind including directories
              * (stored with trailing : with -r option)
              */
    return 0;

  if (a != NULL) {
    *a = ((ulg)s.st_mode << 16) | !(s.st_mode & S_IWRITE);
    if (MacZip.isDirectory) {
      *a |= MSDOS_DIR_ATTR;
    }
  }
  if (n != NULL)
    *n = (s.st_mode & UNX_IFMT) == UNX_IFREG ? s.st_size : -1L;
  if (t != NULL) {
    t->atime = s.st_atime;
    t->mtime = s.st_mtime;
    t->ctime = s.st_ctime;   /* on Mac, st_ctime contains creation time! */
  }

  return unix2dostime(&s.st_mtime);
}



void stamp(char *f, ulg d)
/* char *f;                name of file to change */
/* ulg d;                  dos-style time to change it to */
/* Set last updated and accessed time of file f to the DOS time d. */
{
  time_t u[2];          /* argument for utime() */

f = f;

  /* Convert DOS time to time_t format in u */

  u[0] = u[1] = dos2unixtime(d);
/*  utime(f, u);  */
}




/*
**  return only the longest part of the path:
**  second parameter: Volume:test folder:second folder:
**  third parameter:  Volume:test folder:second folder:third folder:file
**  result will be:   third folder:file
**  first parameter:  contains string buffer that will be used to prepend
**                    the "resource mark" part in front of the result when
**                    a resource fork is processed in "M3" mode.
**
*/

char *StripPartialDir(char *CompletePath,
                      const char *PartialPath, const char *FullPath)
{
const char *tmpPtr1 = PartialPath;
const char *tmpPtr2 = FullPath;
int     result;

Assert_it(CompletePath,"StripPartialDir","")
AssertStrNoOverlap(FullPath,PartialPath,PartialPath)

if (MacZip.DataForkOnly)
        {
        tmpPtr2 += strlen(tmpPtr1);
        return (char *)tmpPtr2;
        }

switch (MacZip.MacZipMode)
    {
    case JohnnyLee_EF:
        {
        tmpPtr2 += strlen(tmpPtr1);
        return (char *)tmpPtr2;
        break;
        }

    case NewZipMode_EF:
        {           /* determine Fork type */
        result = strncmp(FullPath, ResourceMark, sizeof(ResourceMark)-2);
        if (result != 0)
            {                               /* data fork  */
            MacZip.CurrentFork = DataFork;
            tmpPtr2 += strlen(tmpPtr1);
             return (char *)tmpPtr2;
            }
        else
            {                                /* resource fork  */
            MacZip.CurrentFork = ResourceFork;
            sstrcpy(CompletePath, ResourceMark);
            tmpPtr2 += strlen(tmpPtr1);
            tmpPtr2 += sizeof(ResourceMark);
            sstrcat(CompletePath, tmpPtr2);
            return (char *)CompletePath;
            }
        break;
        }
    }

 return NULL;    /* function should never reach this point */
}




/*
** Init all global variables
** Must be called for each zip-run
*/

void ZipInitAllVars(void)
{
getcwd(MacZip.CurrentPath, sizeof(MacZip.CurrentPath));
/* MacZip.MacZipMode = JohnnyLee_EF;     */
MacZip.MacZipMode = NewZipMode_EF;

MacZip.DataForkOnly = false;
MacZip.CurrentFork = NoFork;

MacZip.StoreFoldersAlso = false;

MacZip.FoundFiles = 0;
MacZip.FoundDirectories = 0;
MacZip.RawCountOfItems = 0;
MacZip.BytesOfData = 0;

MacZip.StoreFullPath = false;
MacZip.StatingProgress = false;
MacZip.IncludeInvisible = false;

MacZip.isMacStatValid = false;

MacZip.CurrTextEncodingBase = FontScript();

MacZip.HaveGMToffset = false;

createTime = TickCount();
estTicksToFinish = -1;
updateTicks = 0;

/* init some functions */
IsZipFile(NULL);

destroy(NULL);
deletedir(NULL);
ShowCounter(true);

extra_fields = 1;
error_level = 0;
count_of_Zippedfiles = 0;
}




/*
** Get the findercomment and store it as file-comment in the Zip-file
**
*/
char *GetComment(char *filename)
{
OSErr       err;
static char buffer[NAME_MAX];
char buffer2[NAME_MAX];
char *tmpPtr;

if (filename == NULL) return NULL;

    /* now we can convert Unix-Path in HFS-Path */
for (tmpPtr = filename; *tmpPtr; tmpPtr++)
    if (*tmpPtr == '/')
      *tmpPtr = ':';

if (MacZip.StoreFullPath)
    {  /* filename is already a fullpath */
    sstrcpy(buffer,filename);
    }
else
    {  /* make a fullpath */
    sstrcpy(buffer,MacZip.SearchDir);
    sstrcat(buffer,filename);
    }

/* make fullpath and get FSSpec */
/* Unfortunately: I get only the converted filename here */
/* so filenames with extended characters can not be found  */
GetCompletePath(buffer2,buffer, &MacZip.fileSpec, &err);
printerr("GetCompletePath:",(err != -43) && (err != -120) && (err != 0) ,
          err,__LINE__,__FILE__,buffer);

err = FSpDTGetComment(&MacZip.fileSpec, (unsigned char *) buffer);
printerr("FSpDTGetComment:", (err != -5012) && (err != 0), err,
         __LINE__, __FILE__, filename);
P2CStr((unsigned char *) buffer);
if (err == -5012) return NULL;  /* no finder-comments found */

if (noisy) printf("\n%32s -> %s",filename, buffer);
/*
Beside the script change we need only to change 0x0d in 0x0a
so the last two arguments are not needed and does nothing.
*/
MakeCompatibleString(buffer, 0x0d, 0x0a, ' ', ' ',
                     MacZip.CurrTextEncodingBase);

return buffer;
}




/*
** Print a progress indicator for stating the files
**
*/

void PrintStatProgress(char *msg)
{

if (!noisy) return;     /* do no output if noisy is false */

MacZip.StatingProgress = true;

if (strcmp(msg,"done") == 0)
    {
    MacZip.StatingProgress = false;
    printf("\n ... done \n\n");
    }
else printf("\n %s",msg);

}




void InformProgress(const long progressMax, const long progressSoFar )
{
int curr_percent;
char no_time[5] = "...";

curr_percent = percent(progressMax, progressSoFar);

if (curr_percent < 95)
    {
    estTicksToFinish = EstimateCompletionTime(progressMax,
                                                  progressSoFar, curr_percent);
    }
else
    {
    rightStatusString(no_time);
    leftStatusString(no_time);
    }

updateTicks = TickCount() + 60;
return;
}


void ShowCounter(Boolean reset)
{
static char statusline[100];
static unsigned long filecount = 0;

if (reset)
        {
        filecount = 0;
        return;
        }

if (noisy)
    {
    sprintf(statusline, "%6d", filecount++);
    rightStatusString(statusline);
    }
}


static long EstimateCompletionTime(const long progressMax,
                                const long progressSoFar,
                                unsigned char curr_percent)
{
    long  max = progressMax, value = progressSoFar;
    static char buf[100];
    unsigned long ticksTakenSoFar = TickCount() - createTime;
    float currentRate = (float) ticksTakenSoFar / (float) value;
    long  newEst = (long)( currentRate * (float)( max - value ));

    sprintf(buf, "%d [%d%%]",progressSoFar, curr_percent);
    rightStatusString(buf);

    estTicksToFinish = newEst;

    UpdateTimeToComplete();

return estTicksToFinish;
}





static void UpdateTimeToComplete(void)
{
    short       days, hours, minutes, seconds;
    char    estStr[255];
    Str15   xx, yy;
    short   idx = 0;

    if ( estTicksToFinish == -1 )
        return;

    days    =   estTicksToFinish / 5184000L;
    hours   = ( estTicksToFinish - ( days * 5184000L )) / 216000L;
    minutes = ( estTicksToFinish - ( days * 5184000L ) -
              ( hours * 216000L )) / 3600L;
    seconds = ( estTicksToFinish - ( days * 5184000L ) -
              ( hours * 216000L ) - ( minutes * 3600L )) / 60L;

        xx[0] = 0;
        yy[0] = 0;

        if ( days )
        {
            /* "more than 24 hours" */

            idx = 1;
            goto setEstTimeStr;
        }

        if ( hours >= 8 )
        {
            /* "more than x hours" */

            NumToString( hours, xx );
            idx = 2;
            goto setEstTimeStr;
        }

        if ( estTicksToFinish > 252000L  )      /* > 1hr, 10 minutes */
        {
            /* "about x hours, y minutes" */

            NumToString( hours, xx );
            NumToString( minutes, yy );
            idx = 3;
            goto setEstTimeStr;
        }

        if ( estTicksToFinish > 198000L )   /* > 55 minutes */
        {
            /* "about an hour" */
            idx = 4;
            goto setEstTimeStr;
        }

        if ( estTicksToFinish > 144000L )   /* > 40 minutes */
        {
            /* "less than an hour" */

            idx = 5;
            goto setEstTimeStr;
        }

        if ( estTicksToFinish > 4200L )     /* > 1 minute, 10 sec */
        {
            /* "about x minutes, y seconds */

            NumToString( minutes, xx );
            NumToString( seconds, yy );

            if ( minutes == 1 )
                idx = 11;
            else
                idx = 6;
            goto setEstTimeStr;
        }

        if ( estTicksToFinish > 3000L )     /* > 50 seconds */
        {
            /* "about a minute" */

            idx = 7;
            goto setEstTimeStr;
        }

        if ( estTicksToFinish > 1500L )     /* > 25 seconds */
        {
            /* "less than a minute" */

            idx = 8;
            goto setEstTimeStr;
        }

        if ( estTicksToFinish > 120L )      /* > 2 seconds */
        {
            NumToString( seconds, xx );
            idx = 9;
            goto setEstTimeStr;
        }

        idx = 10;

    setEstTimeStr:
        sprintf(estStr,Time_Est_strings[idx],P2CStr(xx),P2CStr(yy));
        leftStatusString((char *)estStr);
}





/*
** Just return the zip version
**
*/

char *GetZipVersionsInfo(void)
{
static char ZipVersion[100];

sprintf(ZipVersion, "Zip Module\n%d.%d%d%s of %s", Z_MAJORVER, Z_MINORVER,
        Z_PATCHLEVEL, Z_BETALEVEL, REVDATE);

return ZipVersion;
}




#ifndef USE_SIOUX

/*
** Just return the copyright message
**
*/

char *GetZipCopyright(void)
{
static char CopyR[300];

sstrcpy(CopyR, copyright[0]);
sstrcat(CopyR, copyright[1]);
sstrcat(CopyR, "\r\rPlease send bug reports to the authors at\r"\
              "Zip-Bugs@lists.wku.edu");

return CopyR;
}




/*
** Just return the compilers date/time
**
*/

char *GetZipVersionLocal(void)
{
static char ZipVersionLocal[50];

sprintf(ZipVersionLocal, "[%s %s]", __DATE__, __TIME__);

return ZipVersionLocal;
}

#endif  /*   #ifndef USE_SIOUX  */




