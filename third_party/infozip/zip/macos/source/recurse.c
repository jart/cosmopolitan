/*
These functions are based on Jim Luther's IterateDirectory() found in MoreFiles
However, it's heavily modified by Dirk Haase
*/

/*
**  IterateDirectory: File Manager directory iterator routines.
**
**  by Jim Luther
**
**  File:       IterateDirectory.c
**
**  Copyright (c) 1995-1998 Jim Luther and Apple Computer, Inc.
**  All rights reserved.
**
**  You may incorporate this sample code into your applications without
**  restriction, though the sample code has been provided "AS IS" and the
**  responsibility for its operation is 100% yours.
**
**  IterateDirectory is designed to drop into the MoreFiles sample code
**  library I wrote while in Apple Developer Technical Support
*/

/*****************************************************************************/
/*  Includes                                                                 */
/*****************************************************************************/

#include <Types.h>
#include <Errors.h>
#include <Files.h>
#include <stdio.h>
#include <string.h>


#include "zip.h"
#include "macstuff.h"
#include "helpers.h"
#include "recurse.h"
#include "macglob.h"
#include "pathname.h"




/*****************************************************************************/
/*  Macros, typedefs                                                         */
/*****************************************************************************/

/* The RecurseGlobals structure is used to minimize the amount of
** stack space used when recursively calling RecurseDirectoryLevel
** and to hold global information that might be needed at any time.
*/
struct RecurseGlobals
{
    short                   vRefNum;
    CInfoPBRec              cPB;            /* the parameter block used for
                                               PBGetCatInfo calls */
    unsigned char           *itemName;      /* the name of the current item */
    char                    *FullPath;
    short                   FullPathLen;
    OSErr                   result;         /* temporary holder of results -
                                            saves 2 bytes of stack each level */
    Boolean                 quitFlag;       /* set to true if filter wants to
                                               kill interation */
    unsigned short          maxLevels;      /* Maximum levels to
                                               iterate through */
    unsigned short          currentLevel;   /* The current level
                                               IterateLevel is on */
};

typedef struct RecurseGlobals RecurseGlobals;
typedef RecurseGlobals *RecurseGlobalsPtr;


/*****************************************************************************/
/*  Global Vars                                                              */
/*****************************************************************************/

extern MacZipGlobals    MacZip;
extern const char ResourceMark[13]; /* "XtraStuf.mac:" var is initialized in file pathname.c */
extern int extra_fields;            /* do not create extra fields if false */

static RecurseGlobals theGlobals;

static unsigned long    DirLevels = 0;
static char *buffer;
extern int verbose;        /* 1=report oddities in zip file structure */

/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/

int procname(char *filename, int caseflag);
int MatchWild( char *pPat, char *pStr, int case_sens);
Boolean IsZipFile(char *name);

static  void    RecurseDirectoryLevel(long DirID, RecurseGlobals *Globals);
static Boolean isRegularItem( RecurseGlobals *Globals);
static void ProcessFiles(RecurseGlobals *Globals,
                         Boolean hasDataFork, Boolean hasResourceFork);
static void ProcessDirectory(RecurseGlobals *Globals,
                             Boolean IncludeItem, long DirID);
static void ProcessItem(RecurseGlobals *Globals, long DirID);

/*****************************************************************************/
/*  Functions                                                                */
/*****************************************************************************/

static  void    RecurseDirectoryLevel(long DirID, RecurseGlobals *Globals)
{
char buffer2[23];

                 /* if maxLevels is zero, we aren't checking levels */
    if ( (Globals->maxLevels == 0) ||
                 /* if currentLevel < maxLevels, look at this level */
         (Globals->currentLevel < Globals->maxLevels) )
    {
        short index = 1;

        ++Globals->currentLevel;    /* go to next level */
        if (DirLevels < Globals->currentLevel) DirLevels = Globals->currentLevel;
        sprintf(buffer2,"Globals->currentLevel: %d",Globals->currentLevel);

        do
        {   /* Isn't C great... What I'd give for a "WITH
                theGlobals DO" about now... */

            /* Get next source item at the current directory level */
            Globals->cPB.dirInfo.ioFDirIndex = index;
            Globals->cPB.dirInfo.ioDrDirID = DirID;
            Globals->result = PBGetCatInfoSync((CInfoPBPtr)&Globals->cPB);

                        ShowCounter(false);

            if ( Globals->result == noErr )
                {
                ProcessItem(Globals, DirID);
                } /* if ( Globals->result == noErr ) */

            ++index; /* prepare to get next item */
                                         /* time to fall back a level? */
        } while ( (Globals->result == noErr) && (!Globals->quitFlag) );

        if ( (Globals->result == fnfErr) || /* fnfErr is OK -
                                               it only means we hit
                                               the end of this level */
             (Globals->result == afpAccessDenied) ) /* afpAccessDenied is OK,
                      too - it only means we cannot see inside a directory */
        {
            Globals->result = noErr;
        }

        --Globals->currentLevel;    /* return to previous level as we leave */
    }
}



