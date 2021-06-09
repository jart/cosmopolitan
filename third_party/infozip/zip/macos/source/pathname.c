/*
  Copyright (c) 1990-2003 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2000-Apr-09 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/*---------------------------------------------------------------------------

  pathname.c

  Function dealing with the pathname. Mostly C-string work.

  ---------------------------------------------------------------------------*/

/*****************************************************************************/
/*  Includes                                                                 */
/*****************************************************************************/

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sound.h>

#include "pathname.h"
#include "helpers.h"
#include "macstuff.h"


/*****************************************************************************/
/*  Global Vars                                                              */
/*****************************************************************************/

const char  ResourceMark[] = "XtraStuf.mac:";  /* see also macos.c */


#include "zip.h"




/*****************************************************************************/
/*  Functions                                                                */
/*****************************************************************************/


/*
 *----------------------------------------------------------------------
 *
 * FSpFindFolder --
 *
 *  This function is a version of the FindFolder function that
 *  returns the result as a FSSpec rather than a vRefNum and dirID.
 *
 * Results:
 *  Results will be simaler to that of the FindFolder function.
 *
 * Side effects:
 *  None.
 *
 *----------------------------------------------------------------------
 */

OSErr
FSpFindFolder(
    short vRefNum,      /* Volume reference number. */
    OSType folderType,      /* Folder type taken by FindFolder. */
    Boolean createFolder,   /* Should we create it if non-existant. */
    FSSpec *spec)       /* Pointer to resulting directory. */
{
    short foundVRefNum;
    long foundDirID;
    OSErr err;

    err = FindFolder(vRefNum, folderType, createFolder,
        &foundVRefNum, &foundDirID);
    if (err != noErr) {
    return err;
    }

    err = FSMakeFSSpecCompat(foundVRefNum, foundDirID, "\p", spec);
    return err;
}


/*
**  return volumename from pathname
**
*/

unsigned short GetVolumeFromPath(const char *FullPath, char *VolumeName)
{
const char *VolEnd, *tmpPtr1;
char *tmpPtr2 = VolumeName;

AssertStr(FullPath,"GetVolumeFromPath")

for (VolEnd = FullPath; *VolEnd != '\0' && *VolEnd != ':'; VolEnd++)
      ;
if (*VolEnd == '\0') return 0;

for (tmpPtr1 = FullPath; tmpPtr1 != VolEnd;)
    {
    *tmpPtr2++ = *tmpPtr1++;
    }

*tmpPtr2 = '\0';

return (unsigned short) strlen(VolumeName);
}



/***********************************/
/* Function FindNewExtractFolder() */
/***********************************/

char *FindNewExtractFolder(char *ExtractPath, Boolean uniqueFolder)
{
char buffer[NAME_MAX], *tmpPtr, *namePtr;
char *last_dotpos         = ExtractPath;
short count = 0, folderCount = 0;
OSErr err;
FSSpec Spec;
long theDirID;
Boolean isDirectory;
unsigned short namelen, pathlen = strlen(ExtractPath);
unsigned long ext_length  = 0;
unsigned long num_to_cut  = 0;
long firstpart_length = pathlen;

AssertStr(ExtractPath,"FindNewExtractFolder ExtractPath == NULL")

for (tmpPtr = ExtractPath; *tmpPtr; tmpPtr++)
    if (*tmpPtr == ':')
        {
        folderCount++;
        namePtr = tmpPtr;
        }

if (folderCount > 1) {
    namelen = strlen(namePtr);
} else {
    namelen = strlen(ExtractPath);
}

if (uniqueFolder) {
    for (count = 0; count < 99; count++)
        {
        memset(buffer,0,sizeof(buffer));

        if (namelen >= 28)
            ExtractPath[pathlen-2] = 0x0;
        else
            ExtractPath[pathlen-1] = 0x0;

        sprintf(buffer,"%s%d",ExtractPath,count);
        GetCompletePath(ExtractPath, buffer, &Spec,&err);
        err = FSpGetDirectoryID(&Spec, &theDirID, &isDirectory);
        if (err == -43) break;
        }
} else {
    /* Look for the last extension pos */
    for (tmpPtr = ExtractPath; *tmpPtr; tmpPtr++)
        if (*tmpPtr == '.') last_dotpos = tmpPtr;

    ext_length = strlen(last_dotpos);

    if (ext_length < 6) {  /* up to 5 chars are treated as a */
                           /* normal extension like ".html" or ".class"  */
        int nameLength = last_dotpos - ExtractPath;
        if (nameLength > 1) {
            ExtractPath[nameLength] = 0x0;
        } else {
            ExtractPath[pathlen-1] = 0x0;
        }
    } else {
        ExtractPath[pathlen-1] = 0x0;
    }

    GetCompletePath(ExtractPath, ExtractPath, &Spec,&err);
}

/* Foldernames must always end with a colon  */
sstrcat(ExtractPath,":");
return ExtractPath;
}



