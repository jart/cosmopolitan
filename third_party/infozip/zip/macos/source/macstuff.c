/*
These Functions were originally part of More Files version 1.4.8

More Files fixes many of the broken or underfunctional
parts of the file system.

More Files

A collection of File Manager and related routines

by Jim Luther (Apple Macintosh Developer Technical Support Emeritus)
with significant code contributions by Nitin Ganatra
(Apple Macintosh Developer Technical Support Emeritus)
Copyright  1992-1998 Apple Computer, Inc.
Portions copyright  1995 Jim Luther
All rights reserved.

The Package "More Files" is distributed under the following
license terms:

         "You may incorporate this sample code into your
          applications without restriction, though the
          sample code has been provided "AS IS" and the
          responsibility for its operation is 100% yours.
          However, what you are not permitted to do is to
          redistribute the source as "DSC Sample Code" after
          having made changes. If you're going to
          redistribute the source, we require that you make
          it clear in the source that the code was descended
          from Apple Sample Code, but that you've made
          changes."


The following changes are made by Info-ZIP:

- The only changes are made by pasting the functions
  (mostly found in MoreFilesExtras.c / MoreFiles.c)
  directly into macstuff.c / macstuff.h and slightly
  reformatting the text (replacement of TABs by spaces,
  removal/replacement of non-ASCII characters).
  The code itself is NOT changed.

This file has been modified by Info-ZIP for use in MacZip.
This file is NOT part of the original package More Files.

More Files can be found on the MetroWerks CD and Developer CD from
Apple. You can also download the latest version from:

    http://members.aol.com/JumpLong/#MoreFiles

Jim Luther's Home-page:
    http://members.aol.com/JumpLong/


*/

#include <string.h>


#include "macstuff.h"



extern int errno;

static  OSErr   GetCommentFromDesktopFile(short vRefNum,
                                          long dirID,
                                          ConstStr255Param name,
                                          Str255 comment);

static  OSErr   GetCommentID(short vRefNum,
                             long dirID,
                             ConstStr255Param name,
                             short *commentID);

static  OSErr   GetDesktopFileName(short vRefNum,
                                   Str255 desktopName);


enum
{
    kBNDLResType    = 'BNDL',
    kFREFResType    = 'FREF',
    kIconFamResType = 'ICN#',
    kFCMTResType    = 'FCMT',
    kAPPLResType    = 'APPL'
};


/*****************************************************************************/

/*
**  File Manager FSp calls
*/

/*****************************************************************************/

pascal  OSErr   FSMakeFSSpecCompat(short vRefNum,
                                   long dirID,
                                   ConstStr255Param fileName,
                                   FSSpec *spec)
{
    OSErr   result;

#if !__MACOSSEVENORLATER
    if ( !FSHasFSSpecCalls() && !QTHasFSSpecCalls() )
    {
        Boolean isDirectory;

        result = GetObjectLocation(vRefNum, dirID, fileName,
                                    &(spec->vRefNum), &(spec->parID), spec->name,
                                    &isDirectory);
    }
    else
#endif  /* !__MACOSSEVENORLATER */
    {
     /* Let the file system create the FSSpec if it can since it does the job */
     /* much more efficiently than I can. */
        result = FSMakeFSSpec(vRefNum, dirID, fileName, spec);

        /* Fix a bug in Macintosh PC Exchange's MakeFSSpec code where 0 is */
        /* returned in the parID field when making an FSSpec to the volume's */
        /* root directory by passing a full pathname in MakeFSSpec's */
        /* fileName parameter. Fixed in Mac OS 8.1 */
        if ( (result == noErr) && (spec->parID == 0) )
            spec->parID = fsRtParID;
    }
    return ( result );
}


/*****************************************************************************/
/* FSHasFSSpecCalls returns true if the file system provides FSSpec calls. */

#if !__MACOSSEVENORLATER
static  Boolean FSHasFSSpecCalls(void)
{
    long            response;
#if !GENERATENODATA
    static Boolean  tested = false;
    static Boolean  result = false;
#else
    Boolean result = false;
#endif

#if !GENERATENODATA
    if ( !tested )
    {
        tested = true;
#endif
        if ( Gestalt(gestaltFSAttr, &response) == noErr )
        {
            result = ((response & (1L << gestaltHasFSSpecCalls)) != 0);
        }
#if !GENERATENODATA
    }
#endif
    return ( result );
}
#endif  /* !__MACOSSEVENORLATER */



/*****************************************************************************/
/* QTHasFSSpecCalls returns true if QuickTime provides FSSpec calls */
/* except for FSpExchangeFiles. */

#if !__MACOSSEVENORLATER
static  Boolean QTHasFSSpecCalls(void)
{
    long            response;
#if !GENERATENODATA
    static Boolean  tested = false;
    static Boolean  result = false;
#else
    Boolean result = false;
#endif

#if !GENERATENODATA
    if ( !tested )
    {
        tested = true;
#endif
        result = (Gestalt(gestaltQuickTimeVersion, &response) == noErr);
#if !GENERATENODATA
    }
#endif
    return ( result );
}
#endif  /* !__MACOSSEVENORLATER */




/*
 *----------------------------------------------------------------------
 *
 * FSpGetDefaultDir --
 *
 *  This function gets the current default directory.
 *
 * Results:
 *  The provided FSSpec is changed to point to the "default"
 *  directory.  The function returns what ever errors
 *  FSMakeFSSpecCompat may encounter.
 *
 * Side effects:
 *  None.
 *
 *----------------------------------------------------------------------
 */

int FSpGetDefaultDir(FSSpecPtr dirSpec) /* On return the default directory. */
{
    OSErr err;
    short vRefNum = 0;
    long int dirID = 0;

    err = HGetVol(NULL, &vRefNum, &dirID);

    if (err == noErr) {
    err = FSMakeFSSpecCompat(vRefNum, dirID, (ConstStr255Param) NULL,
        dirSpec);
    }

    return err;
}

/*
 *----------------------------------------------------------------------
 *
 * FSpSetDefaultDir --
 *
 *  This function sets the default directory to the directory
 *  pointed to by the provided FSSpec.
 *
 * Results:
 *  The function returns what ever errors HSetVol may encounter.
 *
 * Side effects:
 *  None.
 *
 *----------------------------------------------------------------------
 */