/*****************************************************************************/

pascal  OSErr   RecurseDirectory(short vRefNum,
                                 long thedirID,
                                 ConstStr255Param name,
                                 unsigned short maxLevels)
{
    OSErr           result;
    short           theVRefNum;
    Boolean         isDirectory;
    long            DirID;

        /* Get the real directory ID and make sure it is a directory */
        result = GetDirectoryID(vRefNum, thedirID, name, &DirID, &isDirectory);
        if ( result == noErr )
        {
            if ( isDirectory == true )
            {
                /* Get the real vRefNum */
                result = DetermineVRefNum(name, vRefNum, &theVRefNum);
                if ( result == noErr )
                {
                    /* Set up the globals we need to access from
                       the recursive routine. */
                    theGlobals.cPB.hFileInfo.ioNamePtr = theGlobals.itemName;
                    theGlobals.cPB.hFileInfo.ioVRefNum = theVRefNum;
                    theGlobals.itemName[0] = 0;
                    theGlobals.result = noErr;
                    theGlobals.quitFlag = false;
                    theGlobals.maxLevels = maxLevels;
                    theGlobals.currentLevel = 0;    /* start at level 0 */

                    /* Here we go into recursion land... */
                    RecurseDirectoryLevel(DirID, &theGlobals);

                    result = theGlobals.result; /* set the result */
                }
            }
            else
            {
                result = dirNFErr;  /* a file was passed instead
                                       of a directory */
            }
        }

    return ( result );
}



/*****************************************************************************/

pascal  OSErr   FSpRecurseDirectory(const FSSpec *spec,
                                    unsigned short maxLevels)
{
    OSErr rc;

    theGlobals.vRefNum = spec->vRefNum;

    /* make room for pathnames  */
    theGlobals.itemName = (unsigned char *) StrCalloc(NAME_MAX);
    theGlobals.FullPath = StrCalloc(NAME_MAX);
    buffer = StrCalloc(NAME_MAX);


    if ((noisy) && (MacZip.DataForkOnly))
        printf("\n Warning: Datafork only \n");

    /* reset the count to zero */
    ShowCounter(true);

    if (noisy) leftStatusString("Build File List; Items done:");
    if (noisy) printf("\n Collecting Filenames ...");
    rc = RecurseDirectory(spec->vRefNum, spec->parID, spec->name,maxLevels);
    printerr("RecurseDirectory:",rc,rc,__LINE__,__FILE__,"");

    if (noisy) printf("\n... done \n\n %6d matched files found  \n",
                      MacZip.FoundFiles);
    if (noisy) printf(" %6d folders found in %d Levels \n",
                         MacZip.FoundDirectories,DirLevels);

    if (MacZip.BytesOfData > (1024*1024))
        if (noisy) printf(" %4.3f MBytes unzipped size\n\n",
                            (float) MacZip.BytesOfData/(1024*1024));
    else
        if (noisy) printf(" %4.3f KBytes unzipped size\n\n",
                           (float) MacZip.BytesOfData/1024);

    /* free all memory of pathnames  */
    theGlobals.itemName = (unsigned char *) StrFree((char *)theGlobals.itemName);
    theGlobals.FullPath = StrFree(theGlobals.FullPath);
    buffer = StrFree(buffer);

    return rc;
}




/*
* Return true if filename == zipfile
* After the first match no further check will be done !
*
*/
Boolean IsZipFile(char *filen)
{
static firstMatch = false;

if (filen == NULL)
    firstMatch = false;

if (!firstMatch)
    {
    if (stricmp(filen, MacZip.ZipFullPath) == 0)
        {
        firstMatch = true;
        return true;
        }
    }

return false;
}



