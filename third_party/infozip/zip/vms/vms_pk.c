/*
  Copyright (c) 1990-2007 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2007-Mar-4 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/*
 *    vms_pk.c  by Igor Mandrichenko
 *
 *    version 2.0       20-Mar-1993
 *                      Generates PKWARE version of VMS attributes
 *                      extra field according to appnote 2.0.
 *                      Uses low level QIO-ACP interface.
 *    version 2.0-1     10-Apr-1993
 *                      Save ACLs
 *    version 2.1       24-Aug-1993
 *                      By default produce 0x010C extra record ID instead of
 *                      PKWARE's 0x000C. The format is mostly compatible with
 *                      PKWARE.
 *                      Incompatibility (?): zip produces multiple ACE
 *                      fields.
 *    version 2.1-1     Clean extra fields in vms_get_attributes().
 *                      Fixed bug with EOF.
 *    version 2.1-2     15-Sep-1995, Chr. Spieler
 *                      Removed extra fields cleanup from vms_get_attributes().
 *                      This is now done in zipup.c
 *                      Modified (according to UnZip's vms.[ch]) the fib stuff
 *                      for DEC C (AXP,VAX) support.
 *    version 2.2       28-Sep-1995, Chr. Spieler
 *                      Reorganized code for easier maintance of the two
 *                      incompatible flavours (IM style and PK style) VMS
 *                      attribute support.  Generic functions (common to
 *                      both flavours) are now collected in a `wrapper'
 *                      source file that includes one of the VMS attribute
 *                      handlers.
 *                      Made extra block header conforming to PKware's
 *                      specification (extra block header has a length
 *                      of four bytes, two bytes for a signature, and two
 *                      bytes for the length of the block excluding this
 *                      header.
 *    version 2.2-1     19-Oct-1995, Chr. Spieler
 *                      Fixed bug in CRC calculation.
 *                      Use official PK VMS extra field id.
 *    version 2.2-2     21-Nov-1997, Chr. Spieler
 *                      Fixed bug in vms_get_attributes() for directory
 *                      entries (access to uninitialized ioctx record).
 *                      Removed unused second arg for vms_open().
 *    version 2.2-3     04-Apr-1999, Chr. Spieler
 *                      Changed calling interface of vms_get_attributes()
 *                      to accept a void pointer as first argument.
 *    version 2.2-4     26-Jan-2002, Chr. Spieler
 *                      Modified vms_read() to handle files larger than 2GByte
 *                      (up to size limit of "unsigned long", resp. 4GByte).
 *    version 3.0       20-Oct-2004, Steven Schweda.
 *                      Changed vms_read() to read all the allocated
 *                      blocks in a file, for sure.  Changed the default
 *                      chunk size from 16K to 32K.  Changed to use the
 *                      new typedef for the ioctx structure.  Moved the
 *                      VMS_PK_EXTRA test into here from VMS.C to allow
 *                      more general automatic dependency generation.
 *                      08-Feb-2005, SMS.
 *                      Changed to accomodate ODS5 extended file names:
 *                      NAM structure -> NAM[L], and so on.  (VMS.H.)
 *                      Added some should-never-appear error messages in
 *                      vms_open().
 */

#ifdef VMS                      /* For VMS only ! */

#ifdef VMS_PK_EXTRA

#include <ssdef.h>

#ifndef VMS_ZIP
#define VMS_ZIP
#endif

#include "crc32.h"
#include "vms.h"
#include "vmsdefs.h"

#ifndef ERR
#define ERR(x) (((x)&1)==0)
#endif

#ifndef NULL
#define NULL (void*)(0L)
#endif

#ifndef UTIL

static PK_info_t PK_def_info =
{
        ATR$C_RECATTR,  ATR$S_RECATTR,  {0},
        ATR$C_UCHAR,    ATR$S_UCHAR,    {0},
        ATR$C_CREDATE,  ATR$S_CREDATE,  {0},
        ATR$C_REVDATE,  ATR$S_REVDATE,  {0},
        ATR$C_EXPDATE,  ATR$S_EXPDATE,  {0},
        ATR$C_BAKDATE,  ATR$S_BAKDATE,  {0},
        ATR$C_ASCDATES, sizeof(ush),    0,
        ATR$C_UIC,      ATR$S_UIC,      {0},
        ATR$C_FPRO,     ATR$S_FPRO,     {0},
        ATR$C_RPRO,     ATR$S_RPRO,     {0},
        ATR$C_JOURNAL,  ATR$S_JOURNAL,  {0}
};

