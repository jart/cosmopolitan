/*
  Copyright (c) 1990-2007 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2007-Mar-4 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/*
 *  vms_im.c (zip) by Igor Mandrichenko    Version 2.2-2
 *
 *  Revision history:
 *  ...
 *  2.1-1       16-feb-1993     I.Mandrichenko
 *      Get file size from XABFHC and check bytes rest in file before
 *      reading.
 *  2.1-2       2-mar-1993      I.Mandrichenko
 *      Make code more standard
 *  2.2         21-jun-1993     I.Mandrichenko
 *      Free all allocated space, use more static storage.
 *      Use memcompress() from bits.c (deflation) for block compression.
 *      To revert to old compression method #define OLD_COMPRESS
 *  2.2-2       28-sep-1995     C.Spieler
 *      Reorganized code for easier maintance of the two incompatible
 *      flavours (IM style and PK style) VMS attribute support.
 *      Generic functions (common to both flavours) are now collected
 *      in a `wrapper' source file that includes one of the VMS attribute
 *      handlers.
 *  3.0         23-Oct-2004     Steven Schweda
 *      Changed to maintain compatibility with VMS_PK.C.  Note that
 *      reading with sys$read() prevents getting any data past EOF,
 *      regardless of appearances.  Moved the VMS_PK_EXTRA test into
 *      here from VMS.C to allow more general automatic dependency
 *      generation.
 *              17-Feb-2005     Steven Schweda
 *      Added support for ODS5 extended names.
 */

#ifdef VMS                      /* For VMS only ! */

#ifndef VMS_PK_EXTRA

#define OLD_COMPRESS            /*To use old compression method define it.*/

#ifdef VMS_ZIP
#undef VMS_ZIP                  /* do NOT include PK style Zip definitions */
#endif

#include "vms.h"

#ifndef __LIB$ROUTINES_LOADED
#include <lib$routines.h>
#endif

#ifndef UTIL

#define RET_ERROR 1
#define RET_SUCCESS 0
#define RET_EOF 0

#define Kbyte 1024

typedef struct XAB *xabptr;

/*
 *   Block sizes
 */

#define EXTL0   ((FABL + EXTHL)+        \
                (XFHCL + EXTHL)+        \
                (XPROL + EXTHL)+        \
                (XDATL + EXTHL)+        \
                (XRDTL + EXTHL))

#ifdef OLD_COMPRESS
#define PAD     sizeof(uch)
#else
#define PAD     10*sizeof(ush)          /* Two extra bytes for compr. header */
#endif

#define PAD0    (5*PAD)                 /* Reserve space for the case when
                                         *  compression fails */
static int _compress(uch *from, uch *to, int size);
#ifdef DEBUG
static void dump_rms_block(uch *p);
#endif /* DEBUG */

/********************************
 *   Function set_extra_field   *
 ********************************/
/*
 |   2004-11-11 SMS.
 |   Changed to use separate storage for ->extra and ->cextra.  Zip64
 |   processing may move (reallocate) one and not the other.
 */

static uch *_compress_block(register struct IZ_block *to,
                            uch *from, int size, char *sig);
static int get_vms_version(char *verbuf, int len);

int set_extra_field(z, z_utim)
  struct zlist far *z;
  iztimes *z_utim;
/*
 *      Get file VMS file attributes and store them into extent fields.
 *      Store VMS version also.
 *      On error leave z intact.
 */
{
    int status;
    uch *xtra;
    uch *cxtra;
    uch *scan;
    extent extra_l;
    static struct FAB fab;
    static struct NAM_STRUCT nam;
    static struct XABSUM xabsum;
    static struct XABFHC xabfhc;
    static struct XABDAT xabdat;
    static struct XABPRO xabpro;
    static struct XABRDT xabrdt;
    xabptr x = (xabptr)NULL, xab_chain = (xabptr)NULL, last_xab = (xabptr)NULL;
    int nk, na;
    int i;
    int rc=RET_ERROR;
    char verbuf[80];
    int verlen = 0;

    if (!vms_native)
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
        xtra[ 5] = (uch) (z_utim->mtime);
        xtra[ 6] = (uch) (z_utim->mtime >> 8);
        xtra[ 7] = (uch) (z_utim->mtime >> 16);
        xtra[ 8] = (uch) (z_utim->mtime >> 24);

        /* Copy xtra[] data into cxtra[]. */
        memcpy( cxtra, xtra, (EB_HEADSIZE+ EB_UT_LEN( 1)));

        /* Set sizes and pointers. */
        z->cext = z->ext = (EB_HEADSIZE+ EB_UT_LEN( 1));
        z->extra = (char *) xtra;
        z->cextra = (char *) cxtra;

#endif /* USE_EF_UT_TIME */

        return RET_SUCCESS;
    }

    /*
     *  Initialize RMS control blocks and link them
     */

    fab =    cc$rms_fab;
    nam =    CC_RMS_NAM;
    xabsum = cc$rms_xabsum;
    xabdat = cc$rms_xabdat;
    xabfhc = cc$rms_xabfhc;
    xabpro = cc$rms_xabpro;
    xabrdt = cc$rms_xabrdt;

    fab.FAB_NAM = &nam;
    fab.fab$l_xab = (char*)&xabsum;
    /*
     *  Open the file and read summary information.
     */