/*
**  creates an archive file name
**
*/

void createArchiveName(char *thePath)
{
char *tmpPtr, *namePtr;
short folderCount = 0;
unsigned short namelen, pathlen = strlen(thePath);

if (thePath[pathlen-1] == ':') thePath[pathlen-1] = 0x0;

for (tmpPtr = thePath; *tmpPtr; tmpPtr++)
    if (*tmpPtr == ':')
        {
        folderCount++;
        namePtr = tmpPtr;
        }

namelen = strlen(namePtr);

    /* we have to eliminate illegal chars:
     * The name space for Mac filenames and Zip filenames (unix style names)
     * do both include all printable extended-ASCII characters.  The only
     * difference we have to take care of is the single special character
     * used as path delimiter:
     * ':' on MacOS and '/' on Unix and '\\' on Dos.
     * So, to convert between Mac filenames and Unix filenames without any
     * loss of information, we simply interchange ':' and '/'.  Additionally,
     * we try to convert the coding of the extended-ASCII characters into
     * InfoZip's standard ISO 8859-1 codepage table.
     */
  MakeCompatibleString(namePtr, '/', '_', '.', '-', -1);

 /* Avoid filenames like: "Archive..zip"  */
if (thePath[pathlen-1] == '.')
    {
    thePath[pathlen-1] = 0;
    }

if (folderCount >= 1)
    { /* path contains at least one folder */

    if (namelen >= 28)
        {
        pathlen = pathlen-4;
        }

    thePath[pathlen]   = '.';
    thePath[pathlen+1] = 'z';
    thePath[pathlen+2] = 'i';
    thePath[pathlen+3] = 'p';
    thePath[pathlen+4] = 0x0;
    return;
    }
else
    {  /* path contains no folder */
    FindDesktopFolder(thePath);
    createArchiveName(thePath);
    }
}



/*
** finds the desktop-folder on a volume with
** largest amount of free-space.
*/

void FindDesktopFolder(char *Path)
{
char buffer[255];
FSSpec  volumes[50];        /* 50 Volumes should be enough */
short   actVolCount, volIndex = 1, VolCount = 0;
OSErr   err;
short     i, foundVRefNum;
FSSpec spec;
UInt64 freeBytes;
UInt64 totalBytes;
UInt64 MaxFreeBytes;

err = OnLine(volumes, 50, &actVolCount, &volIndex);
printerr("OnLine:", (err != -35) && (err != 0), err, __LINE__, __FILE__, "");

MaxFreeBytes = 0;

for (i=0; i < actVolCount; i++)
    {
    XGetVInfo(volumes[i].vRefNum,
              volumes[i].name,
              &volumes[i].vRefNum,
              &freeBytes,
              &totalBytes);

    if (MaxFreeBytes < freeBytes) {
        MaxFreeBytes = freeBytes;
        foundVRefNum = volumes[i].vRefNum;
    }

    if ((freeBytes == 0) && (MaxFreeBytes < freeBytes)) {
        MaxFreeBytes = freeBytes;
        foundVRefNum = volumes[i].vRefNum;
    }

}

 FSpFindFolder(foundVRefNum, kDesktopFolderType,
            kDontCreateFolder,&spec);

 GetFullPathFromSpec(buffer, &spec , &err);
 sstrcat(buffer,Path);
 sstrcpy(Path,buffer);
}