int FSpSetDefaultDir(FSSpecPtr dirSpec) /* The new default directory. */
{
    OSErr err;

    /*
     * The following special case is needed to work around a bug
     * in the Macintosh OS.  (Acutally PC Exchange.)
     */

    if (dirSpec->parID == fsRtParID) {
    err = HSetVol(NULL, dirSpec->vRefNum, fsRtDirID);
    } else {
    err = HSetVol(dirSpec->name, dirSpec->vRefNum, dirSpec->parID);
    }

    return err;
}

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
 *----------------------------------------------------------------------
 *
 * FSpPathFromLocation --
 *
 *  This function obtains a full path name for a given macintosh
 *  FSSpec.  Unlike the More Files function FSpGetFullPath, this
 *  function will return a C string in the Handle.  It also will
 *  create paths for FSSpec that do not yet exist.
 *
 * Results:
 *  OSErr code.
 *
 * Side effects:
 *  None.
 *
 *----------------------------------------------------------------------
 */

OSErr
FSpPathFromLocation(
    FSSpec *spec,       /* The location we want a path for. */
    int *length,        /* Length of the resulting path. */
    Handle *fullPath)       /* Handle to path. */
{
    OSErr err;
    FSSpec tempSpec;
    CInfoPBRec pb;

    *fullPath = NULL;

    /*
     * Make a copy of the input FSSpec that can be modified.
     */
    BlockMoveData(spec, &tempSpec, sizeof(FSSpec));

    if (tempSpec.parID == fsRtParID) {
    /*
     * The object is a volume.  Add a colon to make it a full
     * pathname.  Allocate a handle for it and we are done.
     */
    tempSpec.name[0] += 2;
    tempSpec.name[tempSpec.name[0] - 1] = ':';
    tempSpec.name[tempSpec.name[0]] = '\0';

    err = PtrToHand(&tempSpec.name[1], fullPath, tempSpec.name[0]);
    } else {
    /*
     * The object isn't a volume.  Is the object a file or a directory?
     */
    pb.dirInfo.ioNamePtr = tempSpec.name;
    pb.dirInfo.ioVRefNum = tempSpec.vRefNum;
    pb.dirInfo.ioDrDirID = tempSpec.parID;
    pb.dirInfo.ioFDirIndex = 0;
    err = PBGetCatInfoSync(&pb);

    if ((err == noErr) || (err == fnfErr)) {
        /*
         * If the file doesn't currently exist we start over.  If the
         * directory exists everything will work just fine.  Otherwise we
         * will just fail later.  If the object is a directory, append a
         * colon so full pathname ends with colon.
         */
        if (err == fnfErr) {
        BlockMoveData(spec, &tempSpec, sizeof(FSSpec));
        } else if ( (pb.hFileInfo.ioFlAttrib & ioDirMask) != 0 ) {
        tempSpec.name[0] += 1;
        tempSpec.name[tempSpec.name[0]] = ':';
        }

        /*
         * Create a new Handle for the object - make it a C string.
         */
        tempSpec.name[0] += 1;
        tempSpec.name[tempSpec.name[0]] = '\0';
        err = PtrToHand(&tempSpec.name[1], fullPath, tempSpec.name[0]);
        if (err == noErr) {
        /*
         * Get the ancestor directory names - loop until we have an
         * error or find the root directory.
         */
        pb.dirInfo.ioNamePtr = tempSpec.name;
        pb.dirInfo.ioVRefNum = tempSpec.vRefNum;
        pb.dirInfo.ioDrParID = tempSpec.parID;
        do {
            pb.dirInfo.ioFDirIndex = -1;
            pb.dirInfo.ioDrDirID = pb.dirInfo.ioDrParID;
            err = PBGetCatInfoSync(&pb);
            if (err == noErr) {
            /*
             * Append colon to directory name and add
             * directory name to beginning of fullPath.
             */
            ++tempSpec.name[0];
            tempSpec.name[tempSpec.name[0]] = ':';

            (void) Munger(*fullPath, 0, NULL, 0, &tempSpec.name[1],
                tempSpec.name[0]);
            err = MemError();
            }
        } while ( (err == noErr) &&
            (pb.dirInfo.ioDrDirID != fsRtDirID) );
        }
    }
    }

    /*
     * On error Dispose the handle, set it to NULL & return the err.
     * Otherwise, set the length & return.
     */
    if (err == noErr) {
    *length = GetHandleSize(*fullPath) - 1;
    } else {
    if ( *fullPath != NULL ) {
        DisposeHandle(*fullPath);
    }
    *fullPath = NULL;
    *length = 0;
    }

    return err;
}



/*****************************************************************************/

pascal  OSErr   FSpGetDirectoryID(const FSSpec *spec,
                                  long *theDirID,
                                  Boolean *isDirectory)
{
    return ( GetDirectoryID(spec->vRefNum, spec->parID, spec->name,
             theDirID, isDirectory) );
}


/*****************************************************************************/

pascal  OSErr   GetDirectoryID(short vRefNum,
                               long dirID,
                               ConstStr255Param name,
                               long *theDirID,
                               Boolean *isDirectory)
{
    CInfoPBRec pb;
    OSErr error;

    error = GetCatInfoNoName(vRefNum, dirID, name, &pb);
    if ( error == noErr )
    {
        *isDirectory = (pb.hFileInfo.ioFlAttrib & ioDirMask) != 0;
        if ( *isDirectory )
        {
            *theDirID = pb.dirInfo.ioDrDirID;
        }
        else
        {
            *theDirID = pb.hFileInfo.ioFlParID;
        }
    }

    return ( error );
}


/*****************************************************************************/