#ifdef NAML$C_MAXRSS

    fab.fab$l_dna = (char *) -1;    /* Using NAML for default name. */
    fab.fab$l_fna = (char *) -1;    /* Using NAML for file name. */

#endif /* def NAML$C_MAXRSS */

    FAB_OR_NAML( fab, nam).FAB_OR_NAML_FNA = z->name;
    FAB_OR_NAML( fab, nam).FAB_OR_NAML_FNS = strlen( z->name);

#ifdef NAML$M_OPEN_SPECIAL
    /* 2007-02-28 SMS.
     * If processing symlinks as symlinks ("-y"), then $OPEN the
     * link, not the target file.
     *
     * (nam.naml$v_open_special gets us the symlink itself instead of
     * its target.  fab.fab$v_bio is necessary to allow sys$open() to
     * work.  Without it, you get status %x0001860c, "%RMS-F-ORG,
     * invalid file organization value".)
     */
    if (linkput)
    {
        nam.naml$v_open_special = 1;
        fab.fab$v_bio = 1;
    }
#endif /* def NAML$M_OPEN_SPECIAL */

    status = sys$open(&fab);
    if (ERR(status))
    {
#ifdef DEBUG
        printf("set_extra_field: sys$open for file %s:\n  error status = %d\n",
               z->name, status);
#endif
        goto err_exit;
    }

    nk = xabsum.xab$b_nok;
    na = xabsum.xab$b_noa;
#ifdef DEBUG
    printf("%d keys, %d alls\n", nk, na);
#endif

    /*
     *  Allocate XABKEY and XABALL blocks and link them
     */

    xabfhc.xab$l_nxt = (char*)&xabdat;
    xabdat.xab$l_nxt = (char*)&xabpro;
    xabpro.xab$l_nxt = (char*)&xabrdt;
    xabrdt.xab$l_nxt = NULL;

    xab_chain = (xabptr)(&xabfhc);
    last_xab  = (xabptr)(&xabrdt);

#define INIT(ptr,size,type,init)     \
        if ( (ptr = (type *)malloc(size)) == NULL )     \
        {                                               \
              printf( "set_extra_field: Insufficient memory.\n" );   \
                      goto err_exit;                    \
        }                                               \
        *(ptr) = (init);
    /*
     *  Allocate and initialize all needed XABKEYs and XABALLs
     */
    for (i = 0; i < nk; i++)
    {
        struct XABKEY *k;
        INIT(k, XKEYL, struct XABKEY, cc$rms_xabkey);
        k->xab$b_ref = i;
        if (last_xab != NULL)
            last_xab->xab$l_nxt = (char*)k;
        last_xab = (xabptr)k;
    }
    for (i = 0; i < na; i++)
    {
        struct XABALL *a;
        INIT(a, XALLL, struct XABALL, cc$rms_xaball);
        a->xab$b_aid = i;
        if (last_xab != NULL)
            last_xab->xab$l_nxt = (char*)a;
        last_xab = (xabptr)a;
    }

    fab.fab$l_xab = (char*)xab_chain;
#ifdef DEBUG
    printf("Dump of XAB chain before $DISPLAY:\n");
    for (x = xab_chain; x != NULL; x = x->xab$l_nxt)
        dump_rms_block((uch *)x);
