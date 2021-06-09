/*
  Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 1999-Oct-05 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, both of these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html
*/
/*** macopen.c; stuff only required for the Mac port ***/

#include "zip.h"

#include <string.h>
#include <fcntl.h>
#include <unix.h>
#include <sound.h>

#include "helpers.h"
#include "pathname.h"
#include "macopen.h"
#include "macstuff.h"

#ifdef MACZIP
#include "macglob.h"

extern char *zipfile;   /* filename of the Zipfile */
extern char *tempzip;   /* Temporary zip file name */

extern MacZipGlobals    MacZip;


/* don't include "osdep.h" otherwise we will trap into endless loop */
#undef open
#undef fopen



FILE *MacFopen(const char *path, const char *mode)
{
static char TruncPath[NAME_MAX];
OSErr   err = 0;

AssertStr(path,path)

            /* open zipfile or tempzip */
if (strcmp(zipfile,path) == 0)
    {
    GetCompletePath(MacZip.ZipFullPath,path,&MacZip.ZipFileSpec,&err);
    err = PrintUserHFSerr((err != -43) && (err != 0), err, path);
    printerr("GetCompletePath:",err,err,__LINE__,__FILE__,path);
    if (CheckMountedVolumes(MacZip.ZipFullPath) > 1)
        DoWarnUserDupVol(MacZip.ZipFullPath);

    /* tempfile should appear in the same directory of the zipfile
       -> save path of zipfile */
    TruncFilename(TruncPath, MacZip.ZipFullPath);
    return fopen(MacZip.ZipFullPath, mode);
    }

if (strcmp(tempzip,path) == 0)
    {  /* add path of zipfile */
    sstrcat(TruncPath,tempzip);
    GetCompletePath(MacZip.TempZipFullPath,TruncPath,&MacZip.TempZipFileSpec,&err);
    err = PrintUserHFSerr((err != -43) && (err != 0), err, path);
    printerr("GetCompletePath:",err,err,__LINE__,__FILE__,path);

    return fopen(MacZip.TempZipFullPath, mode);
    }

printerr("MacFopen:",err,err,__LINE__,__FILE__,path);
return NULL;
}




int MacOpen(const char *path,int oflag, ...)
{
char RealFname[NAME_MAX];

AssertStr(path,path)

RfDfFilen2Real(RealFname,path, MacZip.MacZipMode, MacZip.DataForkOnly, &MacZip.CurrentFork);
/* convert to real fname and init global var  MacZip.CurrentFork  !!  */

switch (MacZip.CurrentFork)
    {
    case DataFork:
        {
        return my_open(RealFname, oflag);
        break;
        }
    case ResourceFork:
        {
        return my_open( RealFname, oflag | O_RSRC);
        break;
        }
    default:  /* for now (Zip ver 2.3b) MacOpen should never reach this point */
        {     /* however, this may change in the future ... */
        printerr("open: no resource / datafork ",-1,-1,__LINE__,__FILE__,path);
        return -1;
        }
    }
}


#ifdef muell
 /* file to delete */
int destroy(char *path)
{
static char lastpath[NAME_MAX];
char    currpath[NAME_MAX];
static Boolean FirstCall = true;
long rc;

AssertStr(path,path)

RfDfFilen2Real(currpath, path, MacZip.MacZipMode, MacZip.DataForkOnly, &MacZip.CurrentFork);

if (FirstCall == true)
    {
    FirstCall = false;
    rc = remove(currpath);
    }
else if (strcmp(currpath,lastpath) == 0) return 0; /* ignore, file is already deleted */
        else rc = remove(currpath); /* we are removeing all the files only by their
        pathname this is dangerous on a mac but there is no other way without
         a complete rewrite of the port  */

strcpy(lastpath,currpath);

return rc;
}
#endif




/* this function replaces the function "replace()" defined in fileio.c */
int replace(char *new_f, char *temp_f)  /* destination and source file names */
{
OSErr   err = 0;
char    newfname[NAME_MAX];

AssertStr(new_f,new_f)
AssertStr(temp_f,temp_f)

UserStop();

GetFilename(newfname, new_f);

/* check zipfile name and tempfile name */
/* we are using this function only for replacing the tempfile with the zipfile */
if ((strcmp(zipfile,new_f) == 0) || (strcmp(tempzip,temp_f) == 0))
    {
    remove(MacZip.ZipFullPath);

    /* rename the temp file to the zip file */
    err = rename(MacZip.TempZipFullPath,MacZip.ZipFullPath);
    printerr("rename:",err,err,__LINE__,__FILE__,MacZip.TempZipFullPath);
if (err != 0) return ZE_CREAT;
    else return ZE_OK;
    }
else return ZE_CREAT;
}



 /* file to delete */
 /* we are removeing all the files only by their
    pathname this is dangerous on a mac but there is no
    other way without a complete rewrite of the port  */

