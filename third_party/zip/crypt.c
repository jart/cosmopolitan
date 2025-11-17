/*
  Copyright (c) 1990-2008 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2007-Mar-4 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/*
  crypt.c (full version) by Info-ZIP.      Last revised:  [see crypt.h]

  The main encryption/decryption source code for Info-Zip software was
  originally written in Europe.  To the best of our knowledge, it can
  be freely distributed in both source and object forms from any country,
  including the USA under License Exception TSU of the U.S. Export
  Administration Regulations (section 740.13(e)) of 6 June 2002.

  NOTE on copyright history:
  Previous versions of this source package (up to version 2.8) were
  not copyrighted and put in the public domain.  If you cannot comply
  with the Info-Zip LICENSE, you may want to look for one of those
  public domain versions.
 */

/*
  This encryption code is a direct transcription of the algorithm from
  Roger Schlafly, described by Phil Katz in the file appnote.txt.  This
  file (appnote.txt) is distributed with the PKZIP program (even in the
  version without encryption capabilities).
 */

#define ZCRYPT_INTERNAL
#include "zip.h"
#include "crypt.h"
#include "ttyio.h"

#if CRYPT

#ifndef FALSE
#  define FALSE 0
#endif

#ifdef ZIP
   /* For the encoding task used in Zip (and ZipCloak), we want to initialize
      the crypt algorithm with some reasonably unpredictable bytes, see
      the crypthead() function. The standard rand() library function is
      used to supply these `random' bytes, which in turn is initialized by
      a srand() call. The srand() function takes an "unsigned" (at least 16bit)
      seed value as argument to determine the starting point of the rand()
      pseudo-random number generator.
      This seed number is constructed as "Seed = Seed1 .XOR. Seed2" with
      Seed1 supplied by the current time (= "(unsigned)time()") and Seed2
      as some (hopefully) nondeterministic bitmask. On many (most) systems,
      we use some "process specific" number, as the PID or something similar,
      but when nothing unpredictable is available, a fixed number may be
      sufficient.
      NOTE:
      1.) This implementation requires the availability of the following
          standard UNIX C runtime library functions: time(), rand(), srand().
          On systems where some of them are missing, the environment that
          incorporates the crypt routines must supply suitable replacement
          functions.
      2.) It is a very bad idea to use a second call to time() to set the
          "Seed2" number! In this case, both "Seed1" and "Seed2" would be
          (almost) identical, resulting in a (mostly) "zero" constant seed
          number passed to srand().

      The implementation environment defined in the "zip.h" header should
      supply a reasonable definition for ZCR_SEED2 (an unsigned number; for
      most implementations of rand() and srand(), only the lower 16 bits are
      significant!). An example that works on many systems would be
           "#define ZCR_SEED2  (unsigned)getpid()".
      The default definition for ZCR_SEED2 supplied below should be regarded
      as a fallback to allow successful compilation in "beta state"
      environments.
    */
#  include <time.h>     /* time() function supplies first part of crypt seed */
   /* "last resort" source for second part of crypt seed pattern */
#  ifndef ZCR_SEED2
#    define ZCR_SEED2 (unsigned)3141592654L     /* use PI as default pattern */
#  endif
#  ifdef GLOBAL         /* used in Amiga system headers, maybe others too */
#    undef GLOBAL
#  endif
#  define GLOBAL(g) g
#else /* !ZIP */
#  define GLOBAL(g) G.g
#endif /* ?ZIP */


#ifdef UNZIP
   /* char *key = (char *)NULL; moved to globals.h */
#  ifndef FUNZIP
     local int testp OF((__GPRO__ ZCONST uch *h));
     local int testkey OF((__GPRO__ ZCONST uch *h, ZCONST char *key));
#  endif
#else /* def UNZIP */           /* moved to globals.h for UnZip */
   local z_uint4 keys[3];       /* keys defining the pseudo-random sequence */
#endif /* def UNZIP [else] */

#ifndef Trace
#  ifdef CRYPT_DEBUG
#    define Trace(x) fprintf x
#  else
#    define Trace(x)
#  endif
#endif

#include "crc32.h"