#endif
    /*
     *  Get information on the file structure etc.
     */
    status = sys$display(&fab, 0, 0);
    if (ERR(status))
    {
#ifdef DEBUG
        printf("set_extra_field: sys$display for file %s:\n  error status = %d\n",
               z->name, status);
#endif
        goto err_exit;
    }

#ifdef DEBUG
    printf("\nDump of XAB chain after $DISPLAY:\n");
    for (x = xab_chain; x != NULL; x = x->xab$l_nxt)
        dump_rms_block((uch *)x);
#endif

    fab.fab$l_xab = NULL;  /* Keep XABs */
    status = sys$close(&fab);
    if (ERR(status))
    {
#ifdef DEBUG
        printf("set_extra_field: sys$close for file %s:\n  error status = %d\n",
               z->name, status);
#endif
        goto err_exit;
    }

    extra_l = EXTL0 + nk * (XKEYL + EXTHL) + na * (XALLL + EXTHL);
#ifndef OLD_COMPRESS
    extra_l += PAD0 + (nk+na) * PAD;
#endif

    if ( (verlen = get_vms_version(verbuf, sizeof(verbuf))) > 0 )
    {
        extra_l += verlen + EXTHL;
#ifndef OLD_COMPRESS
        extra_l += PAD;
#endif
    }

    if ((scan = xtra = (uch *) malloc( extra_l)) == (uch*)NULL)
    {
#ifdef DEBUG
        printf(
         "set_extra_field: Insufficient memory to allocate extra L buffer\n");
#endif
        goto err_exit;
    }

    if ((cxtra = (uch *) malloc( extra_l)) == (uch*)NULL)
    {
#ifdef DEBUG
        printf(
         "set_extra_field: Insufficient memory to allocate extra C buffer\n");
#endif
        goto err_exit;
    }

    if (verlen > 0)
        scan = _compress_block((struct IZ_block *)scan, (uch *)verbuf,
                               verlen, VERSIG);

    /*
     *  Zero all unusable fields to improve compression
     */
    fab.fab$b_fns = fab.fab$b_shr = fab.fab$b_dns = fab.fab$b_fac = 0;
    fab.fab$w_ifi = 0;
    fab.fab$l_stv = fab.fab$l_sts = fab.fab$l_ctx = 0;
    fab.fab$l_dna = NULL;
    fab.fab$l_fna = NULL;
    fab.fab$l_nam = NULL;
#ifdef NAML$C_MAXRSS
    fab.fab$l_naml = NULL;
#endif /* def NAML$C_MAXRSS */
    fab.fab$l_xab = NULL;

#ifdef DEBUG
    dump_rms_block( (uch *)&fab );
#endif
    scan = _compress_block((struct IZ_block *)scan, (uch *)&fab, FABL, FABSIG);
    for (x = xab_chain; x != NULL;)
    {
        int bln;
        char *sig;
        xabptr next;

        next = (xabptr)(x->xab$l_nxt);
        x->xab$l_nxt = 0;

        switch (x->xab$b_cod)
        {
            case XAB$C_ALL:
                bln = XALLL;
                sig = XALLSIG;
                break;
            case XAB$C_KEY:
                bln = XKEYL;
                sig = XKEYSIG;
                break;
            case XAB$C_PRO:
                bln = XPROL;
                sig = XPROSIG;
                break;
            case XAB$C_FHC:
                bln = XFHCL;
                sig = XFHCSIG;
                break;
            case XAB$C_DAT:
                bln = XDATL;
                sig = XDATSIG;
                break;
            case XAB$C_RDT:
                bln = XRDTL;
                sig = XRDTSIG;
                break;
            default:
                bln = 0;
                sig = 0L;
                break;
        }
        if (bln > 0)
            scan = _compress_block((struct IZ_block *)scan, (uch *)x,
                                   bln, sig);
        x = next;
    }

    /* Copy xtra[] data into cxtra[]. */
    memcpy( cxtra, xtra, (scan- xtra));

    /* Set sizes and pointers. */
    z->cext = z->ext = scan- xtra;
    z->extra = (char*) xtra;
    z->cextra = (char*) cxtra;

    rc = RET_SUCCESS;

err_exit:
    /*
     *  Give up all allocated blocks
     */
    for (x = (struct XAB *)xab_chain; x != NULL; )
    {
        struct XAB *next;
        next = (xabptr)(x->xab$l_nxt);
        if (x->xab$b_cod == XAB$C_ALL || x->xab$b_cod == XAB$C_KEY)
            free(x);
        x = next;
    }
    return rc;
}