/*
**  return the path without the filename
**
*/

char *TruncFilename(char *DirPath, const char *FilePath)
{
char *tmpPtr;
char *dirPtr = NULL;

AssertStr(DirPath,"TruncFilename")
Assert_it(Spec,"TruncFilename","")

sstrcpy(DirPath, FilePath);

for (tmpPtr = DirPath; *tmpPtr; tmpPtr++)
    if (*tmpPtr == ':')
        dirPtr = tmpPtr;

if (dirPtr)
    *++dirPtr = '\0';
else
    printerr("TruncFilename: FilePath has no Folders", -1,
         -1, __LINE__, __FILE__, FilePath);

return DirPath;
}



/*
**  return only filename
**
*/

char *GetFilename(char *FileName, const char *FilePath)
{
const char *tmpPtr;
const char *dirPtr = NULL;

Assert_it(FileName,"GetFilename","")
Assert_it(FilePath,"GetFilename","")

for (tmpPtr = FilePath; *tmpPtr; tmpPtr++)
    {
    if (*tmpPtr == ':')
        {
        dirPtr = tmpPtr;
        }
    }

if (dirPtr)
    {
    ++dirPtr;  /* jump over the ':' */
    }
else
    {
    return strcpy(FileName, FilePath); /* FilePath has no Folders */
    }

return strcpy(FileName, dirPtr);
}



/*
**  return fullpathname from folder/dir-id
**
*/

char *GetFullPathFromID(char *CompletePath, short vRefNum, long dirID,
                        ConstStr255Param name, OSErr *err)
{
FSSpec      spec;

    *err = FSMakeFSSpecCompat(vRefNum, dirID, name, &spec);
    printerr("FSMakeFSSpecCompat:", (*err != -43) && (*err != 0), *err,
             __LINE__, __FILE__, "");
    if ( (*err == noErr) || (*err == fnfErr) )
        {
        return GetFullPathFromSpec(CompletePath, &spec, err);
        }

return NULL;
}



/*
**  convert real-filename to archive-filename
**
*/

char *Real2RfDfFilen(char *RfDfFilen, const char *RealPath,
                    short CurrentFork, short MacZipMode, Boolean DataForkOnly)
{

AssertStr(RealPath,"Real2RfDfFilen")
AssertStr(RfDfFilen,"Real2RfDfFilen")

if (DataForkOnly) /* make no changes */
    {
    return sstrcpy(RfDfFilen, RealPath);
    }

switch (MacZipMode)
    {
    case JohnnyLee_EF:
        {
        sstrcpy(RfDfFilen, RealPath);
        if (CurrentFork == DataFork)            /* data-fork  */
            return sstrcat(RfDfFilen, "d");
        if (CurrentFork == ResourceFork)        /* resource-fork */
            return sstrcat(RfDfFilen, "r");
        break;
        }

    case NewZipMode_EF:
        {
        switch (CurrentFork)
            {
            case DataFork:
                {
                sstrcpy(RfDfFilen, RealPath);
                return RfDfFilen;  /* data-fork  */
                break;
                }
            case ResourceFork:
                {
                sstrcpy(RfDfFilen, ResourceMark);
                sstrcat(RfDfFilen, RealPath);  /* resource-fork */
                return RfDfFilen;
                break;
                }
            default:
                {
                printerr("Real2RfDfFilen:", -1, -1,
                         __LINE__, __FILE__, RealPath);
                return NULL;  /* function should never reach this point */
                }
            }
        break;
        }
    default:
        {
        printerr("Real2RfDfFilen:", -1, -1, __LINE__, __FILE__, RealPath);
        return NULL;  /* function should never reach this point */
        }
    }

printerr("Real2RfDfFilen:", -1, -1, __LINE__, __FILE__, RealPath);
return NULL;  /* function should never come reach this point */
}



