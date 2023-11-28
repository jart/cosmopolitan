/*
  Copyright (c) 1990-2007 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2007-Mar-04 or later
  (the contents of which are also included in unzip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/* explode.c -- by Mark Adler
   version c17d, 01 December 2007 */


/* Copyright history:
   - Starting with UnZip 5.41 of 16-April-2000, this source file
     is covered by the Info-Zip LICENSE cited above.
   - Prior versions of this source file, found in UnZip source packages
     up to UnZip 5.40, were put in the public domain.
     The original copyright note by Mark Adler was:
         "You can do whatever you like with this source file,
         though I would prefer that if you modify it and
         redistribute it that you include comments to that effect
         with your name and the date.  Thank you."

   History:
   vers    date          who           what
   ----  ---------  --------------  ------------------------------------
    c1   30 Mar 92  M. Adler        explode that uses huft_build from inflate
                                    (this gives over a 70% speed improvement
                                    over the original unimplode.c, which
                                    decoded a bit at a time)
    c2    4 Apr 92  M. Adler        fixed bug for file sizes a multiple of 32k.
    c3   10 Apr 92  M. Adler        added a little memory tracking if DEBUG
    c4   11 Apr 92  M. Adler        added NOMEMCPY do kill use of memcpy()
    c5   21 Apr 92  M. Adler        added the WSIZE #define to allow reducing
                                    the 32K window size for specialized
                                    applications.
    c6   31 May 92  M. Adler        added typecasts to eliminate some warnings
    c7   27 Jun 92  G. Roelofs      added more typecasts.
    c8   17 Oct 92  G. Roelofs      changed ULONG/UWORD/byte to ulg/ush/uch.
    c9   19 Jul 93  J. Bush         added more typecasts (to return values);
                                    made l[256] array static for Amiga.
    c10   8 Oct 93  G. Roelofs      added used_csize for diagnostics; added
                                    buf and unshrink arguments to flush();
                                    undef'd various macros at end for Turbo C;
                                    removed NEXTBYTE macro (now in unzip.h)
                                    and bytebuf variable (not used); changed
                                    memset() to memzero().
    c11   9 Jan 94  M. Adler        fixed incorrect used_csize calculation.
    c12   9 Apr 94  G. Roelofs      fixed split comments on preprocessor lines
                                    to avoid bug in Encore compiler.
    c13  25 Aug 94  M. Adler        fixed distance-length comment (orig c9 fix)
    c14  22 Nov 95  S. Maxwell      removed unnecessary "static" on auto array
    c15   6 Jul 96  W. Haidinger    added ulg typecasts to flush() calls.
    c16   8 Feb 98  C. Spieler      added ZCONST modifiers to const tables
                                    and #ifdef DEBUG around debugging code.
    c16b 25 Mar 98  C. Spieler      modified DLL code for slide redirection.
    c16d 05 Jul 99  C. Spieler      take care of flush() return values and
                                    stop processing in case of errors
    c17  04 Feb 01  C. Spieler      reorganized code to reduce repetitions
                                    of large code parts; adapted huft decoding
                                    to the changes in inflate's huft_build()
                                    due to support of deflate64; fixed memory
                                    leaks (huft tables were not free'd when
                                    get_tree() failed).
    c17b 16 Feb 02  C. Spieler      changed type of the "extra lengths" array
                                    "extra" from ush into uch (to save space)
    c17c 10 Aug 04  NN              file sizes use zoff_t.
    c17d 01 Dec 07  C. Spieler      type for file sizes changed from zoff_t
                                    into zusz_t.
 */