static int get_vms_version(verbuf, len)
char *verbuf;
int len;
{
    int i = SYI$_VERSION;
    int verlen = 0;
    struct dsc$descriptor version;
    char *m;

    version.dsc$a_pointer = verbuf;
    version.dsc$w_length  = len - 1;
    version.dsc$b_dtype   = DSC$K_DTYPE_B;
    version.dsc$b_class   = DSC$K_CLASS_S;

    if (ERR(lib$getsyi(&i, 0, &version, &verlen, 0, 0)) || verlen == 0)
        return 0;

    /* Cut out trailing spaces "V5.4-3   " -> "V5.4-3" */
    for (m = verbuf + verlen, i = verlen - 1; i > 0 && verbuf[i] == ' '; --i)
        --m;
    *m = 0;

    /* Cut out release number "V5.4-3" -> "V5.4" */
    if ((m = strrchr(verbuf, '-')) != NULL)
        *m = 0;
    return strlen(verbuf) + 1;  /* Transmit ending 0 too */
}

#define CTXSIG ((ulg)('CtXx'))

typedef struct user_context
{
    ulg sig;
    struct FAB *fab;
    struct NAM_STRUCT *nam;
    struct RAB *rab;
    uzoff_t size;
    uzoff_t rest;
    int status;
} Ctx, *Ctxptr;

Ctx init_ctx =
{
        CTXSIG,
        NULL,
        NULL,
        NULL,
        0L,
        0L,
        0
};

#define CTXL    sizeof(Ctx)
#define CHECK_RAB(_r) ( (_r) != NULL &&                         \
                        (_r) -> rab$b_bid == RAB$C_BID &&       \
                        (_r) -> rab$b_bln == RAB$C_BLN &&       \
                        (_r) -> rab$l_ctx != 0         &&       \
                        (_r) -> rab$l_fab != NULL )


#define BLOCK_BYTES 512

/**************************
 *   Function vms_open    *
 **************************/
struct RAB *vms_open(name)
    char *name;
{
    struct FAB *fab;
    struct NAM_STRUCT *nam;
    struct RAB *rab;
    struct XABFHC *fhc;
    Ctxptr ctx;

    if ((fab = (struct FAB *) malloc(FABL)) == NULL)
        return NULL;

    if ((nam =
     (struct NAM_STRUCT *) malloc( sizeof( struct NAM_STRUCT))) == NULL)
    {
        free(fab);
        return NULL;
    }

    if ((rab = (struct RAB *) malloc(RABL)) == NULL)
    {
        free(fab);
        free(nam);
        return NULL;
    }

    if ((fhc = (struct XABFHC *) malloc(XFHCL)) == (struct XABFHC *)NULL)
    {
        free(fab);
        free(nam);
        free(rab);
        return (struct RAB *)NULL;
    }
    if ((ctx = (Ctxptr) malloc(CTXL)) == (Ctxptr)NULL)
    {
        free(fab);
        free(nam);
        free(rab);
        free(fhc);
        return (struct RAB *)NULL;
    }
    *fab = cc$rms_fab;
    *nam = CC_RMS_NAM;
    *rab = cc$rms_rab;
    *fhc = cc$rms_xabfhc;

    fab->FAB_NAM = nam;

#ifdef NAML$C_MAXRSS

    fab->fab$l_dna = (char *) -1;   /* Using NAML for default name. */
    fab->fab$l_fna = (char *) -1;   /* Using NAML for file name. */

#endif /* def NAML$C_MAXRSS */

    FAB_OR_NAML( fab, nam)->FAB_OR_NAML_FNA = name;
    FAB_OR_NAML( fab, nam)->FAB_OR_NAML_FNS = strlen( name);

    fab->fab$b_fac = FAB$M_GET | FAB$M_BIO;
    fab->fab$l_xab = (char*)fhc;

#ifdef NAML$M_OPEN_SPECIAL
    /* 2007-02-28 SMS.
     * If processing symlinks as symlinks ("-y"), then $OPEN the
     * link, not the target file.  (Note that here the required
     * fab->fab$v_bio flag was set above.)
     */
    if (linkput)
    {
        nam->naml$v_open_special = 1;
    }
#endif /* def NAML$M_OPEN_SPECIAL */

    if (ERR(sys$open(fab)))
    {
        sys$close(fab);
        free(fab);
        free(nam);
        free(rab);
        free(fhc);
        free(ctx);
        return (struct RAB *)NULL;
    }

    rab->rab$l_fab = fab;
    rab->rab$l_rop = RAB$M_BIO;

    if (ERR(sys$connect(rab)))
    {
        sys$close(fab);
        free(fab);
        free(nam);
        free(rab);
        free(ctx);
        return (struct RAB *)NULL;
    }

    *ctx = init_ctx;
    ctx->fab = fab;
    ctx->nam = nam;
    ctx->rab = rab;

    if (fhc->xab$l_ebk == 0)
    {
        /* Only known size is all allocated blocks.
           (This occurs with a zero-length file, for example.)
        */
        ctx->size =
        ctx->rest = ((uzoff_t) fhc->xab$l_hbk)* BLOCK_BYTES;
    }
    else
    {
        /* Store normal (used) size in ->size.
           If only one -V, store normal (used) size in ->rest.
           If -VV, store allocated-blocks size in ->rest.
        */
        ctx->size =
         (((uzoff_t) fhc->xab$l_ebk)- 1)* BLOCK_BYTES+ fhc->xab$w_ffb;
        if (vms_native < 2)
            ctx->rest = ctx->size;
        else
            ctx->rest = ((uzoff_t) fhc->xab$l_hbk)* BLOCK_BYTES;
    }

    free(fhc);
    fab->fab$l_xab = NULL;
    rab->rab$l_ctx = (unsigned) ctx;
    return rab;
}