pascal  OSErr GetCatInfoNoName(short vRefNum,
                               long dirID,
                               ConstStr255Param name,
                               CInfoPBPtr pb)
{
    Str31 tempName;
    OSErr error;

    /* Protection against File Sharing problem */
    if ( (name == NULL) || (name[0] == 0) )
    {
        tempName[0] = 0;
        pb->dirInfo.ioNamePtr = tempName;
        pb->dirInfo.ioFDirIndex = -1;   /* use ioDirID */
    }
    else
    {
        pb->dirInfo.ioNamePtr = (StringPtr)name;
        pb->dirInfo.ioFDirIndex = 0;    /* use ioNamePtr and ioDirID */
    }
    pb->dirInfo.ioVRefNum = vRefNum;
    pb->dirInfo.ioDrDirID = dirID;
    error = PBGetCatInfoSync(pb);
    pb->dirInfo.ioNamePtr = NULL;
    return ( error );
}



/*****************************************************************************/

pascal  OSErr   GetObjectLocation(short vRefNum,
                                  long dirID,
                                  ConstStr255Param pathname,
                                  short *realVRefNum,
                                  long *realParID,
                                  Str255 realName,
                                  Boolean *isDirectory)
{
    OSErr error;
    CInfoPBRec pb;
    Str255 tempPathname;

    /* clear results */
    *realVRefNum = 0;
    *realParID = 0;
    realName[0] = 0;

    /*
    **  Get the real vRefNum
    */
    error = DetermineVRefNum(pathname, vRefNum, realVRefNum);
    if ( error == noErr )
    {
        /*
        **  Determine if the object already exists and if so,
        **  get the real parent directory ID if it's a file
        */

        /* Protection against File Sharing problem */
        if ( (pathname == NULL) || (pathname[0] == 0) )
        {
            tempPathname[0] = 0;
            pb.hFileInfo.ioNamePtr = tempPathname;
            pb.hFileInfo.ioFDirIndex = -1;  /* use ioDirID */
        }
        else
        {
            pb.hFileInfo.ioNamePtr = (StringPtr)pathname;
            pb.hFileInfo.ioFDirIndex = 0;   /* use ioNamePtr and ioDirID */
        }
        pb.hFileInfo.ioVRefNum = vRefNum;
        pb.hFileInfo.ioDirID = dirID;
        error = PBGetCatInfoSync(&pb);
        if ( error == noErr )
        {
            /*
            **  The file system object is present and we have the file's
            **  real parID
            */

            /*  Is it a directory or a file? */
            *isDirectory = (pb.hFileInfo.ioFlAttrib & ioDirMask) != 0;
            if ( *isDirectory )
            {
                /*
                **  It's a directory, get its name and parent dirID, and then
                **  we're done
                */

                pb.dirInfo.ioNamePtr = realName;
                pb.dirInfo.ioVRefNum = *realVRefNum;
                /* pb.dirInfo.ioDrDirID already contains the dirID of the
                   directory object */
                pb.dirInfo.ioFDirIndex = -1;    /* get information about ioDirID */
                error = PBGetCatInfoSync(&pb);

                /* get the parent ID here, because the file system can return the */
                /* wrong parent ID from the last call. */
                *realParID = pb.dirInfo.ioDrParID;
            }
            else
            {
                /*
                **  It's a file - use the parent directory ID from the last call
                **  to GetCatInfoparse, get the file name, and then we're done
                */
                *realParID = pb.hFileInfo.ioFlParID;
                error = GetFilenameFromPathname(pathname, realName);
            }
        }
        else if ( error == fnfErr )
        {
            /*
            **  The file system object is not present - see if its parent is present
            */

            /*
            **  Parse to get the object name from end of pathname
            */
            error = GetFilenameFromPathname(pathname, realName);

            /* if we can't get the object name from the end, we can't continue */
            if ( error == noErr )
            {
                /*
                **  What we want now is the pathname minus the object name
                **  for example:
                **  if pathname is 'vol:dir:file' tempPathname becomes 'vol:dir:'
                **  if pathname is 'vol:dir:file:' tempPathname becomes 'vol:dir:'
                **  if pathname is ':dir:file' tempPathname becomes ':dir:'
                **  if pathname is ':dir:file:' tempPathname becomes ':dir:'
                **  if pathname is ':file' tempPathname becomes ':'
                **  if pathname is 'file or file:' tempPathname becomes ''
                */

                /* get a copy of the pathname */
                BlockMoveData(pathname, tempPathname, pathname[0] + 1);

                /* remove the object name */
                tempPathname[0] -= realName[0];
                /* and the trailing colon (if any) */
                if ( pathname[pathname[0]] == ':' )
                {
                    --tempPathname[0];
                }

                /* OK, now get the parent's directory ID */

                /* Protection against File Sharing problem */
                pb.hFileInfo.ioNamePtr = (StringPtr)tempPathname;
                if ( tempPathname[0] != 0 )
                {
                    pb.hFileInfo.ioFDirIndex = 0;   /* use ioNamePtr and ioDirID */
                }
                else
                {
                    pb.hFileInfo.ioFDirIndex = -1;  /* use ioDirID */
                }
                pb.hFileInfo.ioVRefNum = vRefNum;
                pb.hFileInfo.ioDirID = dirID;
                error = PBGetCatInfoSync(&pb);
                *realParID = pb.dirInfo.ioDrDirID;

                *isDirectory = false;   /* we don't know what the object is
                                           really going to be */
            }

            if ( error != noErr )
            {
                error = dirNFErr;   /* couldn't find parent directory */
            }
            else
            {
                error = fnfErr; /* we found the parent, but not the file */
            }
        }
    }

    return ( error );
}



/*****************************************************************************/

pascal  OSErr   DetermineVRefNum(ConstStr255Param pathname,
                                 short vRefNum,
                                 short *realVRefNum)
{
    HParamBlockRec pb;
    OSErr error;

    error = GetVolumeInfoNoName(pathname,vRefNum, &pb);
    if ( error == noErr )
    {
        *realVRefNum = pb.volumeParam.ioVRefNum;
    }
    return ( error );
}


/*****************************************************************************/