#ifdef IZ_CRC_BE_OPTIMIZ
   local z_uint4 near crycrctab[256];
   local z_uint4 near *cry_crctb_p = NULL;
   local z_uint4 near *crytab_init OF((__GPRO));
#  define CRY_CRC_TAB  cry_crctb_p
#  undef CRC32
#  define CRC32(c, b, crctab) (crctab[((int)(c) ^ (b)) & 0xff] ^ ((c) >> 8))
#else
#  define CRY_CRC_TAB  CRC_32_TAB
#endif /* ?IZ_CRC_BE_OPTIMIZ */

/***********************************************************************
 * Return the next byte in the pseudo-random sequence
 */
int decrypt_byte(__G)
    __GDEF
{
    unsigned temp;  /* POTENTIAL BUG:  temp*(temp^1) may overflow in an
                     * unpredictable manner on 16-bit systems; not a problem
                     * with any known compiler so far, though */

    temp = ((unsigned)GLOBAL(keys[2]) & 0xffff) | 2;
    return (int)(((temp * (temp ^ 1)) >> 8) & 0xff);
}

/***********************************************************************
 * Update the encryption keys with the next byte of plain text
 */
int update_keys(__G__ c)
    __GDEF
    int c;                      /* byte of plain text */
{
    GLOBAL(keys[0]) = CRC32(GLOBAL(keys[0]), c, CRY_CRC_TAB);
    GLOBAL(keys[1]) = (GLOBAL(keys[1])
                       + (GLOBAL(keys[0]) & 0xff))
                      * 134775813L + 1;
    {
      register int keyshift = (int)(GLOBAL(keys[1]) >> 24);
      GLOBAL(keys[2]) = CRC32(GLOBAL(keys[2]), keyshift, CRY_CRC_TAB);
    }
    return c;
}


/***********************************************************************
 * Initialize the encryption keys and the random header according to
 * the given password.
 */
void init_keys(__G__ passwd)
    __GDEF
    ZCONST char *passwd;        /* password string with which to modify keys */
{
#ifdef IZ_CRC_BE_OPTIMIZ
    if (cry_crctb_p == NULL) {
        cry_crctb_p = crytab_init(__G);
    }
#endif
    GLOBAL(keys[0]) = 305419896L;
    GLOBAL(keys[1]) = 591751049L;
    GLOBAL(keys[2]) = 878082192L;
    while (*passwd != '\0') {
        update_keys(__G__ (int)*passwd);
        passwd++;
    }
}


/***********************************************************************
 * Initialize the local copy of the table of precomputed crc32 values.
 * Whereas the public crc32-table is optimized for crc32 calculations
 * on arrays of bytes, the crypt code needs the crc32 values in an
 * byte-order-independent form as 32-bit unsigned numbers. On systems
 * with Big-Endian byte order using the optimized crc32 code, this
 * requires inverting the byte-order of the values in the
 * crypt-crc32-table.
 */
#ifdef IZ_CRC_BE_OPTIMIZ
local z_uint4 near *crytab_init(__G)
    __GDEF
{
    int i;

    for (i = 0; i < 256; i++) {
        crycrctab[i] = REV_BE(CRC_32_TAB[i]);
    }
    return crycrctab;
}
#endif


#ifdef ZIP

/***********************************************************************
 * Write encryption header to file zfile using the password passwd
 * and the cyclic redundancy check crc.
 */
void crypthead(passwd, crc)
    ZCONST char *passwd;         /* password string */
    ulg crc;                     /* crc of file being encrypted */
{
    int n;                       /* index in random header */
    int t;                       /* temporary */
    int c;                       /* random byte */
    uch header[RAND_HEAD_LEN];   /* random header */
    static unsigned calls = 0;   /* ensure different random header each time */

    /* First generate RAND_HEAD_LEN-2 random bytes. We encrypt the
     * output of rand() to get less predictability, since rand() is
     * often poorly implemented.
     */
    if (++calls == 1) {
        srand((unsigned)time(NULL) ^ ZCR_SEED2);
    }
    init_keys(passwd);
    for (n = 0; n < RAND_HEAD_LEN-2; n++) {
        c = (rand() >> 7) & 0xff;
        header[n] = (uch)zencode(c, t);
    }
    /* Encrypt random header (last two bytes is high word of crc) */
    init_keys(passwd);
    for (n = 0; n < RAND_HEAD_LEN-2; n++) {
        header[n] = (uch)zencode(header[n], t);
    }
    header[RAND_HEAD_LEN-2] = (uch)zencode((int)(crc >> 16) & 0xff, t);
    header[RAND_HEAD_LEN-1] = (uch)zencode((int)(crc >> 24) & 0xff, t);
    bfwrite(header, 1, RAND_HEAD_LEN, BFWRITE_DATA);
}