/**************************
 *   Function vms_close   *
 **************************/
int vms_close(rab)
    struct RAB *rab;
{
    struct FAB *fab;
    struct NAM_STRUCT *nam;
    Ctxptr ctx;

    if (!CHECK_RAB(rab))
        return RET_ERROR;
    fab = (ctx = (Ctxptr)(rab->rab$l_ctx))->fab;
    nam = (ctx = (Ctxptr)(rab->rab$l_ctx))->nam;
    sys$close(fab);

    free(fab);
    free(nam);
    free(rab);
    free(ctx);

    return RET_SUCCESS;
}

/**************************
 *   Function vms_rewind  *
 **************************/
int vms_rewind(rab)
    struct RAB *rab;
{
    Ctxptr ctx;

    int status;
    if (!CHECK_RAB(rab))
        return RET_ERROR;

    ctx = (Ctxptr) (rab->rab$l_ctx);
    if (ERR(status = sys$rewind(rab)))
    {
        ctx->status = status;
        return RET_ERROR;
    }

    ctx->status = 0;
    ctx->rest = ctx->size;

    return RET_SUCCESS;
}


#define KByte (2* BLOCK_BYTES)
#define MAX_READ_BYTES (32* KByte)

/**************************
 *   Function vms_read    *
 **************************/
size_t vms_read(rab, buf, size)
struct RAB *rab;
char *buf;
size_t size;
/*
 *      size must be greater or equal to 512 !
 */
{
    int status;
    Ctxptr ctx;

    ctx = (Ctxptr)rab->rab$l_ctx;

    if (!CHECK_RAB(rab))
        return 0;

    if (ctx -> rest == 0)
        return 0;               /* Eof */

    /* If request is smaller than a whole block, fail.
       This really should never happen.  (assert()?)
    */
    if (size < BLOCK_BYTES)
        return 0;

    /* 2004-09-27 SMS.
       Code here now resembles low-level QIO code in VMS_PK.C, but I
       doubt that sys$read() will actually get past the official EOF.
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

    /* Reduce "size" when next (last) read would overrun the EOF,
       but never below one byte (so we'll always get a nice EOF).
    */
    if (size > ctx->rest)
        size = ctx->rest;
    if (size == 0)
        size = 1;

    rab->rab$l_ubf = buf;
    rab->rab$w_usz = size;
    status = sys$read(rab);

    if (!ERR(status) && rab->rab$w_rsz > 0)
    {
        ctx -> status = 0;
        ctx -> rest -= rab->rab$w_rsz;
        return rab->rab$w_rsz;
    }
    else
    {
        ctx->status = (status==RMS$_EOF ? 0:status);
        if (status == RMS$_EOF)
                ctx -> rest = 0;
        return 0;
    }
}