pascal  OSErr   GetFilenameFromPathname(ConstStr255Param pathname,
                                        Str255 filename)
{
    short   index;
    short   nameEnd;
    OSErr   error;

    /* default to no filename */
    filename[0] = 0;

    /* check for no pathname */
    if ( pathname != NULL )
    {
        /* get string length */
        index = pathname[0];

        /* check for empty string */
        if ( index != 0 )
        {
            /* skip over last trailing colon (if any) */
            if ( pathname[index] == ':' )
            {
                --index;
            }

            /* save the end of the string */
            nameEnd = index;

            /* if pathname ends with multiple colons, then this pathname refers */
            /* to a directory, not a file */
            if ( pathname[index] != ':' )
            {
                /* parse backwards until we find a colon or hit the beginning
                   of the pathname */
                while ( (index != 0) && (pathname[index] != ':') )
                {
                    --index;
                }

                /* if we parsed to the beginning of the pathname and the
                   pathname ended */
                /* with a colon, then pathname is a full pathname to a volume,
                   not a file */
                if ( (index != 0) || (pathname[pathname[0]] != ':') )
                {
                    /* get the filename and return noErr */
                    filename[0] = (char)(nameEnd - index);
                    BlockMoveData(&pathname[index+1], &filename[1], nameEnd - index);
                    error = noErr;
                }
                else
                {
                    /* pathname to a volume, not a file */
                    error = notAFileErr;
                }
            }
            else
            {
                /* directory, not a file */
                error = notAFileErr;
            }
        }
        else
        {
            /* empty string isn't a file */
            error = notAFileErr;
        }
    }
    else
    {
        /* NULL pathname isn't a file */
        error = notAFileErr;
    }

    return ( error );
}



/*****************************************************************************/

/*
**  GetVolumeInfoNoName uses pathname and vRefNum to call PBHGetVInfoSync
**  in cases where the returned volume name is not needed by the caller.
**  The pathname and vRefNum parameters are not touched, and the pb
**  parameter is initialized by PBHGetVInfoSync except that ioNamePtr in
**  the parameter block is always returned as NULL (since it might point
**  to the local tempPathname).
**
**  I noticed using this code in several places, so here it is once.
**  This reduces the code size of MoreFiles.
*/
pascal  OSErr   GetVolumeInfoNoName(ConstStr255Param pathname,
                                    short vRefNum,
                                    HParmBlkPtr pb)
{
    Str255 tempPathname;
    OSErr error;

    /* Make sure pb parameter is not NULL */
    if ( pb != NULL )
    {
        pb->volumeParam.ioVRefNum = vRefNum;
        if ( pathname == NULL )
        {
            pb->volumeParam.ioNamePtr = NULL;
            pb->volumeParam.ioVolIndex = 0;     /* use ioVRefNum only */
        }
        else
        {                                   /* make a copy of the string and */
            BlockMoveData(pathname, tempPathname, pathname[0] + 1);
                                    /* use the copy so original isn't trashed */
            pb->volumeParam.ioNamePtr = (StringPtr)tempPathname;
                                       /* use ioNamePtr/ioVRefNum combination */
            pb->volumeParam.ioVolIndex = -1;
        }
        error = PBHGetVInfoSync(pb);
        pb->volumeParam.ioNamePtr = NULL;   /* ioNamePtr may point to local
                                            tempPathname, so don't return it */
    }
    else
    {
        error = paramErr;
    }
    return ( error );
}




/*****************************************************************************/

pascal  OSErr   FSpGetFullPath(const FSSpec *spec,
                               short *fullPathLength,
                               Handle *fullPath)
{
    OSErr       result;
    OSErr       realResult;
    FSSpec      tempSpec;
    CInfoPBRec  pb;

    *fullPathLength = 0;
    *fullPath = NULL;

    /* Default to noErr */
    realResult = noErr;

    /* Make a copy of the input FSSpec that can be modified */
    BlockMoveData(spec, &tempSpec, sizeof(FSSpec));

    if ( tempSpec.parID == fsRtParID )
    {
        /* The object is a volume */

        /* Add a colon to make it a full pathname */
        ++tempSpec.name[0];
        tempSpec.name[tempSpec.name[0]] = ':';

        /* We're done */
        result = PtrToHand(&tempSpec.name[1], fullPath, tempSpec.name[0]);
    }
    else
    {
        /* The object isn't a volume */

        /* Is the object a file or a directory? */
        pb.dirInfo.ioNamePtr = tempSpec.name;
        pb.dirInfo.ioVRefNum = tempSpec.vRefNum;
        pb.dirInfo.ioDrDirID = tempSpec.parID;
        pb.dirInfo.ioFDirIndex = 0;
        result = PBGetCatInfoSync(&pb);
        /* Allow file/directory name at end of path to not exist. */
        realResult = result;
        if ( (result == noErr) || (result == fnfErr) )
        {
            /* if the object is a directory, append a colon so full pathname
               ends with colon */
            if ( (result == noErr) && (pb.hFileInfo.ioFlAttrib & ioDirMask) != 0 )
            {
                ++tempSpec.name[0];
                tempSpec.name[tempSpec.name[0]] = ':';
            }

            /* Put the object name in first */
            result = PtrToHand(&tempSpec.name[1], fullPath, tempSpec.name[0]);
            if ( result == noErr )
            {
                /* Get the ancestor directory names */
                pb.dirInfo.ioNamePtr = tempSpec.name;
                pb.dirInfo.ioVRefNum = tempSpec.vRefNum;
                pb.dirInfo.ioDrParID = tempSpec.parID;
                do  /* loop until we have an error or find the root directory */
                {
                    pb.dirInfo.ioFDirIndex = -1;
                    pb.dirInfo.ioDrDirID = pb.dirInfo.ioDrParID;
                    result = PBGetCatInfoSync(&pb);
                    if ( result == noErr )
                    {
                        /* Append colon to directory name */
                        ++tempSpec.name[0];
                        tempSpec.name[tempSpec.name[0]] = ':';

                        /* Add directory name to beginning of fullPath */
                        (void) Munger(*fullPath, 0, NULL, 0, &tempSpec.name[1],
                                      tempSpec.name[0]);
                        result = MemError();
                    }
                } while ( (result == noErr) && (pb.dirInfo.ioDrDirID != fsRtDirID) );
            }
        }
    }
    if ( result == noErr )
    {
        /* Return the length */
        *fullPathLength = InlineGetHandleSize(*fullPath);
        result = realResult;    /* return realResult in case it was fnfErr */
    }
    else
    {
        /* Dispose of the handle and return NULL and zero length */
        if ( *fullPath != NULL )
        {
            DisposeHandle(*fullPath);
        }
        *fullPath = NULL;
        *fullPathLength = 0;
    }

    return ( result );
}