/*
   Explode imploded (PKZIP method 6 compressed) data.  This compression
   method searches for as much of the current string of bytes (up to a length
   of ~320) in the previous 4K or 8K bytes.  If it doesn't find any matches
   (of at least length 2 or 3), it codes the next byte.  Otherwise, it codes
   the length of the matched string and its distance backwards from the
   current position.  Single bytes ("literals") are preceded by a one (a
   single bit) and are either uncoded (the eight bits go directly into the
   compressed stream for a total of nine bits) or Huffman coded with a
   supplied literal code tree.  If literals are coded, then the minimum match
   length is three, otherwise it is two.

   There are therefore four kinds of imploded streams: 8K search with coded
   literals (min match = 3), 4K search with coded literals (min match = 3),
   8K with uncoded literals (min match = 2), and 4K with uncoded literals
   (min match = 2).  The kind of stream is identified in two bits of a
   general purpose bit flag that is outside of the compressed stream.

   Distance-length pairs for matched strings are preceded by a zero bit (to
   distinguish them from literals) and are always coded.  The distance comes
   first and is either the low six (4K) or low seven (8K) bits of the
   distance (uncoded), followed by the high six bits of the distance coded.
   Then the length is six bits coded (0..63 + min match length), and if the
   maximum such length is coded, then it's followed by another eight bits
   (uncoded) to be added to the coded length.  This gives a match length
   range of 2..320 or 3..321 bytes.

   The literal, length, and distance codes are all represented in a slightly
   compressed form themselves.  What is sent are the lengths of the codes for
   each value, which is sufficient to construct the codes.  Each byte of the
   code representation is the code length (the low four bits representing
   1..16), and the number of values sequentially with that length (the high
   four bits also representing 1..16).  There are 256 literal code values (if
   literals are coded), 64 length code values, and 64 distance code values,
   in that order at the beginning of the compressed stream.  Each set of code
   values is preceded (redundantly) with a byte indicating how many bytes are
   in the code description that follows, in the range 1..256.

   The codes themselves are decoded using tables made by huft_build() from
   the bit lengths.  That routine and its comments are in the inflate.c
   module.
 */

#define __EXPLODE_C     /* identifies this source module */
#define UNZIP_INTERNAL
#include "third_party/unzip/unzip.h"      /* must supply slide[] (uch) array and NEXTBYTE macro */
#include "third_party/unzip/globals.h"

#ifndef WSIZE
#  define WSIZE 0x8000  /* window size--must be a power of two, and */
#endif                  /* at least 8K for zip's implode method */

#if (defined(DLL) && !defined(NO_SLIDE_REDIR))
#  define wszimpl (unsigned)(G._wsize)
#else
#  if defined(USE_DEFLATE64) && defined(INT_16BIT)
#    define wszimpl (unsigned)(WSIZE>>1)
#  else /* !(USE_DEFLATE64 && INT_16BIT) */
#    define wszimpl WSIZE
#  endif /* !(USE_DEFLATE64 && INT_16BIT) */
#endif

/* routines here */
static int get_tree OF((__GPRO__ unsigned *l, unsigned n));
static int explode_lit OF((__GPRO__ struct huft *tb, struct huft *tl,
                           struct huft *td, unsigned bb, unsigned bl,
                           unsigned bd, unsigned bdl));
static int explode_nolit OF((__GPRO__ struct huft *tl, struct huft *td,
                             unsigned bl, unsigned bd, unsigned bdl));


/* The implode algorithm uses a sliding 4K or 8K byte window on the
   uncompressed stream to find repeated byte strings.  This is implemented
   here as a circular buffer.  The index is updated simply by incrementing
   and then and'ing with 0x0fff (4K-1) or 0x1fff (8K-1).  Here, the 32K
   buffer of inflate is used, and it works just as well to always have
   a 32K circular buffer, so the index is anded with 0x7fff.  This is
   done to allow the window to also be used as the output buffer. */
/* This must be supplied in an external module useable like "uch slide[8192];"
   or "uch *slide;", where the latter would be malloc'ed.  In unzip, slide[]
   is actually a 32K area for use by inflate, which uses a 32K sliding window.
 */


#define INVALID_CODE 99
#define IS_INVALID_CODE(c)  ((c) == INVALID_CODE)

/* Tables for length and distance */
static ZCONST ush cplen2[] =
        {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
        18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34,
        35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65};
static ZCONST ush cplen3[] =
        {3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
        19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
        36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52,
        53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66};
