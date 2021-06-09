/*
  Copyright (c) 1990-2002 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2000-Apr-09 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/*---------------------------------------------------------------------------

  extrafld.c

  contains functions to build extra-fields.

  ---------------------------------------------------------------------------*/

/*****************************************************************************/
/*  Includes                                                                 */
/*****************************************************************************/

#include <sound.h>
#include "zip.h"
#include "unixlike.h"
#include "helpers.h"
#include "pathname.h"


/*****************************************************************************/
/*  Macros, typedefs                                                         */
/*****************************************************************************/

/* ---------------------------------------------------------------------- */
/* Add a 'MAC3' extra field to the zlist data pointed to by z.            */
/* This is the (new) Info-zip extra block for Macintosh */
#define EB_MAC3_HLEN          14    /* fixed length part of MAC3's header */
#define EB_L_MAC3_FINFO_LEN   52    /* fixed part of MAC3 compressible data */

#define EB_MAX_OF_VARDATA   1300    /* max possible datasize */

#define EB_L_MAC3_SIZE    (EB_HEADSIZE + EB_MAC3_HLEN)
#define EB_C_MAC3_SIZE    (EB_HEADSIZE + EB_MAC3_HLEN)

/* maximum memcompress overhead is the sum of the compression header length */
/* (6 = ush compression type, ulg CRC) and the worstcase deflate overhead   */
/* when uncompressible data are kept in 2 "stored" blocks (5 per block =    */
/* byte blocktype + 2 * ush blocklength) */
#define MEMCOMPRESS_OVERHEAD    (EB_MEMCMPR_HSIZ + EB_DEFLAT_EXTRA)

#define EB_M3_FL_COMPRESS   0x00
#define EB_M3_FL_DATFRK     0x01    /* data is data-fork */
#define EB_M3_FL_NOCHANGE   0x02    /* filename will be not changed */
#define EB_M3_FL_UNCMPR     0x04    /* data is 'natural' (not compressed) */
#define EB_M3_FL_TIME64     0x08    /* time is coded in 64 bit */
#define EB_M3_FL_NOUTC      0x10    /* only 'local' time-stamps are stored */


#define EB_L_UT_SIZE    (EB_HEADSIZE + EB_UT_LEN(2))
#define EB_C_UT_SIZE    (EB_HEADSIZE + EB_UT_LEN(1))

/* disable compressing of extra field
#define MAC_EXTRAFLD_UNCMPR       */

/* ---------------------------------------------------------------------- */
/* Add a 'JLEE' extra field to the zlist data pointed to by z.            */
/* This is the (old) Info-zip resource-fork extra block for Macintosh
(last Revision 1996-09-22) Layout made by Johnny Lee, Code made by me :-)  */
#define EB_L_JLEE_LEN  40       /* fixed length of JLEE's header */
#define EB_C_JLEE_LEN  40       /* fixed length of JLEE's header */

#define EB_L_JLEE_SIZE    (EB_HEADSIZE + EB_L_JLEE_LEN)
#define EB_C_JLEE_SIZE    (EB_HEADSIZE + EB_C_JLEE_LEN)



/*****************************************************************************/
/*  Global Vars                                                              */
/*****************************************************************************/

extern MacZipGlobals    MacZip;
extern unsigned long count_of_Zippedfiles;



/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/

static int add_UT_ef(struct zlist far *z, iztimes *z_utim);
static int add_JLEE_ef(struct zlist far *z);  /* old mac extra field */
static int add_MAC3_ef(struct zlist far *z);  /* new mac extra field */

static void make_extrafield_JLEE(char *l_ef);
static unsigned make_extrafield_MAC3(char *ef);
static char *make_EF_Head_MAC3(char *ef, unsigned compsize, ulg attrsize,
                               unsigned flag);

static void print_extra_info(void);
void UserStop(void);


/*****************************************************************************/
/*  Functions                                                                */
/*****************************************************************************/