#ifdef UTIL

/***********************************************************************
 * Encrypt the zip entry described by z from file in_file to file y
 * using the password passwd.  Return an error code in the ZE_ class.
 */
int zipcloak(z, passwd)
    struct zlist far *z;    /* zip entry to encrypt */
    ZCONST char *passwd;    /* password string */
{
    int c;                  /* input byte */
    int res;                /* result code */
    zoff_t n;               /* holds offset and counts size */
    int t;                  /* temporary */
    struct zlist far *localz; /* local header */
    uch buf[1024];          /* write buffer */
    int b;                  /* bytes in buffer */

    /* Set encrypted bit, clear extended local header bit and write local
       header to output file */
    if ((n = (zoff_t)zftello(y)) == (zoff_t)-1L) return ZE_TEMP;

    /* assume this archive is one disk and the file is open */

    /* read the local header */
    res = readlocal(&localz, z);

    /* update disk and offset */
    z->dsk = 0;
    z->off = n;

    /* Set encryption and unset any extended local header */
    z->flg |= 1,  z->flg &= ~8;
    localz->lflg |= 1, localz->lflg &= ~8;

    /* Add size of encryption header */
    localz->siz += RAND_HEAD_LEN;
    z->siz = localz->siz;

    /* Put the local header */
    if ((res = putlocal(localz, PUTLOCAL_WRITE)) != ZE_OK) return res;

    /* Initialize keys with password and write random header */
    crypthead(passwd, localz->crc);

    /* Encrypt data */
    b = 0;
    for (n = z->siz - RAND_HEAD_LEN; n; n--) {
      if ((c = getc(in_file)) == EOF) {
          return ferror(in_file) ? ZE_READ : ZE_EOF;
      }
      buf[b] = (uch)zencode(c, t);
      b++;
      if (b >= 1024) {
        /* write the buffer */
        bfwrite(buf, 1, b, BFWRITE_DATA);
        b = 0;
      }
    }
    if (b) {
      /* write the buffer */
      bfwrite(buf, 1, b, BFWRITE_DATA);
      b = 0;
    }

    /* Since we seek to the start of each local header can skip
       reading any extended local header */
    /*
    if ((flag & 8) != 0 && zfseeko(in_file, 16L, SEEK_CUR)) {
        return ferror(in_file) ? ZE_READ : ZE_EOF;
    }
    if (fflush(y) == EOF) return ZE_TEMP;
    */

    /* Update number of bytes written to output file */
    tempzn += (4 + LOCHEAD) + localz->nam + localz->ext + localz->siz;

    /* Free local header */
    if (localz->ext) free(localz->extra);
    if (localz->nam) free(localz->iname);
    if (localz->nam) free(localz->name);
#ifdef UNICODE_SUPPORT
    if (localz->uname) free(localz->uname);
#endif
    free(localz);

    return ZE_OK;
}

/***********************************************************************
 * Decrypt the zip entry described by z from file in_file to file y
 * using the password passwd.  Return an error code in the ZE_ class.
 */