static ZCONST uch extra[] =
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        8};
static ZCONST ush cpdist4[] =
        {1, 65, 129, 193, 257, 321, 385, 449, 513, 577, 641, 705,
        769, 833, 897, 961, 1025, 1089, 1153, 1217, 1281, 1345, 1409, 1473,
        1537, 1601, 1665, 1729, 1793, 1857, 1921, 1985, 2049, 2113, 2177,
        2241, 2305, 2369, 2433, 2497, 2561, 2625, 2689, 2753, 2817, 2881,
        2945, 3009, 3073, 3137, 3201, 3265, 3329, 3393, 3457, 3521, 3585,
        3649, 3713, 3777, 3841, 3905, 3969, 4033};
static ZCONST ush cpdist8[] =
        {1, 129, 257, 385, 513, 641, 769, 897, 1025, 1153, 1281,
        1409, 1537, 1665, 1793, 1921, 2049, 2177, 2305, 2433, 2561, 2689,
        2817, 2945, 3073, 3201, 3329, 3457, 3585, 3713, 3841, 3969, 4097,
        4225, 4353, 4481, 4609, 4737, 4865, 4993, 5121, 5249, 5377, 5505,
        5633, 5761, 5889, 6017, 6145, 6273, 6401, 6529, 6657, 6785, 6913,
        7041, 7169, 7297, 7425, 7553, 7681, 7809, 7937, 8065};


/* Macros for inflate() bit peeking and grabbing.
   The usage is:

        NEEDBITS(j)
        x = b & mask_bits[j];
        DUMPBITS(j)

   where NEEDBITS makes sure that b has at least j bits in it, and
   DUMPBITS removes the bits from b.  The macros use the variable k
   for the number of bits in b.  Normally, b and k are register
   variables for speed.
 */

#define NEEDBITS(n) {while(k<(n)){b|=((ulg)NEXTBYTE)<<k;k+=8;}}
#define DUMPBITS(n) {b>>=(n);k-=(n);}

#define DECODEHUFT(htab, bits, mask) {\
  NEEDBITS((unsigned)(bits))\
  t = (htab) + ((~(unsigned)b)&(mask));\
  while (1) {\
    DUMPBITS(t->b)\
    if ((e=t->e) <= 32) break;\
    if (IS_INVALID_CODE(e)) return 1;\
    e &= 31;\
    NEEDBITS(e)\
    t = t->v.t + ((~(unsigned)b)&mask_bits[e]);\
  }\
}


static int get_tree(__G__ l, n)
     __GDEF
unsigned *l;            /* bit lengths */
unsigned n;             /* number expected */
/* Get the bit lengths for a code representation from the compressed
   stream.  If get_tree() returns 4, then there is an error in the data.
   Otherwise zero is returned. */
{
  unsigned i;           /* bytes remaining in list */
  unsigned k;           /* lengths entered */
  unsigned j;           /* number of codes */
  unsigned b;           /* bit length for those codes */


  /* get bit lengths */
  i = NEXTBYTE + 1;                     /* length/count pairs to read */
  k = 0;                                /* next code */
  do {
    b = ((j = NEXTBYTE) & 0xf) + 1;     /* bits in code (1..16) */
    j = ((j & 0xf0) >> 4) + 1;          /* codes with those bits (1..16) */
    if (k + j > n)
      return 4;                         /* don't overflow l[] */
    do {
      l[k++] = b;
    } while (--j);
  } while (--i);
  return k != n ? 4 : 0;                /* should have read n of them */
}



static int explode_lit(__G__ tb, tl, td, bb, bl, bd, bdl)
     __GDEF