/*
* Set the extra-field's for each compressed file
*/
int set_extra_field(struct zlist far *z, iztimes *z_utim)
  /* store full data in local header but just modification time stamp info
     in central header */
{
    int retval;

    Assert_it(z, "set_extra_field","")
    Assert_it(z_utim, "set_extra_field","")

    z_utim = z_utim;

    /* Check to make sure z is valid. */
    if( z == NULL ) {
        return ZE_LOGIC;
    }

    /* Resource forks are always binary */
    if (MacZip.CurrentFork == ResourceFork) z->att = BINARY;

    if (noisy)
        {
        count_of_Zippedfiles++;
        InformProgress(MacZip.RawCountOfItems, count_of_Zippedfiles );
        }

    /*
    PrintFileInfo();
    */
    switch (MacZip.MacZipMode)
        {
        case JohnnyLee_EF:
            {
            retval = add_JLEE_ef( z );
            if (retval != ZE_OK) return retval;
            break;
            }
        case NewZipMode_EF:
            {     /*  */
#ifdef USE_EF_UT_TIME
            retval = add_UT_ef(z, z_utim);
            if (retval != ZE_OK) return retval;
#endif

            retval = add_MAC3_ef( z );
            if (retval != ZE_OK) return retval;
            break;
            }
        default:
            {
            printerr("Unknown Extrafieldmode", -1, -1, __LINE__, __FILE__, "");
            return ZE_LOGIC;  /* function should never reach this point */
            }
    }

    /* MacStat information is now outdated and
       must be refreshed for the next file */
    MacZip.isMacStatValid = false;

    return ZE_OK;
}




#ifdef USE_EF_UT_TIME
/*
* Build and add the Unix time extra-field. This extra field
* will be included be default. Johnny Lee's implementation does
* not use this kind of extra-field.
* All datas are in Intel (=little-endian) format

 Extra field info:
   - 'UT' - UNIX time extra field

   This is done the same way ../unix/unix.c stores the 'UT'/'Ux' fields
   (full data in local header, only modification time in central header),
   with the 'M3' field added to the end and the size of the 'M3' field
   in the central header.
 */

static int add_UT_ef(struct zlist far *z, iztimes *z_utim)
{
    char        *l_ef = NULL;
    char        *c_ef = NULL;

    Assert_it(z, "add_UT_ef","")

#ifdef IZ_CHECK_TZ
    if (!zp_tz_is_valid)
        return ZE_OK;           /* skip silently if no valid TZ info */
#endif

    /* We can't work if there's no entry to work on. */
    if( z == NULL ) {
        return ZE_LOGIC;
    }

    /* Check to make sure we've got enough room in the extra fields. */
    if( z->ext + EB_L_UT_SIZE > EF_SIZE_MAX ||
        z->cext + EB_C_UT_SIZE > EF_SIZE_MAX ) {
        return ZE_MEM;
    }

    /* Allocate memory for the local and central extra fields. */
    if( z->extra && z->ext != 0 ) {
        l_ef = (char *)realloc( z->extra, z->ext + EB_L_UT_SIZE );
    } else {
        l_ef = (char *)malloc( EB_L_UT_SIZE );
        z->ext = 0;
    }
    if( l_ef == NULL ) {
        return ZE_MEM;
    }
    z->extra = l_ef;
    l_ef += z->ext;

    if( z->cextra && z->cext != 0 ) {
        c_ef = (char *)realloc( z->cextra, z->cext + EB_C_UT_SIZE );
    } else {
        c_ef = (char *)malloc( EB_C_UT_SIZE );
        z->cext = 0;
    }
    if( c_ef == NULL ) {
        return ZE_MEM;
    }
    z->cextra = c_ef;
    c_ef += z->cext;

    /* Now add the local version of the field. */
    *l_ef++ = 'U';
    *l_ef++ = 'T';
    *l_ef++ = (char)(EB_UT_LEN(2)); /* length of data in local EF */
    *l_ef++ = (char)0;
    *l_ef++ = (char)(EB_UT_FL_MTIME | EB_UT_FL_CTIME);
    *l_ef++ = (char)(z_utim->mtime);
    *l_ef++ = (char)(z_utim->mtime >> 8);
    *l_ef++ = (char)(z_utim->mtime >> 16);
    *l_ef++ = (char)(z_utim->mtime >> 24);
    *l_ef++ = (char)(z_utim->ctime);
    *l_ef++ = (char)(z_utim->ctime >> 8);
    *l_ef++ = (char)(z_utim->ctime >> 16);
    *l_ef++ = (char)(z_utim->ctime >> 24);

    z->ext += EB_L_UT_SIZE;

    /* Now add the central version. */
    memcpy(c_ef, l_ef-EB_L_UT_SIZE, EB_C_UT_SIZE);
    c_ef[EB_LEN] = (char)(EB_UT_LEN(1)); /* length of data in central EF */

    z->cext += EB_C_UT_SIZE;

    return ZE_OK;
}
#endif /* USE_EF_UT_TIME */