int zipbare(z, passwd)
    struct zlist far *z;  /* zip entry to encrypt */
    ZCONST char *passwd;  /* password string */
{
#ifdef ZIP10
    int c0                /* byte preceding the last input byte */
#endif
    int c1;               /* last input byte */
    /* all file offset and size now zoff_t - 8/28/04 EG */
    zoff_t size;          /* size of input data */
    struct zlist far *localz; /* local header */
    uch buf[1024];        /* write buffer */
    int b;                /* bytes in buffer */
    zoff_t n;
    int r;                /* size of encryption header */
    int res;              /* return code */

    /* Save position */
    if ((n = (zoff_t)zftello(y)) == (zoff_t)-1L) return ZE_TEMP;

    /* Read local header */
    res = readlocal(&localz, z);

    /* Update disk and offset */
    z->dsk = 0;
    z->off = n;

    /* Initialize keys with password */
    init_keys(passwd);

    /* Decrypt encryption header, save last two bytes */
    c1 = 0;
    for (r = RAND_HEAD_LEN; r; r--) {
#ifdef ZIP10
        c0 = c1;
#endif
        if ((c1 = getc(in_file)) == EOF) {
            return ferror(in_file) ? ZE_READ : ZE_EOF;
        }
        Trace((stdout, " (%02x)", c1));
        zdecode(c1);
        Trace((stdout, " %02x", c1));
    }
    Trace((stdout, "\n"));

    /* If last two bytes of header don't match crc (or file time in the
     * case of an extended local header), back up and just copy. For
     * pkzip 2.0, the check has been reduced to one byte only.
     */
#ifdef ZIP10
    if ((ush)(c0 | (c1<<8)) !=
        (z->flg & 8 ? (ush) z->tim & 0xffff : (ush)(z->crc >> 16))) {
#else
    if ((ush)c1 != (z->flg & 8 ? (ush) z->tim >> 8 : (ush)(z->crc >> 24))) {
#endif
        if (zfseeko(in_file, n, SEEK_SET)) {
            return ferror(in_file) ? ZE_READ : ZE_EOF;
        }
        if ((res = zipcopy(z)) != ZE_OK) {
            ziperr(res, "was copying an entry");
        }
        return ZE_MISS;
    }

    z->siz -= RAND_HEAD_LEN;
    localz->siz = z->siz;

    localz->flg = z->flg &= ~9;
    z->lflg = localz->lflg &= ~9;

    if ((res = putlocal(localz, PUTLOCAL_WRITE)) != ZE_OK) return res;

    /* Decrypt data */
    b = 0;
    for (size = z->siz; size; size--) {
        if ((c1 = getc(in_file)) == EOF) {
            return ferror(in_file) ? ZE_READ : ZE_EOF;
        }
        zdecode(c1);
        buf[b] = c1;
        b++;
        if (b >= 1024) {
          /* write the buffer */
          bfwrite(buf, 1, b, BFWRITE_DATA);
          b = 0;
        }
    }
    if (b) {
      /* write the buffer */
      bfwrite(buf, 1, b, BFWRITE_DATA);
      b = 0;
    }
    /* Since we seek to the start of each local header can skip
         reading any extended local header */

    /* Update number of bytes written to output file */
    tempzn += (4 + LOCHEAD) + localz->nam + localz->ext + localz->siz;

    /* Free local header */
    if (localz->ext) free(localz->extra);
    if (localz->nam) free(localz->iname);
    if (localz->nam) free(localz->name);
#ifdef UNICODE_SUPPORT
    if (localz->uname) free(localz->uname);
#endif
    free(localz);

    return ZE_OK;
}


#else /* !UTIL */

/***********************************************************************
 * If requested, encrypt the data in buf, and in any case call fwrite()
 * with the arguments to zfwrite().  Return what fwrite() returns.
 *
 * now write to global y
 *
 * A bug has been found when encrypting large files that don't
 * compress.  See trees.c for the details and the fix.
 */
unsigned zfwrite(buf, item_size, nb)
    zvoid *buf;                 /* data buffer */
    extent item_size;           /* size of each item in bytes */
    extent nb;                  /* number of items */
#if 0
    FILE *f;                    /* file to write to */
#endif
{
    int t;                      /* temporary */

    if (key != (char *)NULL) {  /* key is the global password pointer */
        ulg size;               /* buffer size */
        char *p = (char *)buf;  /* steps through buffer */

        /* Encrypt data in buffer */
        for (size = item_size*(ulg)nb; size != 0; p++, size--) {
            *p = (char)zencode(*p, t);
        }
    }
    /* Write the buffer out */
    return bfwrite(buf, item_size, nb, BFWRITE_DATA);
}

#endif /* ?UTIL */
#endif /* ZIP */


#if (defined(UNZIP) && !defined(FUNZIP))

/***********************************************************************
 * Get the password and set up keys for current zipfile member.
 * Return PK_ class error.
 */
int decrypt(__G__ passwrd)
    __GDEF
    ZCONST char *passwrd;
{
    ush b;
    int n, r;
    uch h[RAND_HEAD_LEN];

    Trace((stdout, "\n[incnt = %d]: ", GLOBAL(incnt)));

    /* get header once (turn off "encrypted" flag temporarily so we don't
     * try to decrypt the same data twice) */
    GLOBAL(pInfo->encrypted) = FALSE;
    defer_leftover_input(__G);
    for (n = 0; n < RAND_HEAD_LEN; n++) {
        b = NEXTBYTE;
        h[n] = (uch)b;
        Trace((stdout, " (%02x)", h[n]));
    }
    undefer_input(__G);
    GLOBAL(pInfo->encrypted) = TRUE;

    if (GLOBAL(newzip)) { /* this is first encrypted member in this zipfile */
        GLOBAL(newzip) = FALSE;
        if (passwrd != (char *)NULL) { /* user gave password on command line */
            if (!GLOBAL(key)) {
                if ((GLOBAL(key) = (char *)malloc(strlen(passwrd)+1)) ==
                    (char *)NULL)
                    return PK_MEM2;
                strcpy(GLOBAL(key), passwrd);
                GLOBAL(nopwd) = TRUE;  /* inhibit password prompting! */
            }
        } else if (GLOBAL(key)) { /* get rid of previous zipfile's key */
            free(GLOBAL(key));
            GLOBAL(key) = (char *)NULL;
        }
    }

    /* if have key already, test it; else allocate memory for it */
    if (GLOBAL(key)) {
        if (!testp(__G__ h))
            return PK_COOL;   /* existing password OK (else prompt for new) */
        else if (GLOBAL(nopwd))
            return PK_WARN;   /* user indicated no more prompting */
    } else if ((GLOBAL(key) = (char *)malloc(IZ_PWLEN+1)) == (char *)NULL)
        return PK_MEM2;

    /* try a few keys */
    n = 0;
    do {
        r = (*G.decr_passwd)((zvoid *)&G, &n, GLOBAL(key), IZ_PWLEN+1,
                             GLOBAL(zipfn), GLOBAL(filename));
        if (r == IZ_PW_ERROR) {         /* internal error in fetch of PW */
            free (GLOBAL(key));
            GLOBAL(key) = NULL;
            return PK_MEM2;
        }
        if (r != IZ_PW_ENTERED) {       /* user replied "skip" or "skip all" */
            *GLOBAL(key) = '\0';        /*   We try the NIL password, ... */
            n = 0;                      /*   and cancel fetch for this item. */
        }
        if (!testp(__G__ h))
            return PK_COOL;
        if (r == IZ_PW_CANCELALL)       /* User replied "Skip all" */
            GLOBAL(nopwd) = TRUE;       /*   inhibit any further PW prompt! */
    } while (n > 0);

    return PK_WARN;

} /* end function decrypt() */



/***********************************************************************
 * Test the password.  Return -1 if bad, 0 if OK.
 */
local int testp(__G__ h)
    __GDEF
    ZCONST uch *h;
{
    int r;
    char *key_translated;

    /* On systems with "obscure" native character coding (e.g., EBCDIC),
     * the first test translates the password to the "main standard"
     * character coding. */

#ifdef STR_TO_CP1
    /* allocate buffer for translated password */
    if ((key_translated = malloc(strlen(GLOBAL(key)) + 1)) == (char *)NULL)
        return -1;
    /* first try, test password translated "standard" charset */
    r = testkey(__G__ h, STR_TO_CP1(key_translated, GLOBAL(key)));
#else /* !STR_TO_CP1 */
    /* first try, test password as supplied on the extractor's host */
    r = testkey(__G__ h, GLOBAL(key));
#endif /* ?STR_TO_CP1 */

#ifdef STR_TO_CP2
    if (r != 0) {
#ifndef STR_TO_CP1
        /* now prepare for second (and maybe third) test with translated pwd */
        if ((key_translated = malloc(strlen(GLOBAL(key)) + 1)) == (char *)NULL)
            return -1;
#endif
        /* second try, password translated to alternate ("standard") charset */
        r = testkey(__G__ h, STR_TO_CP2(key_translated, GLOBAL(key)));
#ifdef STR_TO_CP3
        if (r != 0)
            /* third try, password translated to another "standard" charset */
            r = testkey(__G__ h, STR_TO_CP3(key_translated, GLOBAL(key)));
#endif
#ifndef STR_TO_CP1
        free(key_translated);
#endif
    }
#endif /* STR_TO_CP2 */

#ifdef STR_TO_CP1
    free(key_translated);
    if (r != 0) {
        /* last resort, test password as supplied on the extractor's host */
        r = testkey(__G__ h, GLOBAL(key));
    }
#endif /* STR_TO_CP1 */

    return r;

} /* end function testp() */


local int testkey(__G__ h, key)
    __GDEF
    ZCONST uch *h;      /* decrypted header */
    ZCONST char *key;   /* decryption password to test */
{
    ush b;
#ifdef ZIP10
    ush c;
#endif
    int n;
    uch *p;
    uch hh[RAND_HEAD_LEN]; /* decrypted header */

    /* set keys and save the encrypted header */
    init_keys(__G__ key);
    memcpy(hh, h, RAND_HEAD_LEN);

    /* check password */
    for (n = 0; n < RAND_HEAD_LEN; n++) {
        zdecode(hh[n]);
        Trace((stdout, " %02x", hh[n]));
    }

    /* use fzofft to format zoff_t as strings - 10/19/04 from SMS */
    Trace((stdout,
      "\n  lrec.crc= %08lx  crec.crc= %08lx  pInfo->ExtLocHdr= %s\n",
      GLOBAL(lrec.crc32), GLOBAL(pInfo->crc),
      GLOBAL(pInfo->ExtLocHdr) ? "true":"false"));
    Trace((stdout, "  incnt = %d  unzip offset into zipfile = %s\n",
      GLOBAL(incnt),
      fzofft(GLOBAL(cur_zipfile_bufstart)+(GLOBAL(inptr)-GLOBAL(inbuf)),
             NULL, NULL)));

    /* same test as in zipbare(): */

#ifdef ZIP10 /* check two bytes */
    c = hh[RAND_HEAD_LEN-2], b = hh[RAND_HEAD_LEN-1];
    Trace((stdout,
      "  (c | (b<<8)) = %04x  (crc >> 16) = %04x  lrec.time = %04x\n",
      (ush)(c | (b<<8)), (ush)(GLOBAL(lrec.crc32) >> 16),
      ((ush)GLOBAL(lrec.last_mod_dos_datetime) & 0xffff))));
    if ((ush)(c | (b<<8)) != (GLOBAL(pInfo->ExtLocHdr) ?
                           ((ush)GLOBAL(lrec.last_mod_dos_datetime) & 0xffff) :
                           (ush)(GLOBAL(lrec.crc32) >> 16)))
        return -1;  /* bad */
#else
    b = hh[RAND_HEAD_LEN-1];
    Trace((stdout, "  b = %02x  (crc >> 24) = %02x  (lrec.time >> 8) = %02x\n",
      b, (ush)(GLOBAL(lrec.crc32) >> 24),
      ((ush)GLOBAL(lrec.last_mod_dos_datetime) >> 8) & 0xff));
    if (b != (GLOBAL(pInfo->ExtLocHdr) ?
        ((ush)GLOBAL(lrec.last_mod_dos_datetime) >> 8) & 0xff :
        (ush)(GLOBAL(lrec.crc32) >> 24)))
        return -1;  /* bad */
#endif
    /* password OK:  decrypt current buffer contents before leaving */
    for (n = (zoff_t)GLOBAL(incnt) > GLOBAL(csize) ?
             (int)GLOBAL(csize) : GLOBAL(incnt),
         p = GLOBAL(inptr); n--; p++)
        zdecode(*p);
    return 0;       /* OK */

} /* end function testkey() */

#endif /* UNZIP && !FUNZIP */

#else /* !CRYPT */

/* something "externally visible" to shut up compiler/linker warnings */
int zcr_dummy;

#endif /* ?CRYPT */