/* File description structure for Zip low level I/O */
typedef struct
{
    struct iosb         iosb;
    long                vbn;
    uzoff_t             size;
    uzoff_t             rest;
    int                 status;
    ush                 chan;
    ush                 chan_pad;       /* alignment member */
    long                acllen;
    uch                 aclbuf[ATR$S_READACL];
    PK_info_t           PKi;
} ioctx_t;


/* Forward declarations of public functions: */
ioctx_t *vms_open(char *file);
unsigned int  vms_read(register ioctx_t *ctx,
                       register char *buf, register unsigned int size);
int  vms_error(ioctx_t *ctx);
int  vms_rewind(ioctx_t *ctx);
int  vms_get_attributes(ioctx_t *ctx, struct zlist far *z,
                        iztimes *z_utim);
int  vms_close(ioctx_t *ctx);


#define BLOCK_BYTES 512


/*---------------*
 |  vms_open()   |
 *---------------*
 |  This routine opens file for reading fetching its attributes.
 |  Returns pointer to file description structure.
 */

ioctx_t *vms_open(file)
char *file;
{
    static struct atrdef        Atr[VMS_MAX_ATRCNT+1];
    static struct NAM_STRUCT    Nam;
    static struct fibdef        Fib;
    static struct dsc$descriptor FibDesc =
        {sizeof(Fib),DSC$K_DTYPE_Z,DSC$K_CLASS_S,(char *)&Fib};
    static struct dsc$descriptor_s DevDesc =
        {0,DSC$K_DTYPE_T,DSC$K_CLASS_S,&Nam.NAM_DVI[1]};
    static char EName[NAM_MAXRSS];
    static char RName[NAM_MAXRSS];

    struct FAB Fab;
    register ioctx_t *ctx;
    register struct fatdef *fat;
    int status;
    int i;
    ulg efblk;
    ulg hiblk;

    if ( (ctx=(ioctx_t *)malloc(sizeof(ioctx_t))) == NULL )
        return NULL;
    ctx -> PKi = PK_def_info;

#define FILL_REQ(ix,id,b)   {       \
    Atr[ix].atr$l_addr = GVTC &(b);      \
    Atr[ix].atr$w_type = (id);      \
    Atr[ix].atr$w_size = sizeof(b); \
}

    FILL_REQ(0, ATR$C_RECATTR,  ctx->PKi.ra);
    FILL_REQ(1, ATR$C_UCHAR,    ctx->PKi.uc);
    FILL_REQ(2, ATR$C_REVDATE,  ctx->PKi.rd);
    FILL_REQ(3, ATR$C_EXPDATE,  ctx->PKi.ed);
    FILL_REQ(4, ATR$C_CREDATE,  ctx->PKi.cd);
    FILL_REQ(5, ATR$C_BAKDATE,  ctx->PKi.bd);
    FILL_REQ(6, ATR$C_ASCDATES, ctx->PKi.rn);
    FILL_REQ(7, ATR$C_JOURNAL,  ctx->PKi.jr);
    FILL_REQ(8, ATR$C_RPRO,     ctx->PKi.rp);
    FILL_REQ(9, ATR$C_FPRO,     ctx->PKi.fp);
    FILL_REQ(10,ATR$C_UIC,      ctx->PKi.ui);
    FILL_REQ(11,ATR$C_ACLLENGTH,ctx->acllen);
    FILL_REQ(12,ATR$C_READACL,  ctx->aclbuf);

    Atr[13].atr$w_type = 0;     /* End of ATR list */
    Atr[13].atr$w_size = 0;
    Atr[13].atr$l_addr = GVTC NULL;

    /* Initialize RMS structures.  We need a NAM[L] to retrieve the FID. */
    Fab = cc$rms_fab;
    Nam = CC_RMS_NAM;
    Fab.FAB_NAM = &Nam; /* FAB has an associated NAM[L]. */

#ifdef NAML$C_MAXRSS

    Fab.fab$l_dna =(char *) -1;         /* Using NAML for default name. */
    Fab.fab$l_fna = (char *) -1;        /* Using NAML for file name. */