/*
* Build and add the old 'Johnny Lee' Mac extra field
* All native datas are in Motorola (=big-endian) format
*/

static int add_JLEE_ef( struct zlist far *z )
{
    char *l_ef       = NULL;
    char *c_ef       = NULL;

    Assert_it(z, "add_JLEE_ef","")

    /* Check to make sure we've got enough room in the extra fields. */
    if ( z->ext + EB_L_JLEE_SIZE > EF_SIZE_MAX ||
         z->cext + EB_C_JLEE_SIZE > EF_SIZE_MAX ) {
        return ZE_MEM;
    }


    /* Allocate memory for the local extra fields. */
    if ( z->extra && z->ext != 0 ) {
        l_ef = (char *)realloc( z->extra, z->ext + EB_L_JLEE_SIZE );
    } else {
        l_ef = (char *)malloc( EB_L_JLEE_SIZE );
        z->ext = 0;
    }
    if ( l_ef == NULL ) {
        return ZE_MEM;
    }
    z->extra = l_ef;
    l_ef += z->ext;

    /* Allocate memory for the  central extra fields. */
    if ( z->cextra && z->cext != 0 ) {
        c_ef = (char *)realloc( z->cextra, z->cext + EB_C_JLEE_SIZE );
    } else {
        c_ef = (char *)malloc( EB_C_JLEE_SIZE );
        z->cext = 0;
    }
    if ( c_ef == NULL ) {
        return ZE_MEM;
    }
    z->cextra = c_ef;
    c_ef += z->cext;


    if ( verbose ) {
        print_extra_info();
    }


    /**
    **
    **  Now add the local version of the field.
    **/
    make_extrafield_JLEE(l_ef);
    z->ext += EB_L_JLEE_SIZE;


    /**
    **
    **  Now add the central version of the field.
    **  It's identical to the local header. I wonder why ??
    *  the first two fields are in Intel little-endian format  */
    make_extrafield_JLEE(c_ef);
    z->cext += EB_C_JLEE_SIZE;

    return ZE_OK;
}



/*
* This is an implementation of Johnny Lee's extra field.
* I never saw Johnny Lee's code. My code is based on the extra-field
* definition mac (see latest appnote 1997-03-11)
* and on some experiments with Johnny Lee's Zip-app version 1.0, 1992
*
* Unfortunately I cannot agree with his extra-field layout.
*   - it wasted space
*   - and holds not all mac-specific information
*
* I coded this extra-field only for testing purposes.
* I don't want support this extra-field. Please use my implementation.
*
* This is old implementation of Johnny Lee's extra field.
* All native datas are in Motorola (=big-endian) format
*/