struct huft *tb, *tl, *td;      /* literal, length, and distance tables */
unsigned bb, bl, bd;            /* number of bits decoded by those */
unsigned bdl;                   /* number of distance low bits */
/* Decompress the imploded data using coded literals and a sliding
   window (of size 2^(6+bdl) bytes). */
{
  zusz_t s;             /* bytes to decompress */
  register unsigned e;  /* table entry flag/number of extra bits */
  unsigned n, d;        /* length and index for copy */
  unsigned w;           /* current window position */
  struct huft *t;       /* pointer to table entry */
  unsigned mb, ml, md;  /* masks for bb, bl, and bd bits */
  unsigned mdl;         /* mask for bdl (distance lower) bits */
  register ulg b;       /* bit buffer */
  register unsigned k;  /* number of bits in bit buffer */
  unsigned u;           /* true if unflushed */
  int retval = 0;       /* error code returned: initialized to "no error" */


  /* explode the coded data */
  b = k = w = 0;                /* initialize bit buffer, window */
  u = 1;                        /* buffer unflushed */
  mb = mask_bits[bb];           /* precompute masks for speed */
  ml = mask_bits[bl];
  md = mask_bits[bd];
  mdl = mask_bits[bdl];
  s = G.lrec.ucsize;
  while (s > 0)                 /* do until ucsize bytes uncompressed */
  {
    NEEDBITS(1)
    if (b & 1)                  /* then literal--decode it */
    {
      DUMPBITS(1)
      s--;
      DECODEHUFT(tb, bb, mb)    /* get coded literal */
      redirSlide[w++] = (uch)t->v.n;
      if (w == wszimpl)
      {
        if ((retval = flush(__G__ redirSlide, (ulg)w, 0)) != 0)
          return retval;
        w = u = 0;
      }
    }
    else                        /* else distance/length */
    {
      DUMPBITS(1)
      NEEDBITS(bdl)             /* get distance low bits */
      d = (unsigned)b & mdl;
      DUMPBITS(bdl)
      DECODEHUFT(td, bd, md)    /* get coded distance high bits */
      d = w - d - t->v.n;       /* construct offset */
      DECODEHUFT(tl, bl, ml)    /* get coded length */
      n = t->v.n;
      if (e)                    /* get length extra bits */
      {
        NEEDBITS(8)
        n += (unsigned)b & 0xff;
        DUMPBITS(8)
      }

      /* do the copy */
      s = (s > (zusz_t)n ? s - (zusz_t)n : 0);
      do {
#if (defined(DLL) && !defined(NO_SLIDE_REDIR))
        if (G.redirect_slide) {
          /* &= w/ wszimpl not needed and wrong if redirect */
          if (d >= wszimpl)
            return 1;
          e = wszimpl - (d > w ? d : w);
        } else
#endif
          e = wszimpl - ((d &= wszimpl-1) > w ? d : w);
        if (e > n) e = n;
        n -= e;
        if (u && w <= d)
        {
          memzero(redirSlide + w, e);
          w += e;
          d += e;
        }
        else
#ifndef NOMEMCPY
          if (w - d >= e)       /* (this test assumes unsigned comparison) */
          {
            memcpy(redirSlide + w, redirSlide + d, e);
            w += e;
            d += e;
          }
          else                  /* do it slow to avoid memcpy() overlap */
#endif /* !NOMEMCPY */
            do {
              redirSlide[w++] = redirSlide[d++];
            } while (--e);
        if (w == wszimpl)
        {
          if ((retval = flush(__G__ redirSlide, (ulg)w, 0)) != 0)
            return retval;
          w = u = 0;
        }
      } while (n);
    }
  }

  /* flush out redirSlide */
  if ((retval = flush(__G__ redirSlide, (ulg)w, 0)) != 0)
    return retval;
  if (G.csize + G.incnt + (k >> 3))   /* should have read csize bytes, but */
  {                        /* sometimes read one too many:  k>>3 compensates */
    G.used_csize = G.lrec.csize - G.csize - G.incnt - (k >> 3);
    return 5;
  }
  return 0;
}



static int explode_nolit(__G__ tl, td, bl, bd, bdl)
     __GDEF