#endif /* def NAML$C_MAXRSS */

    FAB_OR_NAML( Fab, Nam).FAB_OR_NAML_FNA = file ;     /* File name. */
    FAB_OR_NAML( Fab, Nam).FAB_OR_NAML_FNS = strlen(file);
    Nam.NAM_ESA = EName; /* expanded filename */
    Nam.NAM_ESS = sizeof(EName);
    Nam.NAM_RSA = RName; /* resultant filename */
    Nam.NAM_RSS = sizeof(RName);

    /* Do $PARSE and $SEARCH here. */
    status = sys$parse(&Fab);

    if (!(status & 1))
    {
        fprintf( stderr,
         " vms_open(): $parse sts = %%x%08x.\n", status);
        return NULL;
    }

#ifdef NAML$M_OPEN_SPECIAL
    /* 2007-02-28 SMS.
     * If processing symlinks as symlinks ("-y"), then $SEARCH for the
     * link, not the target file.
     */
    if (linkput)
    {
        Nam.naml$v_open_special = 1;
    }
#endif /* def NAML$M_OPEN_SPECIAL */

    /* Search for the first file.  If none, signal error. */
    status = sys$search(&Fab);

    if (!(status & 1))
    {
        fprintf( stderr,
         " vms_open(): $search sts = %%x%08x.\n", status);
        return NULL;
    }

    /* Initialize Device name length.  Note that this points into the
       NAM[L] to get the device name filled in by the $PARSE, $SEARCH
       services.
    */
    DevDesc.dsc$w_length = Nam.NAM_DVI[0];

    status = sys$assign(&DevDesc,&ctx->chan,0,0);

    if (!(status & 1))
    {
        fprintf( stderr,
         " vms_open(): $assign sts = %%x%08x.\n", status);
        return NULL;
    }

    /* Move the FID (and not the DID) into the FIB.
       2005=02-08 SMS.
       Note that only the FID is needed, not the DID, and not the file
       name.  Setting these other items causes failures on ODS5.
    */
    Fib.FIB$L_ACCTL = FIB$M_NOWRITE;

    for (i = 0; i < 3; i++)
    {
        Fib.FIB$W_FID[ i] = Nam.NAM_FID[ i];
        Fib.FIB$W_DID[ i] = 0;
    }

    /* Use the IO$_ACCESS function to return info about the file. */
    status = sys$qiow( 0, ctx->chan,
     (IO$_ACCESS| IO$M_ACCESS), &ctx->iosb, 0, 0,
     &FibDesc, 0, 0, 0, Atr, 0);

    if (ERR(status) || ERR(status = ctx->iosb.status))
    {
        vms_close(ctx);
        fprintf( stderr,
         " vms_open(): $qiow (access) sts = %%x%08x, iosb sts = %%x%08x.\n",
         status, ctx->iosb.status);
        return NULL;
    }

    fat = (struct fatdef *)&(ctx -> PKi.ra);

#define SWAPW(x)        ( (((x)>>16)&0xFFFF) + ((x)<<16) )

    efblk = SWAPW(fat->fat$l_efblk);
    hiblk = SWAPW(fat->fat$l_hiblk);

    if (efblk == 0)
    {
        /* Only known size is all allocated blocks.
           (This occurs with a zero-length file, for example.)
        */
        ctx -> size =
        ctx -> rest = ((uzoff_t) hiblk)* BLOCK_BYTES;
    }
    else
    {
        /* Store normal (used) size in ->size.
           If only one -V, store normal (used) size in ->rest.
           If multiple -V, store allocated-blocks size in ->rest.
        */
        ctx -> size =
         (((uzoff_t) efblk)- 1)* BLOCK_BYTES+ fat -> fat$w_ffbyte;

        if (vms_native < 2)
            ctx -> rest = ctx -> size;
        else
            ctx -> rest = ((uzoff_t) hiblk)* BLOCK_BYTES;
    }

    ctx -> status = SS$_NORMAL;
    ctx -> vbn = 1;
    return ctx;
}


#define KByte (2* BLOCK_BYTES)
#define MAX_READ_BYTES (32* KByte)

/*----------------*
 |   vms_read()   |
 *----------------*
 |   Reads file in (multi-)block-sized chunks into the buffer.
 |   Stops on EOF. Returns number of bytes actually read.
 |   Note: This function makes no sense (and will error) if the buffer
 |   size ("size") is not a multiple of the disk block size (512).
 */