static void make_extrafield_JLEE(char *ef)
{

    Assert_it(ef, "make_extrafield_JLEE","")

    if (MacZip.isMacStatValid == false)
        {
        fprintf(stderr,"Internal Logic Error: [%d/%s] MacStat is out of sync !",
                        __LINE__,__FILE__);
        exit(-1);
        }


    /*  the first two fields are in Intel little-endian format  */
    *ef++ = 0xC8;                             /* tag for this extra block */
    *ef++ = 0x07;

    *ef++ = (char)(EB_L_JLEE_LEN);            /* total data size this block */
    *ef++ = (char)((EB_L_JLEE_LEN) >> 8);

     /*  the following fields are in motorola big-endian format  */
        *ef++ = 'J';                /* extra field signature:       4 Bytes */
        *ef++ = 'L';                /* the old style extra field */
        *ef++ = 'E';
        *ef++ = 'E';

                /* Start Macintosh Finder FInfo structure   16 Bytes overall */
                                    /* Type:                        4 Bytes */
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlFndrInfo.fdType >> 24);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlFndrInfo.fdType >> 16);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlFndrInfo.fdType >> 8);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlFndrInfo.fdType);

                                    /* Creator:                     4 Bytes */
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlFndrInfo.fdCreator >> 24);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlFndrInfo.fdCreator >> 16);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlFndrInfo.fdCreator >> 8);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlFndrInfo.fdCreator);

                                     /* file Finder Flags:                2 Bytes */
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlFndrInfo.fdFlags >> 8);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlFndrInfo.fdFlags);

                                     /* Finders Icon position of a file*/
                                     /* V/Y-Position:                2 Bytes */
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlFndrInfo.fdLocation.v >> 8);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlFndrInfo.fdLocation.v);
                                     /* H/X-Position:                2 Bytes */
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlFndrInfo.fdLocation.h >> 8);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlFndrInfo.fdLocation.h);

                               /* fdFldr Folder containing file    2 Bytes */
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlFndrInfo.fdFldr >> 8);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlFndrInfo.fdFldr);
                /* End Macintosh Finder FInfo structure */


                                                  /* Creation-time  4 Bytes */
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlCrDat >> 24);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlCrDat >> 16);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlCrDat >> 8);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlCrDat);

                                              /* Modification-time  4 Bytes */
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlMdDat >> 24);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlMdDat >> 16);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlMdDat >> 8);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlMdDat);

                                     /* info Bits                    4 Bytes */
    *ef++ = 0x00;
    *ef++ = 0x00;
    *ef++ = 0x00;
    if (MacZip.DataForkOnly)
        {          /* don't convert filename for unzipping */
                   /* 0x01 = data-fork; 0x00 = resource-fork */
        *ef++ = (char) (MacZip.CurrentFork == DataFork) | 2;
        }
    else
        {
        *ef++ = (char) (MacZip.CurrentFork == DataFork);
        }

                                  /* file's location folder ID  4 Bytes */
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlParID >> 24);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlParID >> 16);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlParID >> 8);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlParID);
                                                          /* ============  */
                                                          /*     40 Bytes  */
}



/*
* Build and add the new mac extra field
* All native data are stored in Intel (=little-endian) format
*/

static int add_MAC3_ef( struct zlist far *z )
{
    char *l_ef        = NULL;
    char *c_ef        = NULL;
    char *attrbuff    = NULL;
    off_t attrsize    = (EB_L_MAC3_FINFO_LEN + EB_MAX_OF_VARDATA);
    char *compbuff    = NULL;
    unsigned compsize = 0;
    unsigned m3_compr;
    Boolean compress_data = true;

    Assert_it(z, "add_MAC3_ef","")

    UserStop();  /* do event handling and let the user stop */

    if( verbose ) {
        print_extra_info();
    }

    /* allocate temporary buffer to collect the Mac extra field info */
    attrbuff = (char *)malloc( (size_t)attrsize );
    if( attrbuff == NULL ) {
        return ZE_MEM;
    }

    /* fill the attribute buffer, to get its (uncompressed) size */
    attrsize = make_extrafield_MAC3(attrbuff);

    if (compress_data &&
        ((compbuff = (char *)malloc((size_t)attrsize + MEMCOMPRESS_OVERHEAD))
         != NULL))
        {
        /* Try compressing the data */
        compsize = memcompress( compbuff,
                                (size_t)attrsize + MEMCOMPRESS_OVERHEAD,
                                attrbuff,
                                (size_t)attrsize );
#ifdef MAC_EXTRAFLD_UNCMPR
        compsize = attrsize;
#endif
        }
    else
        {
        compsize = attrsize;
        }

    if ((compsize) < attrsize) {
        /* compression gained some space ... */
        free(attrbuff);         /* no longer needed ... */
        m3_compr = EB_M3_FL_COMPRESS;
    } else {
        /* compression does not help, store data in uncompressed mode */
        if (compbuff != NULL) free(compbuff);
        compbuff = attrbuff;
        compsize = attrsize;
        m3_compr = EB_M3_FL_UNCMPR;
    }

    /* Check to make sure we've got enough room in the extra fields. */
    if( z->ext + (EB_L_MAC3_SIZE + compsize) > EF_SIZE_MAX ||
        z->cext + EB_C_MAC3_SIZE > EF_SIZE_MAX ) {
        if (compbuff != NULL) free(compbuff);
        return ZE_MEM;
    }

    /* Allocate memory for the local extra fields. */
    if( z->extra && z->ext != 0 ) {
        l_ef = (char *)realloc( z->extra, z->ext +
                                EB_L_MAC3_SIZE + compsize);
    } else {
        l_ef = (char *)malloc( EB_L_MAC3_SIZE + compsize);
        z->ext = 0;
    }
    if( l_ef == NULL ) {
        return ZE_MEM;
    }
    z->extra = l_ef;
    l_ef += z->ext;

    /* Allocate memory for the  central extra fields. */
    if( z->cextra && z->cext != 0 ) {
        c_ef = (char *)realloc( z->cextra, z->cext + EB_C_MAC3_SIZE);
    } else {
        c_ef = (char *)malloc( EB_C_MAC3_SIZE );
        z->cext = 0;
    }
    if( c_ef == NULL ) {
        return ZE_MEM;
    }
    z->cextra = c_ef;
    c_ef += z->cext;

    /**
    **  Now add the local version of the field.
    **/
    l_ef = make_EF_Head_MAC3(l_ef, compsize, (ulg)attrsize, m3_compr);
    memcpy(l_ef, compbuff, (size_t)compsize);
    l_ef += compsize;
    z->ext += EB_L_MAC3_SIZE + compsize;
    free(compbuff);
    /* And the central version. */
    c_ef = make_EF_Head_MAC3(c_ef, 0, (ulg)attrsize, m3_compr);
    z->cext += EB_C_MAC3_SIZE;

    return ZE_OK;
}