static Boolean isRegularItem( RecurseGlobals *Globals)
{
Boolean     isInvisible = false,
            isAlias     = false,
            isSystem    = false;

isSystem    = !((Globals->cPB.hFileInfo.ioFlFndrInfo.fdFlags &
              (1 << 12)) == 0 );
isInvisible = !((Globals->cPB.hFileInfo.ioFlFndrInfo.fdFlags &
              (1 << 14)) == 0 );
isAlias     = !((Globals->cPB.hFileInfo.ioFlFndrInfo.fdFlags &
              (1 << 15)) == 0);

if (isAlias == true)
    {
    return false;
    }

if (MacZip.IncludeInvisible == true)
    {
    return true;
    }

if  ((isSystem == true) ||
    (isInvisible == true))
    {
    return false;
    }

return true;
}




static void ProcessFiles(RecurseGlobals *Globals,
                         Boolean hasDataFork, Boolean hasResourceFork)
{
 /* some file statistics */
MacZip.FoundFiles++;

if (hasDataFork == true)
    {
    MacZip.BytesOfData =
                Globals->cPB.hFileInfo.ioFlLgLen +
                MacZip.BytesOfData;
    MacZip.CurrentFork = DataFork;
    MacZip.RawCountOfItems++;

    if (MacZip.DataForkOnly == true)
        {
        procname(Globals->FullPath, false);
        hasResourceFork = false;
        }
        else
        {
        procname(Real2RfDfFilen(buffer,Globals->FullPath,
                 DataFork, MacZip.MacZipMode,
                 MacZip.DataForkOnly), false);
        }
    }

if (hasResourceFork == true)
    {
    MacZip.BytesOfData =
                Globals->cPB.hFileInfo.ioFlRLgLen +
                MacZip.BytesOfData;
    MacZip.CurrentFork = ResourceFork;
    MacZip.RawCountOfItems++;

    procname(Real2RfDfFilen(buffer, Globals->FullPath,
             ResourceFork, MacZip.MacZipMode,
             MacZip.DataForkOnly), false);
    }
}




static void ProcessDirectory(RecurseGlobals *Globals,
                             Boolean IncludeItem, long DirID)
{
OSErr       rc;

MacZip.isDirectory = true;

GetFullPathFromID(Globals->FullPath,Globals->vRefNum, DirID,
                  Globals->itemName, &rc);

MacZip.RawCountOfItems++;
MacZip.FoundDirectories++;

if  (MacZip.StoreFoldersAlso)
    {
    procname(Globals->FullPath, false);
    }

     /* We have a directory */
    if ( !Globals->quitFlag && IncludeItem)
    {
   /* Dive again if the IterateFilterProc didn't say "quit" and dir is
      not an alias */
        RecurseDirectoryLevel(Globals->cPB.dirInfo.ioDrDirID,
                              Globals);
    }
}



static void ProcessItem(RecurseGlobals *Globals, long DirID)
{
OSErr rc;
Boolean     IncludeItem = false, hasDataFork = false;
Boolean     hasResourceFork = false;

IncludeItem = isRegularItem(Globals);

/* Is it a File? */
if ( (Globals->cPB.hFileInfo.ioFlAttrib & ioDirMask) == 0 )
    {
    PToCCpy(Globals->itemName,MacZip.FileName);
    MacZip.isDirectory = false;

    hasDataFork     = (Globals->cPB.hFileInfo.ioFlLgLen != 0);
    hasResourceFork = (Globals->cPB.hFileInfo.ioFlRLgLen != 0);

    /* include also files with zero recource- and data-fork */
    if ((hasDataFork == 0) && (hasResourceFork == 0))
        hasDataFork = true;

    if ((hasDataFork     == 0) &&
        (hasResourceFork != 0) &&
        (extra_fields    == false))
        {
        IncludeItem = false;
        }

    GetFullPathFromID(Globals->FullPath,Globals->vRefNum,
                      DirID, Globals->itemName, &rc);
    printerr("GetFullPathFromID:",rc,rc,__LINE__,
              __FILE__,MacZip.FileName);

    if  (IncludeItem  &&       /* don't include the zipfile itself */
        (!IsZipFile(Globals->FullPath))   )
        {
        if (MATCH(MacZip.Pattern, MacZip.FileName, false) == true)
            {
            ProcessFiles(Globals, hasDataFork, hasResourceFork);
            } /* if (MatchWild( MacZip.FileName,MacZip.Pattern ) ==
                    true) */
        }  /* if (!IsZipFile(Globals->FullPath)) */
    } /* Is it a File? */

/* Is it a directory? */
if ( (Globals->cPB.hFileInfo.ioFlAttrib & ioDirMask) != 0 )
    {
    ProcessDirectory(Globals,IncludeItem, DirID);
    } /* Is it a directory? */
}