/*****************************************************************************/

pascal OSErr FSpLocationFromFullPath(short fullPathLength,
                                     const void *fullPath,
                                     FSSpec *spec)
{
    AliasHandle alias;
    OSErr       result;
    Boolean     wasChanged;
    Str32       nullString;

    /* Create a minimal alias from the full pathname */
    nullString[0] = 0;  /* null string to indicate no zone or server name */
    result = NewAliasMinimalFromFullPath(fullPathLength, fullPath, nullString,
                                         nullString, &alias);

    if ( result == noErr )
    {
        /* Let the Alias Manager resolve the alias. */
        result = ResolveAlias(NULL, alias, spec, &wasChanged);

        DisposeHandle((Handle)alias);   /* Free up memory used */
    }

    return ( result );
}



/*****************************************************************************/

pascal  OSErr   GetFullPath(short vRefNum,
                            long dirID,
                            ConstStr255Param name,
                            short *fullPathLength,
                            Handle *fullPath)
{
    OSErr       result;
    FSSpec      spec;

    *fullPathLength = 0;
    *fullPath = NULL;

    result = FSMakeFSSpecCompat(vRefNum, dirID, name, &spec);
    if ( (result == noErr) || (result == fnfErr) )
    {
        result = FSpGetFullPath(&spec, fullPathLength, fullPath);
    }

    return ( result );
}



/*****************************************************************************/

pascal  OSErr   ChangeCreatorType(short vRefNum,
                                  long dirID,
                                  ConstStr255Param name,
                                  OSType creator,
                                  OSType fileType)
{
    CInfoPBRec pb;
    OSErr error;
    short realVRefNum;
    long parID;

    pb.hFileInfo.ioNamePtr = (StringPtr)name;
    pb.hFileInfo.ioVRefNum = vRefNum;
    pb.hFileInfo.ioDirID = dirID;
    pb.hFileInfo.ioFDirIndex = 0;   /* use ioNamePtr and ioDirID */
    error = PBGetCatInfoSync(&pb);
    if ( error == noErr )
    {
        if ( (pb.hFileInfo.ioFlAttrib & ioDirMask) == 0 )   /* if file */
        {                            /* save parent dirID for BumpDate call */
            parID = pb.hFileInfo.ioFlParID;

            /* If creator not 0x00000000, change creator */
            if ( creator != (OSType)0x00000000 )
            {
                pb.hFileInfo.ioFlFndrInfo.fdCreator = creator;
            }

            /* If fileType not 0x00000000, change fileType */
            if ( fileType != (OSType)0x00000000 )
            {
                pb.hFileInfo.ioFlFndrInfo.fdType = fileType;
            }

            pb.hFileInfo.ioDirID = dirID;
            error = PBSetCatInfoSync(&pb);  /* now, save the new information
                                               back to disk */

            if ( (error == noErr) && (parID != fsRtParID) ) /* can't
                                                            bump fsRtParID */
            {
                /* get the real vRefNum in case a full pathname was passed */
                error = DetermineVRefNum(name, vRefNum, &realVRefNum);
                if ( error == noErr )
                {
                    error = BumpDate(realVRefNum, parID, NULL);
                        /* and bump the parent directory's mod date to wake
                           up the Finder */
                        /* to the change we just made */
                }
            }
        }
        else
        {
            /* it was a directory, not a file */
            error = notAFileErr;
        }
    }

    return ( error );
}

/*****************************************************************************/

pascal  OSErr   FSpChangeCreatorType(const FSSpec *spec,
                                     OSType creator,
                                     OSType fileType)
{
    return ( ChangeCreatorType(spec->vRefNum, spec->parID, spec->name,
             creator, fileType) );
}

/*****************************************************************************/

pascal  OSErr   BumpDate(short vRefNum,
                         long dirID,
                         ConstStr255Param name)
/* Given a file or directory, change its modification date to the
   current date/time. */
{
    CInfoPBRec pb;
    Str31 tempName;
    OSErr error;
    unsigned long secs;

    /* Protection against File Sharing problem */
    if ( (name == NULL) || (name[0] == 0) )
    {
        tempName[0] = 0;
        pb.hFileInfo.ioNamePtr = tempName;
        pb.hFileInfo.ioFDirIndex = -1;  /* use ioDirID */
    }
    else
    {
        pb.hFileInfo.ioNamePtr = (StringPtr)name;
        pb.hFileInfo.ioFDirIndex = 0;   /* use ioNamePtr and ioDirID */
    }
    pb.hFileInfo.ioVRefNum = vRefNum;
    pb.hFileInfo.ioDirID = dirID;
    error = PBGetCatInfoSync(&pb);
    if ( error == noErr )
    {
        GetDateTime(&secs);
        /* set mod date to current date, or one second into the future
            if mod date = current date */
        pb.hFileInfo.ioFlMdDat =
                          (secs == pb.hFileInfo.ioFlMdDat) ? (++secs) : (secs);
        if ( pb.dirInfo.ioNamePtr == tempName )
        {
            pb.hFileInfo.ioDirID = pb.hFileInfo.ioFlParID;
        }
        else
        {
            pb.hFileInfo.ioDirID = dirID;
        }
        error = PBSetCatInfoSync(&pb);
    }

    return ( error );
}

/*****************************************************************************/

pascal  OSErr   FSpBumpDate(const FSSpec *spec)
{
    return ( BumpDate(spec->vRefNum, spec->parID, spec->name) );
}


/*****************************************************************************/

pascal  OSErr   OnLine(FSSpecPtr volumes,
                       short reqVolCount,
                       short *actVolCount,
                       short *volIndex)
{
    HParamBlockRec pb;
    OSErr error = noErr;
    FSSpec *endVolArray;

    if ( *volIndex > 0 )
    {
        *actVolCount = 0;
        for ( endVolArray = volumes + reqVolCount;
              (volumes < endVolArray) && (error == noErr); ++volumes )
        {
            pb.volumeParam.ioNamePtr = (StringPtr) & volumes->name;
            pb.volumeParam.ioVolIndex = *volIndex;
            error = PBHGetVInfoSync(&pb);
            if ( error == noErr )
            {
                volumes->parID = fsRtParID;     /* the root directory's
                                                   parent is 1 */
                volumes->vRefNum = pb.volumeParam.ioVRefNum;
                ++*volIndex;
                ++*actVolCount;
            }
        }
    }
    else
    {
        error = paramErr;
    }

    return ( error );
}