/*
* Build the new mac local extra field header.
* It's identical with the central extra field.
* All native data are in Intel (=little-endian) format
*/
static char *make_EF_Head_MAC3(char *ef, unsigned compsize, ulg attrsize,
                               unsigned flag)
{
    unsigned info_flag = flag;

    Assert_it(ef, "make_EF_Head_MAC3","")

    /*  the first four fields are in Intel little-endian format  */
    *ef++ = 'M';         /* tag for this extra block               2 Bytes */
    *ef++ = '3';

                                  /* total data size this block    2 Bytes */
    *ef++ = (char) (EB_MAC3_HLEN + compsize);
    *ef++ = (char)((EB_MAC3_HLEN + compsize) >> 8);

    *ef++ = (char)(attrsize);
    *ef++ = (char)(attrsize >> 8);
    *ef++ = (char)(attrsize >> 16);
    *ef++ = (char)(attrsize >> 24);

                                   /* info Bits  (flags)           2 Bytes */

    if (MacZip.DataForkOnly)            info_flag |= (EB_M3_FL_DATFRK |
                                                      EB_M3_FL_NOCHANGE);
    if (MacZip.CurrentFork == DataFork) info_flag |= EB_M3_FL_DATFRK;
    if (!MacZip.HaveGMToffset)          info_flag |= EB_M3_FL_NOUTC;

    *ef++ = (char)info_flag;
    *ef++ = (char)0x00;            /* reserved at the moment */

 /* Note: Apple defined File-Type/-Creator as OSType ( =unsigned long,
          see Universal Headers 3.1). However, File-Type/-Creator are a
          unique four-character sequence. Therefore the byteorder of the
          File-Type/-Creator are NOT changed. The native format is used.  */

                                                       /* Type:    4 Bytes */
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlFndrInfo.fdType >> 24);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlFndrInfo.fdType >> 16);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlFndrInfo.fdType >> 8);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlFndrInfo.fdType);

                                                       /* Creator: 4 Bytes */
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlFndrInfo.fdCreator >> 24);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlFndrInfo.fdCreator >> 16);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlFndrInfo.fdCreator >> 8);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlFndrInfo.fdCreator);

    return ef;
}





