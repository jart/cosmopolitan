/*
  Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 1999-Oct-05 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, both of these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html
*/
/*---------------------------------------------------------------------------

  unixlike.c

  Macintosh-specific routines to emulate unixfunctions.

  ---------------------------------------------------------------------------*/

/*****************************************************************************/
/*  Includes                                                                 */
/*****************************************************************************/

#include "zip.h"

#include <string.h>
#include <stdio.h>
#include <sound.h>

#include "unixlike.h"
#include "helpers.h"
#include "pathname.h"
#include "macstuff.h"
#include "macglob.h"
#include "mactime.h"

/*****************************************************************************/
/*  Global Vars                                                              */
/*****************************************************************************/

extern MacZipGlobals    MacZip;
extern int errno;


/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/


/*****************************************************************************/
/*  Functions                                                                */
/*****************************************************************************/




/*
 *----------------------------------------------------------------------
 *
 * MacStat --
 *
 *  This function replaces the library version of stat.  The stat
 *  function provided by most Mac compiliers is rather broken and
 *  incomplete.
 *
 * Results:
 *  See stat documentation.
 *
 * Side effects:
 *  See stat documentation.
 *
 *----------------------------------------------------------------------
 */

int Zmacstat(const char *Fname, struct stat *buf)
{
    OSErr           err, rc;
    short           fullPathLength;
    Handle          hFullPath;
    char            path[NAME_MAX], path2[NAME_MAX];
    HVolumeParam    vpb;
    static unsigned long count_of_files = 0;

    AssertStr(Fname,Fname)
    Assert_it(buf,"","")

    UserStop();

    memset(buf, 0, sizeof(buf));    /* zero out all fields */

    RfDfFilen2Real(path2, Fname, MacZip.MacZipMode, MacZip.DataForkOnly,
                   &MacZip.CurrentFork);
    GetCompletePath(path, path2, &MacZip.fileSpec, &err);
    err = PrintUserHFSerr((err != -43) && (err != 0), err, path);
    printerr("GetCompletePath:", err, err, __LINE__, __FILE__, path);

    if (err != noErr) {
        errno = err;
        return -1;
    }

    /*  Collect here some more information, it's not related to Macstat.
        (note: filespec gets changed later in this function) */
    /* clear string-buffer */
    memset(MacZip.FullPath, 0x00, sizeof(MacZip.FullPath));
    rc = FSpGetFullPath(&MacZip.fileSpec, &fullPathLength, &hFullPath);
    strncpy(MacZip.FullPath, *hFullPath, fullPathLength);
    DisposeHandle(hFullPath);   /* we don't need it any more */
    /*  Collect some more information not related to Macstat */


    /*
     * Fill the fpb & vpb struct up with info about file or directory.
     */

    FSpGetDirectoryID(&MacZip.fileSpec, &MacZip.dirID, &MacZip.isDirectory);
    vpb.ioVRefNum = MacZip.fpb.hFileInfo.ioVRefNum =  MacZip.fileSpec.vRefNum;
    vpb.ioNamePtr = MacZip.fpb.hFileInfo.ioNamePtr =  MacZip.fileSpec.name;

    if (MacZip.isDirectory) {
        MacZip.fpb.hFileInfo.ioDirID =  MacZip.fileSpec.parID;
        /*
         * Directories are executable by everyone.
         */
        buf->st_mode |= UNX_IXUSR | UNX_IXGRP | UNX_IXOTH | UNX_IFDIR;
    } else {
        MacZip.fpb.hFileInfo.ioDirID = MacZip.dirID;
    }

    MacZip.fpb.hFileInfo.ioFDirIndex = 0;
    err = PBGetCatInfoSync((CInfoPBPtr)&MacZip.fpb);

    if (err == noErr) {
        vpb.ioVolIndex = 0;
        err = PBHGetVInfoSync((HParmBlkPtr)&vpb);
        if (err == noErr && buf != NULL) {
            /*
             * Files are always readable by everyone.
             */
            buf->st_mode |= UNX_IRUSR | UNX_IRGRP | UNX_IROTH;

            /*
             * Use the Volume Info & File Info to fill out stat buf.
             */
            if (MacZip.fpb.hFileInfo.ioFlAttrib & 0x10) {
                buf->st_mode |= UNX_IFDIR;
                buf->st_nlink = 2;
            } else {
                buf->st_nlink = 1;
                if (MacZip.fpb.hFileInfo.ioFlFndrInfo.fdFlags & 0x8000) {
                    buf->st_mode |= UNX_IFLNK;
                } else {
                    buf->st_mode |= UNX_IFREG;
                }
            }

            if (MacZip.fpb.hFileInfo.ioFlFndrInfo.fdType == 'APPL') {
                /*
                 * Applications are executable by everyone.
                 */
                buf->st_mode |= UNX_IXUSR | UNX_IXGRP | UNX_IXOTH;
            }
            if ((MacZip.fpb.hFileInfo.ioFlAttrib & 0x01) == 0){
                /*
                 * If not locked, then everyone has write acces.
                 */
                buf->st_mode |= UNX_IWUSR | UNX_IWGRP | UNX_IWOTH;
            }

            buf->st_ino = MacZip.fpb.hFileInfo.ioDirID;
            buf->st_dev = MacZip.fpb.hFileInfo.ioVRefNum;
            buf->st_uid = -1;
            buf->st_gid = -1;
            buf->st_rdev = 0;

            if (MacZip.CurrentFork == ResourceFork)
                buf->st_size = MacZip.fpb.hFileInfo.ioFlRLgLen;
            else
                buf->st_size = MacZip.fpb.hFileInfo.ioFlLgLen;

            buf->st_blksize = vpb.ioVAlBlkSiz;
            buf->st_blocks = (buf->st_size + buf->st_blksize - 1)
                            / buf->st_blksize;

            /*
             * The times returned by the Mac file system are in the
             * local time zone.  We convert them to GMT so that the
             * epoch starts from GMT.  This is also consistent with
             * what is returned from "clock seconds".
             */
            if (!MacZip.isDirectory) {
                MacZip.CreatDate  = MacZip.fpb.hFileInfo.ioFlCrDat;
                MacZip.ModDate    = MacZip.fpb.hFileInfo.ioFlMdDat;
                MacZip.BackDate   = MacZip.fpb.hFileInfo.ioFlBkDat;
            } else {
                MacZip.CreatDate  = MacZip.fpb.dirInfo.ioDrCrDat;
                MacZip.ModDate    = MacZip.fpb.dirInfo.ioDrMdDat;
                MacZip.BackDate   = MacZip.fpb.dirInfo.ioDrBkDat;
            }

#ifdef IZ_CHECK_TZ
            if (!zp_tz_is_valid)
            {
                MacZip.HaveGMToffset = false;
                MacZip.Md_UTCoffs = 0L;
                MacZip.Cr_UTCoffs = 0L;
                MacZip.Bk_UTCoffs = 0L;
            }
            else
#endif
            {
                /* Do not use GMT offsets when Md_UTCoffs calculation
                 * fails, since this time stamp is used for time
                 * comparisons in Zip and UnZip operations.
                 * We do not bother when GMT offset calculation fails for
                 * any other time stamp value. Instead we simply assume
                 * a default value of 0.
                 */
                MacZip.HaveGMToffset =
                    GetGMToffsetMac(MacZip.ModDate, &MacZip.Md_UTCoffs);
                if (MacZip.HaveGMToffset) {
                    GetGMToffsetMac(MacZip.CreatDate, &MacZip.Cr_UTCoffs);
                    GetGMToffsetMac(MacZip.BackDate, &MacZip.Bk_UTCoffs);
                } else {
                    MacZip.Cr_UTCoffs = 0L;
                    MacZip.Bk_UTCoffs = 0L;
                }
            }
#ifdef DEBUG_TIME
            {
            printf("\nZmacstat:  MacZip.HaveGMToffset: %d",
               MacZip.HaveGMToffset);
            printf("\nZmacstat:  Mac modif: %lu local -> UTOffset: %d",
              MacZip.ModDate, MacZip.Md_UTCoffs);
            printf("\nZmacstat:  Mac creat: %lu local -> UTOffset: %d",
              MacZip.CreatDate, MacZip.Cr_UTCoffs);
            printf("\nZmacstat:  Mac  back: %lu local -> UTOffset: %d",
              MacZip.BackDate, MacZip.Bk_UTCoffs);
            }
#endif /* DEBUG_TIME */


            buf->st_mtime = MacFtime2UnixFtime(MacZip.ModDate);
            buf->st_ctime = MacFtime2UnixFtime(MacZip.CreatDate);
            buf->st_atime = buf->st_mtime;

#ifdef DEBUG_TIME
            {
            printf("\nZmacstat:  Unix modif: %lu UTC; Mac: %lu local",
              buf->st_mtime, MacZip.ModDate);
            printf("\nZmacstat:  Unix creat: %lu UTC; Mac: %lu local\n",
              buf->st_ctime, MacZip.CreatDate);
            }
#endif /* DEBUG_TIME */

            if (noisy)
            {
                if (MacZip.StatingProgress)
                {
                    count_of_files++;
                    InformProgress(MacZip.RawCountOfItems, count_of_files );
                }
                else
                    count_of_files = 0;
            }
        }
    }

    if (err != noErr) {
        errno = err;
    }

    MacZip.isMacStatValid = true;
    return (err == noErr ? 0 : -1);
}





/*
 *----------------------------------------------------------------------
 *
 * chmod --
 *
 * Results:
 *  See chmod documentation.
 *
 * Side effects:
 *  See chmod documentation.
 *
 *----------------------------------------------------------------------
 */

int chmod(char *path, int mode)
{
    HParamBlockRec hpb;
    OSErr err;

    hpb.fileParam.ioNamePtr = C2PStr(path);
    hpb.fileParam.ioVRefNum = 0;
    hpb.fileParam.ioDirID = 0;

    if (mode & 0200) {
        err = PBHRstFLockSync(&hpb);
    } else {
        err = PBHSetFLockSync(&hpb);
    }

    if (err != noErr) {
        errno = err;
        return -1;
    }

    return 0;
}