/*****************************************************************************/

pascal  OSErr   DTGetComment(short vRefNum,
                             long dirID,
                             ConstStr255Param name,
                             Str255 comment)
{
    DTPBRec pb;
    OSErr error;
    short dtRefNum;
    Boolean newDTDatabase;

    if (comment != NULL)
    {
        comment[0] = 0; /* return nothing by default */

        /* attempt to open the desktop database */
        error = DTOpen(name, vRefNum, &dtRefNum, &newDTDatabase);
        if ( error == noErr )
        {
            /* There was a desktop database and it's now open */

            if ( !newDTDatabase )
            {
                pb.ioDTRefNum = dtRefNum;
                pb.ioNamePtr = (StringPtr)name;
                pb.ioDirID = dirID;
                pb.ioDTBuffer = (Ptr)&comment[1];
                /*
                **  IMPORTANT NOTE #1: Inside Macintosh says that comments
                **  are up to 200 characters. While that may be correct for
                **  the HFS file system's Desktop Manager, other file
                **  systems (such as Apple Photo Access) return up to
                **  255 characters. Make sure the comment buffer is a Str255
                **  or you'll regret it.
                **
                **  IMPORTANT NOTE #2: Although Inside Macintosh doesn't
                **  mention it, ioDTReqCount is a input field to
                **  PBDTGetCommentSync. Some file systems (like HFS) ignore
                **  ioDTReqCount and always return the full comment --
                **  others (like AppleShare) respect ioDTReqCount and only
                **  return up to ioDTReqCount characters of the comment.
                */
                pb.ioDTReqCount = sizeof(Str255) - 1;
                error = PBDTGetCommentSync(&pb);
                if (error == noErr)
                {
                    comment[0] = (unsigned char)pb.ioDTActCount;
                }
            }
        }
        else
        {
            /* There is no desktop database - try the Desktop file */
            error = GetCommentFromDesktopFile(vRefNum, dirID, name, comment);
            if ( error != noErr )
            {
                error = afpItemNotFound;    /* return an expected error */
            }
        }
    }
    else
    {
        error = paramErr;
    }

    return (error);
}

/*****************************************************************************/

pascal  OSErr   FSpDTGetComment(const FSSpec *spec,
                              Str255 comment)
{
    return (DTGetComment(spec->vRefNum, spec->parID, spec->name, comment));
}


/*****************************************************************************/

pascal  OSErr   DTSetComment(short vRefNum,
                             long dirID,
                             ConstStr255Param name,
                             ConstStr255Param comment)
{
    DTPBRec pb;
    OSErr error;
    short dtRefNum;
    Boolean newDTDatabase;

    error = DTOpen(name, vRefNum, &dtRefNum, &newDTDatabase);
    if ( error == noErr )
    {
        pb.ioDTRefNum = dtRefNum;
        pb.ioNamePtr = (StringPtr)name;
        pb.ioDirID = dirID;
        pb.ioDTBuffer = (Ptr)&comment[1];
        /* Truncate the comment to 200 characters just in case */
        /* some file system doesn't range check */
        if ( comment[0] <= 200 )
        {
            pb.ioDTReqCount = comment[0];
        }
        else
        {
            pb.ioDTReqCount = 200;
        }
        error = PBDTSetCommentSync(&pb);
    }
    return (error);
}

/*****************************************************************************/

pascal  OSErr   FSpDTSetComment(const FSSpec *spec,
                              ConstStr255Param comment)
{
    return (DTSetComment(spec->vRefNum, spec->parID, spec->name, comment));
}


/*****************************************************************************/

pascal  OSErr   DTOpen(ConstStr255Param volName,
                       short vRefNum,
                       short *dtRefNum,
                       Boolean *newDTDatabase)
{
    OSErr error;
    GetVolParmsInfoBuffer volParmsInfo;
    long infoSize;
    DTPBRec pb;

    /* Check for volume Desktop Manager support before calling */
    infoSize = sizeof(GetVolParmsInfoBuffer);
    error = HGetVolParms(volName, vRefNum, &volParmsInfo, &infoSize);
    if ( error == noErr )
    {
        if ( hasDesktopMgr(volParmsInfo) )
        {
            pb.ioNamePtr = (StringPtr)volName;
            pb.ioVRefNum = vRefNum;
            error = PBDTOpenInform(&pb);
            /* PBDTOpenInform informs us if the desktop was just created */
            /* by leaving the low bit of ioTagInfo clear (0) */
            *newDTDatabase = ((pb.ioTagInfo & 1L) == 0);
            if ( error == paramErr )
            {
                error = PBDTGetPath(&pb);
                /* PBDTGetPath doesn't tell us if the database is new */
                /* so assume it is not new */
                *newDTDatabase = false;
            }
            *dtRefNum = pb.ioDTRefNum;
        }
        else
        {
            error = paramErr;
        }
    }
    return ( error );
}

/*****************************************************************************/