/*
* Build the new mac local extra field header.
* All native data are in Intel (=little-endian) format
*/
unsigned make_extrafield_MAC3(char *ef)
{
    char *ef_m3_begin = ef;
    char *temp_Pathname;
    char tmp_buffer[NAME_MAX];
    unsigned char comment[257];
    unsigned short FLength = 0;
    unsigned short CLength = 0;
    short   tempFork;
    OSErr       err;

    Assert_it(ef, "make_extrafield_MAC3","")

    if (MacZip.isMacStatValid == false)
    {
        fprintf(stderr,
                "Internal Logic Error: [%d/%s] MacStat is out of sync !",
                __LINE__, __FILE__);
        exit(-1);
    }

    /* Start Macintosh Finder FInfo structure except Type/Creator
       (see make_EF_Head_MAC3()) 8 Bytes overall */

                                             /* file Finder Flags: 2 Bytes */
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlFndrInfo.fdFlags);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlFndrInfo.fdFlags >> 8);

                                    /* Finders Icon position of a file*/
                                   /* V/Y-Position:                2 Bytes */
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlFndrInfo.fdLocation.v);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlFndrInfo.fdLocation.v >> 8);

                                   /* H/X-Position:                2 Bytes */
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlFndrInfo.fdLocation.h);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlFndrInfo.fdLocation.h >> 8);

                               /* fdFldr Folder containing file    2 Bytes */
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlFndrInfo.fdFldr);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlFndrInfo.fdFldr >> 8);

                /* End Macintosh Finder FInfo structure */

                                                    /*    8 Bytes so far ... */

                /* Start Macintosh Finder FXInfo structure  16 Bytes overall */
                                                    /* Icon ID:    2 Bytes */
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlXFndrInfo.fdIconID);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlXFndrInfo.fdIconID >> 8);

                                                        /* unused: 6 Bytes */
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlXFndrInfo.fdUnused[0]);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlXFndrInfo.fdUnused[0] >> 8);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlXFndrInfo.fdUnused[1]);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlXFndrInfo.fdUnused[1] >> 8);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlXFndrInfo.fdUnused[2]);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlXFndrInfo.fdUnused[2] >> 8);
                                                   /* Script flag: 1 Byte */
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlXFndrInfo.fdScript);
                                                /* More flag bits: 1 Byte */
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlXFndrInfo.fdXFlags);
                                                /* Comment ID      2 Bytes */
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlXFndrInfo.fdComment);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlXFndrInfo.fdComment >> 8);

                                                   /* Home Dir ID: 4 Bytes */
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlXFndrInfo.fdPutAway);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlXFndrInfo.fdPutAway >> 8);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlXFndrInfo.fdPutAway >> 16);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlXFndrInfo.fdPutAway >> 24);
                /* End Macintosh Finder FXInfo structure */

                                                    /*   24 Bytes so far ... */

                                            /* file version number 1 Byte */
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFVersNum);

                                        /* directory access rights 1 Byte */
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioACUser);

                                                 /* Creation-time  4 Bytes */
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlCrDat);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlCrDat  >> 8);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlCrDat >> 16);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlCrDat >> 24);
                                             /* Modification-time  4 Bytes */
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlMdDat);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlMdDat >> 8);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlMdDat >> 16);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlMdDat >> 24);
                                                   /* Backup-time  4 Bytes */
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlBkDat);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlBkDat >> 8);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlBkDat >> 16);
    *ef++ = (char)(MacZip.fpb.hFileInfo.ioFlBkDat >> 24);

                                                    /*   38 Bytes so far ... */
#ifdef USE_EF_UT_TIME
    if (MacZip.HaveGMToffset) {
                                              /* GMT-Offset times  12 Bytes */
        *ef++ = (char)(MacZip.Cr_UTCoffs);
        *ef++ = (char)(MacZip.Cr_UTCoffs >> 8);
        *ef++ = (char)(MacZip.Cr_UTCoffs >> 16);
        *ef++ = (char)(MacZip.Cr_UTCoffs >> 24);
        *ef++ = (char)(MacZip.Md_UTCoffs);
        *ef++ = (char)(MacZip.Md_UTCoffs >> 8);
        *ef++ = (char)(MacZip.Md_UTCoffs >> 16);
        *ef++ = (char)(MacZip.Md_UTCoffs >> 24);
        *ef++ = (char)(MacZip.Bk_UTCoffs);
        *ef++ = (char)(MacZip.Bk_UTCoffs >> 8);
        *ef++ = (char)(MacZip.Bk_UTCoffs >> 16);
        *ef++ = (char)(MacZip.Bk_UTCoffs >> 24);
    }
                                                    /*   50 Bytes so far ... */