/*
**  convert archive-filename into a real filename
**
*/

char *RfDfFilen2Real(char *RealFn, const char *RfDfFilen, short MacZipMode,
                     Boolean DataForkOnly, short *CurrentFork)
{
short   length;
int     result;

AssertStr(RfDfFilen,"RfDfFilen2Real")

if (DataForkOnly ||
    (MacZipMode == UnKnown_EF) ||
    (MacZipMode < JohnnyLee_EF))
    {
    *CurrentFork = DataFork;
    return sstrcpy(RealFn,RfDfFilen);
    }

result = strncmp(RfDfFilen, ResourceMark, sizeof(ResourceMark)-2);
if (result == 0)
    {
    MacZipMode = NewZipMode_EF;
    }

switch (MacZipMode)
    {
    case JohnnyLee_EF:
        {
        sstrcpy(RealFn, RfDfFilen);
        length = strlen(RealFn);       /* determine Fork type */
        if (RealFn[length-1] == 'd') *CurrentFork = DataFork;
        else *CurrentFork = ResourceFork;
        RealFn[length-1] = '\0';       /* simply cut one char  */
        return RealFn;
        break;
        }

    case NewZipMode_EF:
        {                                   /* determine Fork type */
        result = strncmp(RfDfFilen, ResourceMark, sizeof(ResourceMark)-2);
        if (result != 0)
            {
            *CurrentFork = DataFork;
            sstrcpy(RealFn, RfDfFilen);
            return RealFn;  /* data-fork  */
            }
        else
            {
            *CurrentFork = ResourceFork;
            if (strlen(RfDfFilen) > (sizeof(ResourceMark) - 1))
                {
                sstrcpy(RealFn, &RfDfFilen[sizeof(ResourceMark)-1]);
                }
            else RealFn[0] = '\0';
            return RealFn;  /* resource-fork */
            }
        break;
        }
    default:
        {
        *CurrentFork = NoFork;
        printerr("RfDfFilen2Real():", -1, MacZipMode,
                 __LINE__, __FILE__, RfDfFilen);
        return NULL;  /* function should never reach this point */
        }
    }

printerr("RfDfFilen2Real():", -1, MacZipMode, __LINE__, __FILE__, RfDfFilen);
return NULL;  /* function should never reach this point */
}



/*
**  return the applications name (argv[0])
**
*/

char *GetAppName(void)
{
ProcessSerialNumber psn;
static Str255       AppName;
ProcessInfoRec      pinfo;
OSErr               err;

GetCurrentProcess(&psn);
pinfo.processName = AppName;
pinfo.processInfoLength = sizeof(pinfo);
pinfo.processAppSpec = NULL;

err = GetProcessInformation(&psn,&pinfo);
AppName[AppName[0]+1] = 0x00;

return (char *)&AppName[1];
}



/*
**  return fullpathname from FSSpec
**
*/

char *GetFullPathFromSpec(char *FullPath, FSSpec *Spec, OSErr *err)
{
Handle hFullPath;
short len;

Assert_it(Spec,"GetFullPathFromSpec","")

*err = FSpGetFullPath(Spec, &len, &hFullPath);
printerr("FSpGetFullPath:", (*err != -43) && (*err != 0), *err,
         __LINE__, __FILE__, "");

memmove(FullPath, (Handle) *hFullPath, len);
FullPath[len] = '\0';  /* make c-string */

DisposeHandle((Handle)hFullPath);   /* we don't need it any more */

printerr("Warning path length exceeds limit: ", len >= NAME_MAX, len,
         __LINE__, __FILE__, " chars ");

return FullPath;
}