/*
**  GetCommentFromDesktopFile
**
**  Get a file or directory's Finder comment field (if any) from the
**  Desktop file's 'FCMT' resources.
*/
static  OSErr   GetCommentFromDesktopFile(short vRefNum,
                                          long dirID,
                                          ConstStr255Param name,
                                          Str255 comment)
{
    OSErr error;
    short commentID;
    short realVRefNum;
    Str255 desktopName;
    short savedResFile;
    short dfRefNum;
    StringHandle commentHandle;

    /* Get the comment ID number */
    error = GetCommentID(vRefNum, dirID, name, &commentID);
    if ( error == noErr )
    {
        if ( commentID != 0 )   /* commentID == 0 means there's no comment */
        {
            error = DetermineVRefNum(name, vRefNum, &realVRefNum);
            if ( error == noErr )
            {
                error = GetDesktopFileName(realVRefNum, desktopName);
                if ( error == noErr )
                {
                    savedResFile = CurResFile();
                    /*
                    **  Open the 'Desktop' file in the root directory. (because
                    **  opening the resource file could preload unwanted resources,
                    **  bracket the call with SetResLoad(s))
                    */
                    SetResLoad(false);
                    dfRefNum = HOpenResFile(realVRefNum, fsRtDirID, desktopName,
                                            fsRdPerm);
                    SetResLoad(true);

                    if ( dfRefNum != -1)
                    {
                        /* Get the comment resource */
                        commentHandle = (StringHandle)Get1Resource(kFCMTResType,
                                                                   commentID);
                        if ( commentHandle != NULL )
                        {
                            if ( InlineGetHandleSize((Handle)commentHandle) > 0 )
                            {
                                BlockMoveData(*commentHandle, comment,
                                              *commentHandle[0] + 1);
                            }
                            else
                            {                       /* no comment available */
                                error = afpItemNotFound;
                            }
                        }
                        else
                        {                           /* no comment available */
                            error = afpItemNotFound;
                        }

                        /* restore the resource chain and close
                           the Desktop file */
                        UseResFile(savedResFile);
                        CloseResFile(dfRefNum);
                    }
                    else
                    {
                        error = afpItemNotFound;
                    }
                }
                else
                {
                    error = afpItemNotFound;
                }
            }
        }
        else
        {
            error = afpItemNotFound;    /* no comment available */
        }
    }

    return ( error );
}

/*****************************************************************************/

pascal  OSErr   HGetVolParms(ConstStr255Param volName,
                             short vRefNum,
                             GetVolParmsInfoBuffer *volParmsInfo,
                             long *infoSize)
{
    HParamBlockRec pb;
    OSErr error;

    pb.ioParam.ioNamePtr = (StringPtr)volName;
    pb.ioParam.ioVRefNum = vRefNum;
    pb.ioParam.ioBuffer = (Ptr)volParmsInfo;
    pb.ioParam.ioReqCount = *infoSize;
    error = PBHGetVolParmsSync(&pb);
    if ( error == noErr )
    {
        *infoSize = pb.ioParam.ioActCount;
    }
    return ( error );
}

/*****************************************************************************/
/*
**  GetCommentID
**
**  Get the comment ID number for the Desktop file's 'FCMT' resource ID from
**  the file or folders fdComment (frComment) field.
*/
static  OSErr   GetCommentID(short vRefNum,
                             long dirID,
                             ConstStr255Param name,
                             short *commentID)
{
    CInfoPBRec pb;
    OSErr error;

    error = GetCatInfoNoName(vRefNum, dirID, name, &pb);
    *commentID = pb.hFileInfo.ioFlXFndrInfo.fdComment;
    return ( error );
}

/*****************************************************************************/

/*
**  GetDesktopFileName
**
**  Get the name of the Desktop file.
*/
static  OSErr   GetDesktopFileName(short vRefNum,
                                   Str255 desktopName)
{
    OSErr           error;
    HParamBlockRec  pb;
    short           index;
    Boolean         found;

    pb.fileParam.ioNamePtr = desktopName;
    pb.fileParam.ioVRefNum = vRefNum;
    pb.fileParam.ioFVersNum = 0;
    index = 1;
    found = false;
    do
    {
        pb.fileParam.ioDirID = fsRtDirID;
        pb.fileParam.ioFDirIndex = index;
        error = PBHGetFInfoSync(&pb);
        if ( error == noErr )
        {
            if ( (pb.fileParam.ioFlFndrInfo.fdType == 'FNDR') &&
                 (pb.fileParam.ioFlFndrInfo.fdCreator == 'ERIK') )
            {
                found = true;
            }
        }
        ++index;
    } while ( (error == noErr) && !found );

    return ( error );
}


/*****************************************************************************/

pascal  OSErr   XGetVInfo(short volReference,
                          StringPtr volName,
                          short *vRefNum,
                          UnsignedWide *freeBytes,
                          UnsignedWide *totalBytes)
{
    OSErr           result;
    long            response;
    XVolumeParam    pb;

    /* See if large volume support is available */
    if ( ( Gestalt(gestaltFSAttr, &response) == noErr ) && ((response & (1L << gestaltFSSupports2TBVols)) != 0) )
    {
        /* Large volume support is available */
        pb.ioVRefNum = volReference;
        pb.ioNamePtr = volName;
        pb.ioXVersion = 0;  /* this XVolumeParam version (0) */
        pb.ioVolIndex = 0;  /* use ioVRefNum only, return volume name */
        result = PBXGetVolInfoSync(&pb);
        if ( result == noErr )
        {
            /* The volume name was returned in volName (if not NULL) and */
            /* we have the volume's vRefNum and allocation block size */
            *vRefNum = pb.ioVRefNum;

            /* return the freeBytes and totalBytes */
            *totalBytes = pb.ioVTotalBytes;
            *freeBytes = pb.ioVFreeBytes;
        }
    }
    else
    {
        /* No large volume support */

        /* Use HGetVInfo to get the results */
        result = HGetVInfo(volReference, volName, vRefNum, &freeBytes->lo, &totalBytes->lo);
        if ( result == noErr )
        {
            /* zero the high longs of totalBytes and freeBytes */
            totalBytes->hi = 0;
            freeBytes->hi = 0;
        }
    }
    return ( result );
}



/*****************************************************************************/