#endif

                                  /* Text Encoding Base (charset)  2 Bytes */
    *ef++ = (char)(MacZip.CurrTextEncodingBase);
    *ef++ = (char)(MacZip.CurrTextEncodingBase >> 8);
                                                    /*   52 Bytes so far ... */

    /* MacZip.CurrentFork will be changed, so we have to save it  */
    tempFork = MacZip.CurrentFork;
    if (!MacZip.StoreFullPath)  {
        temp_Pathname = StripPartialDir(tmp_buffer, MacZip.SearchDir,
                                        MacZip.FullPath);
    } else {
        temp_Pathname = MacZip.FullPath;
    }
    MacZip.CurrentFork = tempFork;

    FLength = strlen(temp_Pathname) + 1;
    memcpy( ef, temp_Pathname, (size_t)FLength );
    ef += FLength;          /* make room for the string -  variable length */

    err = FSpLocationFromFullPath(strlen(MacZip.FullPath), MacZip.FullPath,
                                  &MacZip.fileSpec);
    printerr("FSpLocationFromFullPath:", err, err,
             __LINE__, __FILE__, tmp_buffer);

    err = FSpDTGetComment(&MacZip.fileSpec, comment);
    printerr("FSpDTGetComment:", (err != -5012) && (err != 0), err,
             __LINE__, __FILE__, "");
    PToCCpy(comment,tmp_buffer);

    CLength = strlen(tmp_buffer) + 1;
    memcpy( ef, tmp_buffer, (size_t)CLength );
    ef += CLength;          /* make room for the string -  variable length */

    if (verbose) printf("\n comment: [%s]", tmp_buffer);

    return (unsigned)(ef - ef_m3_begin);
}






/*
* Print all native data of the new mac local extra field.
* It's for debugging purposes and disabled by default.
*/