struct huft *tl, *td;   /* length and distance decoder tables */
unsigned bl, bd;        /* number of bits decoded by tl[] and td[] */
unsigned bdl;           /* number of distance low bits */
/* Decompress the imploded data using uncoded literals and a sliding
   window (of size 2^(6+bdl) bytes). */
{
  zusz_t s;             /* bytes to decompress */
  register unsigned e;  /* table entry flag/number of extra bits */
  unsigned n, d;        /* length and index for copy */
  unsigned w;           /* current window position */
  struct huft *t;       /* pointer to table entry */
  unsigned ml, md;      /* masks for bl and bd bits */
  unsigned mdl;         /* mask for bdl (distance lower) bits */
  register ulg b;       /* bit buffer */
  register unsigned k;  /* number of bits in bit buffer */
  unsigned u;           /* true if unflushed */
  int retval = 0;       /* error code returned: initialized to "no error" */


  /* explode the coded data */
  b = k = w = 0;                /* initialize bit buffer, window */
  u = 1;                        /* buffer unflushed */
  ml = mask_bits[bl];           /* precompute masks for speed */
  md = mask_bits[bd];
  mdl = mask_bits[bdl];
  s = G.lrec.ucsize;
  while (s > 0)                 /* do until ucsize bytes uncompressed */
  {
    NEEDBITS(1)
    if (b & 1)                  /* then literal--get eight bits */
    {
      DUMPBITS(1)
      s--;
      NEEDBITS(8)
      redirSlide[w++] = (uch)b;
      if (w == wszimpl)
      {
        if ((retval = flush(__G__ redirSlide, (ulg)w, 0)) != 0)
          return retval;
        w = u = 0;
      }
      DUMPBITS(8)
    }
    else                        /* else distance/length */
    {
      DUMPBITS(1)
      NEEDBITS(bdl)             /* get distance low bits */
      d = (unsigned)b & mdl;
      DUMPBITS(bdl)
      DECODEHUFT(td, bd, md)    /* get coded distance high bits */
      d = w - d - t->v.n;       /* construct offset */
      DECODEHUFT(tl, bl, ml)    /* get coded length */
      n = t->v.n;
      if (e)                    /* get length extra bits */
      {
        NEEDBITS(8)
        n += (unsigned)b & 0xff;
        DUMPBITS(8)
      }

      /* do the copy */
      s = (s > (zusz_t)n ? s - (zusz_t)n : 0);
      do {
#if (defined(DLL) && !defined(NO_SLIDE_REDIR))
        if (G.redirect_slide) {
          /* &= w/ wszimpl not needed and wrong if redirect */
          if (d >= wszimpl)
            return 1;
          e = wszimpl - (d > w ? d : w);
        } else
#endif
          e = wszimpl - ((d &= wszimpl-1) > w ? d : w);
        if (e > n) e = n;
        n -= e;
        if (u && w <= d)
        {
          memzero(redirSlide + w, e);
          w += e;
          d += e;
        }
        else
#ifndef NOMEMCPY
          if (w - d >= e)       /* (this test assumes unsigned comparison) */
          {
            memcpy(redirSlide + w, redirSlide + d, e);
            w += e;
            d += e;
          }
          else                  /* do it slow to avoid memcpy() overlap */
#endif /* !NOMEMCPY */
            do {
              redirSlide[w++] = redirSlide[d++];
            } while (--e);
        if (w == wszimpl)
        {
          if ((retval = flush(__G__ redirSlide, (ulg)w, 0)) != 0)
            return retval;
          w = u = 0;
        }
      } while (n);
    }
  }

  /* flush out redirSlide */
  if ((retval = flush(__G__ redirSlide, (ulg)w, 0)) != 0)
    return retval;
  if (G.csize + G.incnt + (k >> 3))   /* should have read csize bytes, but */
  {                        /* sometimes read one too many:  k>>3 compensates */
    G.used_csize = G.lrec.csize - G.csize - G.incnt - (k >> 3);
    return 5;
  }
  return 0;
}



int explode(__G)
     __GDEF