/*
* This function expands a given partial path to a complete path.
* Path expansions are relative to the running app.
* This function follows the notation:
*   1. relative path:
*       a: ":subfolder:filename"    -> ":current folder:subfolder:filename"
*       b: "::folder2:filename"     -> folder2 is beside the current
*                                      folder on the same level
*       c: "filename"               -> in current folder
*
* An absolute path will be returned.

The following characteristics of Macintosh pathnames should be noted:

       A full pathname never begins with a colon, but must contain at
       least one colon.
       A partial pathname always begins with a colon separator except in
       the case where the file partial pathname is a simple file or
       directory name.
       Single trailing separator colons in full or partial pathnames are
       ignored except in the case of full pathnames to volumes.
       In full pathnames to volumes, the trailing separator colon is required.
       Consecutive separator colons can be used to ascend a level from a
       directory to its parent directory. Two consecutive separator colons
       will ascend one level, three consecutive separator colons will ascend
       two levels, and so on. Ascending can only occur from a directory;
       not a file.
*/

char *GetCompletePath(char *CompletePath, const char *name, FSSpec *Spec,
                      OSErr *err)
{
Boolean hasDirName = false;
char currentdir[NAME_MAX];
char *tmpPtr;
unsigned short pathlen;

AssertStr(name,"GetCompletePath")
Assert_it(Spec,"GetCompletePath","")
Assert_it((CompletePath != name),"GetCompletePath","")

for (tmpPtr = name; *tmpPtr; tmpPtr++)
    if (*tmpPtr == ':') hasDirName = true;

if (name[0] != ':')   /* case c: path including volume name or only filename */
    {
    if (hasDirName)
        {   /* okey, starts with volume name, so it must be a complete path */
        sstrcpy(CompletePath, name);
        }
    else
        {   /* only filename: add cwd and return */
        getcwd(currentdir, NAME_MAX);
        sstrcat(currentdir, name);
        sstrcpy(CompletePath, currentdir);
        }
    }
else if (name[1] == ':')    /* it's case b: "::folder2:filename"  */
    {
    printerr("GetCompletePath ", -1, *err, __LINE__, __FILE__, "not implemented");
            /* it's not yet implemented; do we really need this case ?*/
    return NULL;
    }
else                        /* it's case a: ":subfolder:filename" */
    {
    getcwd(CompletePath, NAME_MAX);     /* we don't need a second colon */
    CompletePath[strlen(CompletePath)-1] = '\0';
    sstrcat(CompletePath, name);
    }

pathlen = strlen(CompletePath);
*err = FSpLocationFromFullPath(pathlen, CompletePath, Spec);

return CompletePath;
}



char *MakeFilenameShorter(const char *LongFilename)
{
static char filename[35];  /* contents should be never longer than 32 chars */
static unsigned char Num = 0; /* change the number for every call */
                              /* this var will rollover without a problem */
char tempLongFilename[1024], charnum[5];
char *last_dotpos         = tempLongFilename;
unsigned long full_length = strlen(LongFilename);
unsigned long ext_length  = 0;
unsigned long num_to_cut  = 0;
long firstpart_length;
char *tmpPtr;
short MaxLength = 31;

if (full_length <= MaxLength) /* filename is not long */
    {
    return strcpy(filename,LongFilename);
    }

Num++;
strcpy(tempLongFilename,LongFilename);

/* Look for the last extension pos */
for (tmpPtr = tempLongFilename; *tmpPtr; tmpPtr++)
    if (*tmpPtr == '.') last_dotpos = tmpPtr;

ext_length = strlen(last_dotpos);
firstpart_length = last_dotpos - tempLongFilename;

if (ext_length > 6)  /* up to 5 chars are treated as a */
    {                /* normal extension like ".html" or ".class"  */
    firstpart_length = 0;
    }

num_to_cut = full_length - MaxLength;

/* number the files to make the names unique */
sprintf(charnum,"~%x", Num);
num_to_cut += strlen(charnum);

if (firstpart_length == 0)
    {
    firstpart_length = full_length;
    tempLongFilename[firstpart_length - num_to_cut] = 0;
    sprintf(filename,"%s%s", tempLongFilename, charnum);
    }
else
    {
    tempLongFilename[firstpart_length - num_to_cut] = 0;
    sprintf(filename,"%s%s%s", tempLongFilename, charnum, last_dotpos);
    }

return filename;
}