size_t vms_read( ctx, buf, size)
ioctx_t *ctx;
char *buf;
size_t size;
{
    int act_cnt;
    uzoff_t rest_rndup;
    int status;
    size_t bytes_read = 0;

    /* If previous read hit EOF, fail early. */
    if (ctx -> status == SS$_ENDOFFILE)
        return 0;               /* EOF. */

    /* If no more expected to be read, fail early. */
    if (ctx -> rest == 0)
        return 0;               /* Effective EOF. */

    /* If request is smaller than a whole block, fail.
       This really should never happen.  (assert()?)
    */
    if (size < BLOCK_BYTES)
        return 0;

    /* Note that on old VMS VAX versions (like V5.5-2), QIO[W] may fail
       with status %x0000034c (= %SYSTEM-F-IVBUFLEN, invalid buffer
       length) when size is not a multiple of 512.  Thus the requested
       size is boosted as needed, but the IOSB byte count returned is
       reduced when it exceeds the actual bytes remaining (->rest).
    */

    /* Adjust request size as appropriate. */
    if (size > MAX_READ_BYTES)
    {
        /* Restrict request to MAX_READ_BYTES. */
        size = MAX_READ_BYTES;
    }
    else
    {
        /* Round odd-ball request up to the next whole block.
           This really should never happen.  (assert()?)
        */
        size = (size+ BLOCK_BYTES- 1)& ~(BLOCK_BYTES- 1);
    }
    rest_rndup = (ctx -> rest+ BLOCK_BYTES- 1)& ~(BLOCK_BYTES- 1);

    /* Read (QIOW) until error or "size" bytes have been read. */
    do
    {
        /* Reduce "size" when next (last) read would overrun the EOF,
           but never below one block (so we'll always get a nice EOF).
        */
        if (size > rest_rndup)
            size = rest_rndup;

        status = sys$qiow( 0, ctx->chan, IO$_READVBLK,
            &ctx->iosb, 0, 0,
            buf, size, ctx->vbn, 0, 0, 0);

        /* If initial status was good, use final status. */
        if ( !ERR(status) )
                status = ctx->iosb.status;

        if ( !ERR(status) || status == SS$_ENDOFFILE )
        {
            act_cnt = ctx->iosb.count;
            /* Ignore whole-block boost when remainder is smaller. */
            if (act_cnt > ctx->rest)
            {
                act_cnt = ctx->rest;
                status = SS$_ENDOFFILE;
            }
            /* Adjust counters/pointers according to delivered bytes. */
            size -= act_cnt;
            buf += act_cnt;
            bytes_read += act_cnt;
            ctx->vbn += ctx->iosb.count/ BLOCK_BYTES;
        }

    } while ( !ERR(status) && (size > 0) );

    if (!ERR(status))
    {
        /* Record any successful status as SS$_NORMAL. */
        ctx -> status = SS$_NORMAL;
    }
    else if (status == SS$_ENDOFFILE)
    {
        /* Record EOF as SS$_ENDOFFILE.  (Ignore error status codes?) */
        ctx -> status = SS$_ENDOFFILE;
    }

    /* Decrement bytes-to-read.  Return the total bytes read. */
    ctx -> rest -= bytes_read;

    return bytes_read;
}

/*-----------------*
 |   vms_error()   |
 *-----------------*
 |   Returns whether last operation on the file caused an error
 */

int vms_error(ctx)
ioctx_t *ctx;
{   /* EOF is not actual error */
    return ERR(ctx->status) && (ctx->status != SS$_ENDOFFILE);
}

/*------------------*
 |   vms_rewind()   |
 *------------------*
 |   Rewinds file to the beginning for the next vms_read().
 */

int vms_rewind(ctx)
ioctx_t *ctx;
{
    ctx -> vbn = 1;
    ctx -> rest = ctx -> size;
    return 0;
}

/*--------------------------*
 |   vms_get_attributes()   |
 *--------------------------*
 |   Malloc a PKWARE extra field and fill with file attributes. Returns
 |   error number of the ZE_??? class.
 |   If the passed ioctx record "FILE *" pointer is NULL, vms_open() is
 |   called to fetch the file attributes.
 |   When `vms_native' is not set, a generic "UT" type timestamp extra
 |   field is generated instead.
 |
 |   2004-11-11 SMS.
 |   Changed to use separate storage for ->extra and ->cextra.  Zip64
 |   processing may move (reallocate) one and not the other.
 */