/**************************
 *   Function vms_error   *
 **************************/
int vms_error(rab)
    struct RAB *rab;
{
    if (!CHECK_RAB(rab))
        return RET_ERROR;
    return ((Ctxptr) (rab->rab$l_ctx))->status;
}


#ifdef DEBUG
static void dump_rms_block(p)
    uch *p;
{
    uch bid, len;
    int err;
    char *type;
    char buf[132];
    int i;

    err = 0;
    bid = p[0];
    len = p[1];
    switch (bid)
    {
        case FAB$C_BID:
            type = "FAB";
            break;
        case XAB$C_ALL:
            type = "xabALL";
            break;
        case XAB$C_KEY:
            type = "xabKEY";
            break;
        case XAB$C_DAT:
            type = "xabDAT";
            break;
        case XAB$C_RDT:
            type = "xabRDT";
            break;
        case XAB$C_FHC:
            type = "xabFHC";
            break;
        case XAB$C_PRO:
            type = "xabPRO";
            break;
        default:
            type = "Unknown";
            err = 1;
            break;
    }
    printf("Block @%08X of type %s (%d).", p, type, bid);
    if (err)
    {
        printf("\n");
        return;
    }
    printf(" Size = %d\n", len);
    printf(" Offset - Hex - Dec\n");
    for (i = 0; i < len; i += 8)
    {
        int j;

        printf("%3d - ", i);
        for (j = 0; j < 8; j++)
            if (i + j < len)
                printf("%02X ", p[i + j]);
            else
                printf("   ");
        printf(" - ");
        for (j = 0; j < 8; j++)
            if (i + j < len)
                printf("%03d ", p[i + j]);
            else
                printf("    ");
        printf("\n");
    }
}
#endif /* DEBUG */

#ifdef OLD_COMPRESS
# define BC_METHOD      EB_IZVMS_BC00
# define        COMP_BLK(to,tos,from,froms) _compress( from,to,froms )
#else
# define BC_METHOD      EB_IZVMS_BCDEFL
# define        COMP_BLK(to,tos,from,froms) memcompress(to,tos,from,froms)
#endif

static uch *_compress_block(to,from,size,sig)
register struct IZ_block *to;
uch *from;
int size;
char *sig;
{
        ulg cl;
        to -> sig =  *(ush*)IZ_SIGNATURE;
        to -> bid =       *(ulg*)(sig);
        to -> flags =           BC_METHOD;
        to -> length =  size;
#ifdef DEBUG
        printf("\nmemcompr(%d,%d,%d,%d)\n",&(to->body[0]),size+PAD,from,size);
#endif
        cl = COMP_BLK( &(to->body[0]), size+PAD, from, size );
#ifdef DEBUG
        printf("Compressed to %d\n",cl);
#endif
        if (cl >= size)
        {
                memcpy(&(to->body[0]), from, size);
                to->flags = EB_IZVMS_BCSTOR;
                cl = size;
#ifdef DEBUG
                printf("Storing block...\n");
#endif
        }
        return (uch*)(to) + (to->size = cl + EXTBSL + RESL) + EB_HEADSIZE;
}

#define NBITS 32

static int _compress(from,to,size)
uch *from,*to;
int size;
{
    int off=0;
    ulg bitbuf=0;
    int bitcnt=0;
    int i;

#define _BIT(val,len)   {                       \
        if (bitcnt + (len) > NBITS)             \
            while(bitcnt >= 8)                  \
            {                                   \
                to[off++] = (uch)bitbuf;        \
                bitbuf >>= 8;                   \
                bitcnt -= 8;                    \
            }                                   \
        bitbuf |= ((ulg)(val))<<bitcnt;         \
        bitcnt += len;                          \
    }

#define _FLUSH  {                               \
            while(bitcnt>0)                     \
            {                                   \
                to[off++] = (uch)bitbuf;        \
                bitbuf >>= 8;                   \
                bitcnt -= 8;                    \
            }                                   \
        }

    for (i=0; i<size; i++)
    {
        if (from[i])
        {
                _BIT(1,1);
                _BIT(from[i],8);
        }
        else
            _BIT(0,1);
    }
    _FLUSH;
    return off;
}

#endif /* !UTIL */

#endif /* ndef VMS_PK_EXTRA */

#endif /* VMS */