pascal  OSErr   HGetVInfo(short volReference,
                          StringPtr volName,
                          short *vRefNum,
                          unsigned long *freeBytes,
                          unsigned long *totalBytes)
{
    HParamBlockRec  pb;
    unsigned long   allocationBlockSize;
    unsigned short  numAllocationBlocks;
    unsigned short  numFreeBlocks;
    VCB             *theVCB;
    Boolean         vcbFound;
    OSErr           result;

    /* Use the File Manager to get the real vRefNum */
    pb.volumeParam.ioVRefNum = volReference;
    pb.volumeParam.ioNamePtr = volName;
    pb.volumeParam.ioVolIndex = 0;  /* use ioVRefNum only, return volume name */
    result = PBHGetVInfoSync(&pb);

    if ( result == noErr )
    {
        /* The volume name was returned in volName (if not NULL) and */
        /* we have the volume's vRefNum and allocation block size */
        *vRefNum = pb.volumeParam.ioVRefNum;
        allocationBlockSize = (unsigned long)pb.volumeParam.ioVAlBlkSiz;

        /* System 7.5 (and beyond) pins the number of allocation blocks and */
        /* the number of free allocation blocks returned by PBHGetVInfo to */
        /* a value so that when multiplied by the allocation block size, */
        /* the volume will look like it has $7fffffff bytes or less. This */
        /* was done so older applications that use signed math or that use */
        /* the GetVInfo function (which uses signed math) will continue to work. */
        /* However, the unpinned numbers (which we want) are always available */
        /* in the volume's VCB so we'll get those values from the VCB if possible. */

        /* Find the volume's VCB */
        vcbFound = false;
        theVCB = (VCB *)(GetVCBQHdr()->qHead);
        while ( (theVCB != NULL) && !vcbFound )
        {
            /* Check VCB signature before using VCB. Don't have to check for */
            /* MFS (0xd2d7) because they can't get big enough to be pinned */
            if ( theVCB->vcbSigWord == 0x4244 )
            {
                if ( theVCB->vcbVRefNum == *vRefNum )
                {
                    vcbFound = true;
                }
            }

            if ( !vcbFound )
            {
                theVCB = (VCB *)(theVCB->qLink);
            }
        }

        if ( theVCB != NULL )
        {
            /* Found a VCB we can use. Get the un-pinned number of allocation blocks */
            /* and the number of free blocks from the VCB. */
            numAllocationBlocks = (unsigned short)theVCB->vcbNmAlBlks;
            numFreeBlocks = (unsigned short)theVCB->vcbFreeBks;
        }
        else
        {
            /* Didn't find a VCB we can use. Return the number of allocation blocks */
            /* and the number of free blocks returned by PBHGetVInfoSync. */
            numAllocationBlocks = (unsigned short)pb.volumeParam.ioVNmAlBlks;
            numFreeBlocks = (unsigned short)pb.volumeParam.ioVFrBlk;
        }

        /* Now, calculate freeBytes and totalBytes using unsigned values */
        *freeBytes = numFreeBlocks * allocationBlockSize;
        *totalBytes = numAllocationBlocks * allocationBlockSize;
    }

    return ( result );
}


/*
**  PBXGetVolInfoSync is the glue code needed to make PBXGetVolInfoSync
**  File Manager requests from CFM-based programs. At some point, Apple
**  will get around to adding this to the standard libraries you link with
**  and you'll get a duplicate symbol link error. At that time, just delete
**  this code (or comment it out).
**
**  Non-CFM 68K programs don't needs this glue (and won't get it) because
**  they instead use the inline assembly glue found in the Files.h interface
**  file.
*/

#if __WANTPASCALELIMINATION
#undef  pascal
#endif

#if GENERATINGCFM
pascal OSErr PBXGetVolInfoSync(XVolumeParamPtr paramBlock)
{
    enum
    {
        kXGetVolInfoSelector = 0x0012,  /* Selector for XGetVolInfo */

        uppFSDispatchProcInfo = kRegisterBased
             | REGISTER_RESULT_LOCATION(kRegisterD0)
             | RESULT_SIZE(SIZE_CODE(sizeof(OSErr)))
             | REGISTER_ROUTINE_PARAMETER(1, kRegisterD1, SIZE_CODE(sizeof(long)))  /* trap word */
             | REGISTER_ROUTINE_PARAMETER(2, kRegisterD0, SIZE_CODE(sizeof(long)))  /* selector */
             | REGISTER_ROUTINE_PARAMETER(3, kRegisterA0, SIZE_CODE(sizeof(XVolumeParamPtr)))
    };

    return ( CallOSTrapUniversalProc(NGetTrapAddress(_FSDispatch, OSTrap),
                                        uppFSDispatchProcInfo,
                                        _FSDispatch,
                                        kXGetVolInfoSelector,
                                        paramBlock) );
}
#endif

#if __WANTPASCALELIMINATION
#define pascal
#endif

/*****************************************************************************/

pascal  OSErr   GetDirName(short vRefNum,
                           long dirID,
                           Str31 name)
{
    CInfoPBRec pb;
    OSErr error;

    if ( name != NULL )
    {
        pb.dirInfo.ioNamePtr = name;
        pb.dirInfo.ioVRefNum = vRefNum;
        pb.dirInfo.ioDrDirID = dirID;
        pb.dirInfo.ioFDirIndex = -1;    /* get information about ioDirID */
        error = PBGetCatInfoSync(&pb);
    }
    else
    {
        error = paramErr;
    }

    return ( error );
}


/*****************************************************************************/

pascal  OSErr   GetVolFileSystemID(ConstStr255Param pathname,
                                   short vRefNum,
                                   short *fileSystemID)
{
    HParamBlockRec pb;
    OSErr error;

    error = GetVolumeInfoNoName(pathname,vRefNum, &pb);
    if ( error == noErr )
    {
        *fileSystemID = pb.volumeParam.ioVFSID;
    }

    return ( error );
}

/*****************************************************************************/

pascal  OSErr GetDInfo(short vRefNum,
                       long dirID,
                       ConstStr255Param name,
                       DInfo *fndrInfo)
{
    CInfoPBRec pb;
    OSErr error;

    error = GetCatInfoNoName(vRefNum, dirID, name, &pb);
    if ( error == noErr )
    {
        if ( (pb.dirInfo.ioFlAttrib & ioDirMask) != 0 )
        {
            /* it's a directory, return the DInfo */
            *fndrInfo = pb.dirInfo.ioDrUsrWds;
        }
        else
        {
            /* oops, a file was passed */
            error = dirNFErr;
        }
    }

    return ( error );
}

/*****************************************************************************/

pascal  OSErr FSpGetDInfo(const FSSpec *spec,
                          DInfo *fndrInfo)
{
    return ( GetDInfo(spec->vRefNum, spec->parID, spec->name, fndrInfo) );
}