int vms_get_attributes(ctx, z, z_utim)
ioctx_t *ctx;           /* Internal file control structure. */
struct zlist far *z;    /* Zip entry to compress. */
iztimes *z_utim;
{
    byte    *p;
    byte    *xtra;
    byte    *cxtra;
    struct  PK_header    *h;
    extent  l;
    int     notopened;

    if ( !vms_native )
    {
#ifdef USE_EF_UT_TIME
        /*
         *  A `portable' zipfile entry is created. Create an "UT" extra block
         *  containing UNIX style modification time stamp in UTC, which helps
         *  maintaining the `real' "last modified" time when the archive is
         *  transfered across time zone boundaries.
         */
#  ifdef IZ_CHECK_TZ
        if (!zp_tz_is_valid)
            return ZE_OK;       /* skip silently if no valid TZ info */
#  endif

        if ((xtra = (uch *) malloc( EB_HEADSIZE+ EB_UT_LEN( 1))) == NULL)
            return ZE_MEM;

        if ((cxtra = (uch *) malloc( EB_HEADSIZE+ EB_UT_LEN( 1))) == NULL)
            return ZE_MEM;

        /* Fill xtra[] with data. */
        xtra[ 0] = 'U';
        xtra[ 1] = 'T';
        xtra[ 2] = EB_UT_LEN(1);        /* length of data part of e.f. */
        xtra[ 3] = 0;
        xtra[ 4] = EB_UT_FL_MTIME;
        xtra[ 5] = (byte) (z_utim->mtime);
        xtra[ 6] = (byte) (z_utim->mtime >> 8);
        xtra[ 7] = (byte) (z_utim->mtime >> 16);
        xtra[ 8] = (byte) (z_utim->mtime >> 24);

        /* Copy xtra[] data into cxtra[]. */
        memcpy( cxtra, xtra, (EB_HEADSIZE+ EB_UT_LEN( 1)));

        /* Set sizes and pointers. */
        z->cext = z->ext = (EB_HEADSIZE+ EB_UT_LEN( 1));
        z->extra = (char*) xtra;
        z->cextra = (char*) cxtra;

#endif /* USE_EF_UT_TIME */

        return ZE_OK;
    }

    notopened = (ctx == NULL);
    if ( notopened && ((ctx = vms_open(z->name)) == NULL) )
        return ZE_OPEN;

    l = PK_HEADER_SIZE + sizeof(ctx->PKi);
    if (ctx->acllen > 0)
        l += PK_FLDHDR_SIZE + ctx->acllen;

    if ((xtra = (uch *) malloc( l)) == NULL)
        return ZE_MEM;

    if ((cxtra = (uch *) malloc( l)) == NULL)
        return ZE_MEM;

    /* Fill xtra[] with data. */

    h = (struct PK_header *) xtra;
    h->tag = PK_SIGNATURE;
    h->size = l - EB_HEADSIZE;
    p = (h->data);

    /* Copy default set of attributes */
    memcpy(h->data, (char*)&(ctx->PKi), sizeof(ctx->PKi));
    p += sizeof(ctx->PKi);

    if ( ctx->acllen > 0 )
    {
        struct PK_field *f;

        if (dosify)
            zipwarn("file has ACL, may be incompatible with PKUNZIP","");

        f = (struct PK_field *)p;
        f->tag = ATR$C_ADDACLENT;
        f->size = ctx->acllen;
        memcpy((char *)&(f->value[0]), ctx->aclbuf, ctx->acllen);
        p += PK_FLDHDR_SIZE + ctx->acllen;
    }


    h->crc32 = CRCVAL_INITIAL;                  /* Init CRC register */
    h->crc32 = crc32(h->crc32, (uch *)(h->data), l - PK_HEADER_SIZE);

    /* Copy xtra[] data into cxtra[]. */
    memcpy( cxtra, xtra, l);

    /* Set sizes and pointers. */
    z->ext = z->cext = l;
    z->extra = (char *) xtra;
    z->cextra = (char *) cxtra;

    if (notopened)              /* close "ctx", if we have opened it here */
        vms_close(ctx);

    return ZE_OK;
}


int vms_close(ctx)
ioctx_t *ctx;
{
        sys$dassgn(ctx->chan);
        free(ctx);
        return 0;
}

#endif /* !_UTIL */

#endif /* def VMS_PK_EXTRA */

#endif /* VMS */