static void PrintFileInfo(void)
{
DateTimeRec  MacTime;

printf("\n\n---------------------------------------------"\
       "----------------------------------");
printf("\n FullPath Name =                   [%s]",  MacZip.FullPath);
printf("\n File Attributes =                  %s  0x%x  %d",
          sBit2Str(MacZip.fpb.hFileInfo.ioFlAttrib),
          MacZip.fpb.hFileInfo.ioFlAttrib,
          MacZip.fpb.hFileInfo.ioFlAttrib);
printf("\n Enclosing Folder ID# =             0x%x  %d",
          MacZip.fpb.hFileInfo.ioFlParID,
          MacZip.fpb.hFileInfo.ioFlParID);

if (!MacZip.isDirectory)
{
printf("\n File Type =                        [%c%c%c%c]  0x%lx",
    MacZip.fpb.hFileInfo.ioFlFndrInfo.fdType >> 24,
    MacZip.fpb.hFileInfo.ioFlFndrInfo.fdType >> 16,
    MacZip.fpb.hFileInfo.ioFlFndrInfo.fdType >> 8,
    MacZip.fpb.hFileInfo.ioFlFndrInfo.fdType,
    MacZip.fpb.hFileInfo.ioFlFndrInfo.fdType);

printf("\n File Creator =                     [%c%c%c%c]  0x%lx",
    MacZip.fpb.hFileInfo.ioFlFndrInfo.fdCreator >> 24,
    MacZip.fpb.hFileInfo.ioFlFndrInfo.fdCreator >> 16,
    MacZip.fpb.hFileInfo.ioFlFndrInfo.fdCreator >> 8,
    MacZip.fpb.hFileInfo.ioFlFndrInfo.fdCreator,
    MacZip.fpb.hFileInfo.ioFlFndrInfo.fdCreator);

printf("\n Data Fork :" );
printf("\n    Actual (Logical) Length =       %d  0x%x ",
              MacZip.fpb.hFileInfo.ioFlLgLen,
              MacZip.fpb.hFileInfo.ioFlLgLen);
printf("\n    Allocated (Physical) Length =   %d  0x%x",
              MacZip.fpb.hFileInfo.ioFlPyLen,
              MacZip.fpb.hFileInfo.ioFlPyLen);
printf("\n Resource Fork :" );
printf("\n    Actual (Logical) Length =       %d  0x%x",
              MacZip.fpb.hFileInfo.ioFlRLgLen,
              MacZip.fpb.hFileInfo.ioFlRLgLen );
printf("\n    Allocated (Physical) Length =   %d  0x%x",
              MacZip.fpb.hFileInfo.ioFlRPyLen,
              MacZip.fpb.hFileInfo.ioFlRPyLen );
}

printf("\n Dates :               ");

SecondsToDate (MacZip.CreatDate, &MacTime);
printf("\n    Created  =                       %4d/%2d/%2d %2d:%2d:%2d  ",
    MacTime.year,
    MacTime.month,
    MacTime.day,
    MacTime.hour,
    MacTime.minute,
    MacTime.second);

SecondsToDate (MacZip.BackDate, &MacTime);
printf("\n    Backup   =                       %4d/%2d/%2d %2d:%2d:%2d  ",
    MacTime.year,
    MacTime.month,
    MacTime.day,
    MacTime.hour,
    MacTime.minute,
    MacTime.second);

SecondsToDate (MacZip.ModDate, &MacTime);
printf("\n    Modified =                       %4d/%2d/%2d %2d:%2d:%2d  ",
    MacTime.year,
    MacTime.month,
    MacTime.day,
    MacTime.hour,
    MacTime.minute,
    MacTime.second);

if (!MacZip.isDirectory)
{
printf("\n Finder Flags :                     %s  0x%x  %d",
          sBit2Str(MacZip.fpb.hFileInfo.ioFlFndrInfo.fdFlags),
          MacZip.fpb.hFileInfo.ioFlFndrInfo.fdFlags,
          MacZip.fpb.hFileInfo.ioFlFndrInfo.fdFlags);
printf("\n Finder Icon Position =          X: %d  0x%x ",
          MacZip.fpb.hFileInfo.ioFlFndrInfo.fdLocation.h,
          MacZip.fpb.hFileInfo.ioFlFndrInfo.fdLocation.h);
printf("\n                                  Y: %d  0x%x ",
          MacZip.fpb.hFileInfo.ioFlFndrInfo.fdLocation.v,
          MacZip.fpb.hFileInfo.ioFlFndrInfo.fdLocation.v);
}
else
{
printf("\n Finder Flags :                     %s  0x%x  %d",
          sBit2Str(MacZip.fpb.dirInfo.ioDrUsrWds.frFlags),
          MacZip.fpb.dirInfo.ioDrUsrWds.frFlags,
          MacZip.fpb.dirInfo.ioDrUsrWds.frFlags);
printf("\n Finder Icon Position =          X: %d  0x%x ",
          MacZip.fpb.dirInfo.ioDrUsrWds.frLocation.h,
          MacZip.fpb.dirInfo.ioDrUsrWds.frLocation.h);
printf("\n                                  Y: %d  0x%x ",
          MacZip.fpb.dirInfo.ioDrUsrWds.frLocation.v,
          MacZip.fpb.dirInfo.ioDrUsrWds.frLocation.v);
}

printf("\n----------------------------------------------------"\
        "---------------------------\n");
}



/*
* If the switch '-v' is used, print some more info.
*/

static void print_extra_info(void)
{
char Fork[20];

if (MacZip.CurrentFork == DataFork) sstrcpy(Fork,"<DataFork>");
else  sstrcpy(Fork,"<ResourceFork>");

printf("\n%16s [%c%c%c%c]  [%c%c%c%c]",Fork,
    MacZip.fpb.hFileInfo.ioFlFndrInfo.fdType >> 24,
    MacZip.fpb.hFileInfo.ioFlFndrInfo.fdType >> 16,
    MacZip.fpb.hFileInfo.ioFlFndrInfo.fdType >> 8,
    MacZip.fpb.hFileInfo.ioFlFndrInfo.fdType,

    MacZip.fpb.hFileInfo.ioFlFndrInfo.fdCreator >> 24,
    MacZip.fpb.hFileInfo.ioFlFndrInfo.fdCreator >> 16,
    MacZip.fpb.hFileInfo.ioFlFndrInfo.fdCreator >> 8,
    MacZip.fpb.hFileInfo.ioFlFndrInfo.fdCreator);
}