int destroy(char *path)
{
static char lastpath[NAME_MAX];
static FSSpec  trashfolder;
static Boolean FirstCall = true;
static char Num = 0;
static Boolean  Immediate_File_Deletion = false;
char    currpath[NAME_MAX], *envptr;
FSSpec  fileToDelete;
OSErr   err;

/* init this function */
if ((path == NULL) ||
    (strlen(path) == 0))
    {
    FirstCall = true;
    Num = 0;
    return -1;
    }

UserStop();

RfDfFilen2Real(currpath, path, MacZip.MacZipMode,
                MacZip.DataForkOnly, &MacZip.CurrentFork);
GetCompletePath(currpath,currpath,&fileToDelete, &err);

if (FirstCall == true)
    {
    FirstCall = false;
    sstrcpy(lastpath,currpath);
    err = FSpFindFolder(fileToDelete.vRefNum, kTrashFolderType,
                      kDontCreateFolder,&trashfolder);
    printerr("FSpFindFolder:",err,err,__LINE__,__FILE__,path);

    envptr = getenv("Immediate_File_Deletion");
    if (!(envptr == (char *)NULL || *envptr == '\0'))
        {
        if (stricmp(envptr,"yes") == 0)
            Immediate_File_Deletion = true;
        else
            Immediate_File_Deletion = false;
        }

    if (Immediate_File_Deletion)
        {
        err = FSpDelete(&fileToDelete);
        return err;
        }

    err = CatMove (fileToDelete.vRefNum, fileToDelete.parID,
                   fileToDelete.name, trashfolder.parID, trashfolder.name);
    return err;
    }

if (strcmp(currpath,lastpath) == 0)
    {
    return 0; /* ignore, file is already deleted */
    }
else
    {

    if (Immediate_File_Deletion)
        {
        err = FSpDelete(&fileToDelete);
        sstrcpy(lastpath,path);
        return err;
        }

    err = CatMove (fileToDelete.vRefNum, fileToDelete.parID,
                   fileToDelete.name, trashfolder.parID, trashfolder.name);

    /* -48 = file is already existing so we have to rename it before
       moving the file */
    if (err == -48)
        {
        Num++;
        if (fileToDelete.name[0] >= 28) /* cut filename if to long */
            fileToDelete.name[0] = 28;
        P2CStr(fileToDelete.name);
        sprintf(currpath,"%s~%d",(char *)fileToDelete.name,Num);
        C2PStr(currpath);
        C2PStr((char *)fileToDelete.name);
        err = HRename (fileToDelete.vRefNum, fileToDelete.parID,
                       fileToDelete.name, (unsigned char *) currpath);
        err = CatMove (fileToDelete.vRefNum, fileToDelete.parID,
                       (unsigned char *) currpath, trashfolder.parID,
                       trashfolder.name);
        }
    }

sstrcpy(lastpath,currpath);
return err;
}



#endif  /* #ifdef MACZIP */




/*
 *  int open(const char *path, int oflag)
 *
 *      Opens a file stream.
 */
int my_open(char *path, int oflag)
{
    FSSpec          spec;
    char            permission;
    HParamBlockRec  hpb;
    OSErr           err, errno;
    Boolean targetIsFolder, wasAliased;

    AssertStr(path,path)

    /* Setup permission */
    if ((oflag & 0x03) == O_RDWR)
        permission = fsRdWrPerm;
    else
        permission = (oflag & O_RDONLY) ? fsRdPerm : 0 + (oflag & O_WRONLY) ? fsWrPerm : 0;

        FSpLocationFromFullPath(strlen(path),path, &spec);
        if ((oflag & (O_ALIAS | O_NRESOLVE)) == 0)
            ResolveAliasFile(&spec, true, &targetIsFolder, &wasAliased);
        hpb.fileParam.ioNamePtr = spec.name;
        hpb.fileParam.ioVRefNum = spec.vRefNum;
        hpb.fileParam.ioDirID = spec.parID;
        hpb.ioParam.ioPermssn = permission;

        if (oflag & O_RSRC)         /* open the resource fork of the file */
            err = PBHOpenRFSync(&hpb);
        else                        /* open the data fork of the file */
            err = PBHOpenDFSync(&hpb);

    if ((err == fnfErr) && (oflag & O_CREAT)) {
        hpb.fileParam.ioFlVersNum = 0;
        err = PBHCreateSync(&hpb);
        if (err == noErr) {
            /* Set the finder info */
            unsigned long secs;
            unsigned long isbinary = oflag & O_BINARY;

            hpb.fileParam.ioFlFndrInfo.fdType = '\?\?\?\?';
            hpb.fileParam.ioFlFndrInfo.fdCreator = '\?\?\?\?';
            hpb.fileParam.ioFlFndrInfo.fdFlags = 0;
            if (oflag & O_ALIAS)        /* set the alias bit */
                hpb.fileParam.ioFlFndrInfo.fdFlags = kIsAlias;
            else                                        /* clear all flags */
                hpb.fileParam.ioFlFndrInfo.fdFlags = 0;

            GetDateTime(&secs);
            hpb.fileParam.ioFlCrDat = hpb.fileParam.ioFlMdDat = secs;
            PBHSetFInfoSync(&hpb);
        }

        if (err && (err != dupFNErr)) {
            errno = err; return -1;
        }

            if (oflag & O_RSRC)         /* open the resource fork of the file */
                err = PBHOpenRFSync(&hpb);
            else                        /* open the data fork of the file */
                err = PBHOpenDFSync(&hpb);
    }

    if (err && (err != dupFNErr) && (err != opWrErr)) {
        errno = err; return -1;
    }

    if (oflag & O_TRUNC) {
        IOParam pb;

        pb.ioRefNum = hpb.ioParam.ioRefNum;
        pb.ioMisc = 0L;
        err = PBSetEOFSync((ParmBlkPtr)&pb);
        if (err != noErr) {
            errno = err; return -1;
        }
    }

    if (oflag & O_APPEND) lseek(hpb.ioParam.ioRefNum,0,SEEK_END);

    return (hpb.ioParam.ioRefNum);
}