/* Explode an imploded compressed stream.  Based on the general purpose
   bit flag, decide on coded or uncoded literals, and an 8K or 4K sliding
   window.  Construct the literal (if any), length, and distance codes and
   the tables needed to decode them (using huft_build() from inflate.c),
   and call the appropriate routine for the type of data in the remainder
   of the stream.  The four routines are nearly identical, differing only
   in whether the literal is decoded or simply read in, and in how many
   bits are read in, uncoded, for the low distance bits. */
{
  unsigned r;           /* return codes */
  struct huft *tb;      /* literal code table */
  struct huft *tl;      /* length code table */
  struct huft *td;      /* distance code table */
  unsigned bb;          /* bits for tb */
  unsigned bl;          /* bits for tl */
  unsigned bd;          /* bits for td */
  unsigned bdl;         /* number of uncoded lower distance bits */
  unsigned l[256];      /* bit lengths for codes */

#if (defined(DLL) && !defined(NO_SLIDE_REDIR))
  if (G.redirect_slide)
    /* For 16-bit systems, it has already been checked at DLL entrance that
     * the buffer size in G.redirect_size does not exceed unsigned range.
     */
    G._wsize = G.redirect_size, redirSlide = G.redirect_buffer;
  else
#if defined(USE_DEFLATE64) && defined(INT_16BIT)
    /* For systems using 16-bit ints, reduce the used buffer size below
     * the limit of "unsigned int" numbers range.
     */
    G._wsize = WSIZE>>1, redirSlide = slide;
#else /* !(USE_DEFLATE64 && INT_16BIT) */
    G._wsize = WSIZE, redirSlide = slide;
#endif /* !(USE_DEFLATE64 && INT_16BIT) */
#endif /* DLL && !NO_SLIDE_REDIR */

  /* Tune base table sizes.  Note: I thought that to truly optimize speed,
     I would have to select different bl, bd, and bb values for different
     compressed file sizes.  I was surprised to find out that the values of
     7, 7, and 9 worked best over a very wide range of sizes, except that
     bd = 8 worked marginally better for large compressed sizes. */
  bl = 7;
  bd = (G.csize + G.incnt) > 200000L ? 8 : 7;

#ifdef DEBUG
  G.hufts = 0;                    /* initialize huft's malloc'ed */
#endif

  if (G.lrec.general_purpose_bit_flag & 4)
  /* With literal tree--minimum match length is 3 */
  {
    bb = 9;                     /* base table size for literals */
    if ((r = get_tree(__G__ l, 256)) != 0)
      return (int)r;
    if ((r = huft_build(__G__ l, 256, 256, NULL, NULL, &tb, &bb)) != 0)
    {
      if (r == 1)
        huft_free(tb);
      return (int)r;
    }
    if ((r = get_tree(__G__ l, 64)) != 0) {
      huft_free(tb);
      return (int)r;
    }
    if ((r = huft_build(__G__ l, 64, 0, cplen3, extra, &tl, &bl)) != 0)
    {
      if (r == 1)
        huft_free(tl);
      huft_free(tb);
      return (int)r;
    }
  }
  else
  /* No literal tree--minimum match length is 2 */
  {
    tb = (struct huft *)NULL;
    if ((r = get_tree(__G__ l, 64)) != 0)
      return (int)r;
    if ((r = huft_build(__G__ l, 64, 0, cplen2, extra, &tl, &bl)) != 0)
    {
      if (r == 1)
        huft_free(tl);
      return (int)r;
    }
  }

  if ((r = get_tree(__G__ l, 64)) != 0) {
    huft_free(tl);
    if (tb != (struct huft *)NULL) huft_free(tb);
    return (int)r;
  }
  if (G.lrec.general_purpose_bit_flag & 2)      /* true if 8K */
  {
    bdl = 7;
    r = huft_build(__G__ l, 64, 0, cpdist8, extra, &td, &bd);
  }
  else                                          /* else 4K */
  {
    bdl = 6;
    r = huft_build(__G__ l, 64, 0, cpdist4, extra, &td, &bd);
  }
  if (r != 0)
  {
    if (r == 1)
      huft_free(td);
    huft_free(tl);
    if (tb != (struct huft *)NULL) huft_free(tb);
    return (int)r;
  }

  if (tb != NULL) {
    r = explode_lit(__G__ tb, tl, td, bb, bl, bd, bdl);
    huft_free(tb);
  } else {
    r = explode_nolit(__G__ tl, td, bl, bd, bdl);
  }

  huft_free(td);
  huft_free(tl);
  Trace((stderr, "<%u > ", G.hufts));
  return (int)r;
}

/* so explode.c and inflate.c can be compiled together into one object: */
#undef DECODEHUFT
#undef NEEDBITS
#undef DUMPBITS
#undef wszimpl
