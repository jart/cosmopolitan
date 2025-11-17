/*
  trees.h - Zip 3

  Copyright (c) 1990-2007 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2005-Feb-10 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/*
 *  trees.c by Jean-loup Gailly
 *
 *  This is a new version of im_ctree.c originally written by Richard B. Wales
 *  for the defunct implosion method.
 *  The low level bit string handling routines from bits.c (originally
 *  im_bits.c written by Richard B. Wales) have been merged into this version
 *  of trees.c.
 *
 *  PURPOSE
 *
 *      Encode various sets of source values using variable-length
 *      binary code trees.
 *      Output the resulting variable-length bit strings.
 *      Compression can be done to a file or to memory.
 *
 *  DISCUSSION
 *
 *      The PKZIP "deflation" process uses several Huffman trees. The more
 *      common source values are represented by shorter bit sequences.
 *
 *      Each code tree is stored in the ZIP file in a compressed form
 *      which is itself a Huffman encoding of the lengths of
 *      all the code strings (in ascending order by source values).
 *      The actual code strings are reconstructed from the lengths in
 *      the UNZIP process, as described in the "application note"
 *      (APPNOTE.TXT) distributed as part of PKWARE's PKZIP program.
 *
 *      The PKZIP "deflate" file format interprets compressed file data
 *      as a sequence of bits.  Multi-bit strings in the file may cross
 *      byte boundaries without restriction.
 *      The first bit of each byte is the low-order bit.
 *
 *      The routines in this file allow a variable-length bit value to
 *      be output right-to-left (useful for literal values). For
 *      left-to-right output (useful for code strings from the tree routines),
 *      the bits must have been reversed first with bi_reverse().
 *
 *      For in-memory compression, the compressed bit stream goes directly
 *      into the requested output buffer. The buffer is limited to 64K on
 *      16 bit machines; flushing of the output buffer during compression
 *      process is not supported.
 *      The input data is read in blocks by the (*read_buf)() function.
 *
 *      For more details about input to and output from the deflation routines,
 *      see the actual input functions for (*read_buf)(), flush_outbuf(), and
 *      the filecompress() resp. memcompress() wrapper functions which handle
 *      the I/O setup.
 *
 *  REFERENCES
 *
 *      Lynch, Thomas J.
 *          Data Compression:  Techniques and Applications, pp. 53-55.
 *          Lifetime Learning Publications, 1985.  ISBN 0-534-03418-7.
 *
 *      Storer, James A.
 *          Data Compression:  Methods and Theory, pp. 49-50.
 *          Computer Science Press, 1988.  ISBN 0-7167-8156-5.
 *
 *      Sedgewick, R.
 *          Algorithms, p290.
 *          Addison-Wesley, 1983. ISBN 0-201-06672-6.
 *
 *  INTERFACE
 *
 *      void ct_init (ush *attr, int *method)
 *          Allocate the match buffer, initialize the various tables and save
 *          the location of the internal file attribute (ascii/binary) and
 *          method (DEFLATE/STORE)
 *
 *      void ct_tally (int dist, int lc);
 *          Save the match info and tally the frequency counts.
 *
 *      uzoff_t flush_block (char *buf, ulg stored_len, int eof)
 *          Determine the best encoding for the current block: dynamic trees,
 *          static trees or store, and output the encoded block to the zip
 *          file. Returns the total compressed length for the file so far.
 *
 *      void bi_init (char *tgt_buf, unsigned tgt_size, int flsh_allowed)
 *          Initialize the bit string routines.
 *
 *    Most of the bit string output functions are only used internally
 *    in this source file, they are normally declared as "local" routines:
 *
 *      local void send_bits (int value, int length)
 *          Write out a bit string, taking the source bits right to
 *          left.
 *
 *      local unsigned bi_reverse (unsigned code, int len)
 *          Reverse the bits of a bit string, taking the source bits left to
 *          right and emitting them right to left.
 *
 *      local void bi_windup (void)
 *          Write out any remaining bits in an incomplete byte.
 *
 *      local void copy_block(char *buf, unsigned len, int header)
 *          Copy a stored block to the zip file, storing first the length and
 *          its one's complement if requested.
 *
 *    All output that exceeds the bitstring output buffer size (as initialized
 *    by bi_init() is fed through an externally provided transfer routine
 *    which flushes the bitstring output buffer on request and resets the
 *    buffer fill counter:
 *
 *      extern void flush_outbuf(char *o_buf, unsigned *o_idx);
 *
 */
#define __TREES_C

/* Put zip.h first as when using 64-bit file environment in unix ctype.h
   defines off_t and then while other files are using an 8-byte off_t this
   file gets a 4-byte off_t.  Once zip.h sets the large file defines can
   then include ctype.h and get 8-byte off_t.  8/14/04 EG */
#include "zip.h"
#include <ctype.h>

#ifndef USE_ZLIB

/* ===========================================================================
 * Constants
 */

#define MAX_BITS 15
/* All codes must not exceed MAX_BITS bits */

#define MAX_BL_BITS 7
/* Bit length codes must not exceed MAX_BL_BITS bits */

#define LENGTH_CODES 29
/* number of length codes, not counting the special END_BLOCK code */

#define LITERALS  256
/* number of literal bytes 0..255 */

#define END_BLOCK 256
/* end of block literal code */

#define L_CODES (LITERALS+1+LENGTH_CODES)
/* number of Literal or Length codes, including the END_BLOCK code */

#define D_CODES   30
/* number of distance codes */

#define BL_CODES  19
/* number of codes used to transfer the bit lengths */


local int near extra_lbits[LENGTH_CODES] /* extra bits for each length code */
   = {0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0};

local int near extra_dbits[D_CODES] /* extra bits for each distance code */
   = {0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13};

local int near extra_blbits[BL_CODES]/* extra bits for each bit length code */
   = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,3,7};

#define STORED_BLOCK 0
#define STATIC_TREES 1
#define DYN_TREES    2
/* The three kinds of block type */

#ifndef LIT_BUFSIZE
#  ifdef SMALL_MEM
#    define LIT_BUFSIZE  0x2000
#  else
#  ifdef MEDIUM_MEM
#    define LIT_BUFSIZE  0x4000
#  else
#    define LIT_BUFSIZE  0x8000
#  endif
#  endif
#endif
#define DIST_BUFSIZE  LIT_BUFSIZE
/* Sizes of match buffers for literals/lengths and distances.  There are
 * 4 reasons for limiting LIT_BUFSIZE to 64K:
 *   - frequencies can be kept in 16 bit counters
 *   - if compression is not successful for the first block, all input data is
 *     still in the window so we can still emit a stored block even when input
 *     comes from standard input.  (This can also be done for all blocks if
 *     LIT_BUFSIZE is not greater than 32K.)
 *   - if compression is not successful for a file smaller than 64K, we can
 *     even emit a stored file instead of a stored block (saving 5 bytes).
 *   - creating new Huffman trees less frequently may not provide fast
 *     adaptation to changes in the input data statistics. (Take for
 *     example a binary file with poorly compressible code followed by
 *     a highly compressible string table.) Smaller buffer sizes give
 *     fast adaptation but have of course the overhead of transmitting trees
 *     more frequently.
 *   - I can't count above 4
 * The current code is general and allows DIST_BUFSIZE < LIT_BUFSIZE (to save
 * memory at the expense of compression). Some optimizations would be possible
 * if we rely on DIST_BUFSIZE == LIT_BUFSIZE.
 */

#define REP_3_6      16
/* repeat previous bit length 3-6 times (2 bits of repeat count) */

#define REPZ_3_10    17
/* repeat a zero length 3-10 times  (3 bits of repeat count) */

#define REPZ_11_138  18
/* repeat a zero length 11-138 times  (7 bits of repeat count) */

/* ===========================================================================
 * Local data
 */

/* Data structure describing a single value and its code string. */
typedef struct ct_data {
    union {
        ush  freq;       /* frequency count */
        ush  code;       /* bit string */
    } fc;
    union {
        ush  dad;        /* father node in Huffman tree */
        ush  len;        /* length of bit string */
    } dl;
} ct_data;

#define Freq fc.freq
#define Code fc.code
#define Dad  dl.dad
#define Len  dl.len

#define HEAP_SIZE (2*L_CODES+1)
/* maximum heap size */

local ct_data near dyn_ltree[HEAP_SIZE];   /* literal and length tree */
local ct_data near dyn_dtree[2*D_CODES+1]; /* distance tree */

local ct_data near static_ltree[L_CODES+2];
/* The static literal tree. Since the bit lengths are imposed, there is no
 * need for the L_CODES extra codes used during heap construction. However
 * The codes 286 and 287 are needed to build a canonical tree (see ct_init
 * below).
 */

local ct_data near static_dtree[D_CODES];
/* The static distance tree. (Actually a trivial tree since all codes use
 * 5 bits.)
 */

local ct_data near bl_tree[2*BL_CODES+1];
/* Huffman tree for the bit lengths */

typedef struct tree_desc {
    ct_data near *dyn_tree;      /* the dynamic tree */
    ct_data near *static_tree;   /* corresponding static tree or NULL */
    int     near *extra_bits;    /* extra bits for each code or NULL */
    int     extra_base;          /* base index for extra_bits */
    int     elems;               /* max number of elements in the tree */
    int     max_length;          /* max bit length for the codes */
    int     max_code;            /* largest code with non zero frequency */
} tree_desc;

local tree_desc near l_desc =
{dyn_ltree, static_ltree, extra_lbits, LITERALS+1, L_CODES, MAX_BITS, 0};

local tree_desc near d_desc =
{dyn_dtree, static_dtree, extra_dbits, 0,          D_CODES, MAX_BITS, 0};

local tree_desc near bl_desc =
{bl_tree, NULL,       extra_blbits, 0,         BL_CODES, MAX_BL_BITS, 0};


local ush near bl_count[MAX_BITS+1];
/* number of codes at each bit length for an optimal tree */

local uch near bl_order[BL_CODES]
   = {16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15};
/* The lengths of the bit length codes are sent in order of decreasing
 * probability, to avoid transmitting the lengths for unused bit length codes.
 */

local int near heap[2*L_CODES+1]; /* heap used to build the Huffman trees */
local int heap_len;               /* number of elements in the heap */
local int heap_max;               /* element of largest frequency */
/* The sons of heap[n] are heap[2*n] and heap[2*n+1]. heap[0] is not used.
 * The same heap array is used to build all trees.
 */

local uch near depth[2*L_CODES+1];
/* Depth of each subtree used as tie breaker for trees of equal frequency */

local uch length_code[MAX_MATCH-MIN_MATCH+1];
/* length code for each normalized match length (0 == MIN_MATCH) */

local uch dist_code[512];
/* distance codes. The first 256 values correspond to the distances
 * 3 .. 258, the last 256 values correspond to the top 8 bits of
 * the 15 bit distances.
 */

local int near base_length[LENGTH_CODES];
/* First normalized length for each code (0 = MIN_MATCH) */

local int near base_dist[D_CODES];
/* First normalized distance for each code (0 = distance of 1) */

#ifndef DYN_ALLOC
  local uch far l_buf[LIT_BUFSIZE];  /* buffer for literals/lengths */
  local ush far d_buf[DIST_BUFSIZE]; /* buffer for distances */
#else
  local uch far *l_buf;
  local ush far *d_buf;
#endif

local uch near flag_buf[(LIT_BUFSIZE/8)];
/* flag_buf is a bit array distinguishing literals from lengths in
 * l_buf, and thus indicating the presence or absence of a distance.
 */

local unsigned last_lit;    /* running index in l_buf */
local unsigned last_dist;   /* running index in d_buf */
local unsigned last_flags;  /* running index in flag_buf */
local uch flags;            /* current flags not yet saved in flag_buf */
local uch flag_bit;         /* current bit used in flags */
/* bits are filled in flags starting at bit 0 (least significant).
 * Note: these flags are overkill in the current code since we don't
 * take advantage of DIST_BUFSIZE == LIT_BUFSIZE.
 */

local ulg opt_len;        /* bit length of current block with optimal trees */
local ulg static_len;     /* bit length of current block with static trees */

/* zip64 support 08/29/2003 R.Nausedat */
/* now all file sizes and offsets are zoff_t 7/24/04 EG */
local uzoff_t cmpr_bytelen;     /* total byte length of compressed file */
local ulg cmpr_len_bits;        /* number of bits past 'cmpr_bytelen' */

#ifdef DEBUG
local uzoff_t input_len;        /* total byte length of input file */
/* input_len is for debugging only since we can get it by other means. */
#endif

local ush *file_type;       /* pointer to UNKNOWN, BINARY or ASCII */
local int *file_method;     /* pointer to DEFLATE or STORE */

/* ===========================================================================
 * Local data used by the "bit string" routines.
 */

local int flush_flg;

#if (!defined(ASMV) || !defined(RISCOS))
local unsigned bi_buf;
#else
unsigned bi_buf;
#endif
/* Output buffer. bits are inserted starting at the bottom (least significant
 * bits). The width of bi_buf must be at least 16 bits.
 */

#define Buf_size (8 * 2*sizeof(char))
/* Number of bits used within bi_buf. (bi_buf may be implemented on
 * more than 16 bits on some systems.)
 */

#if (!defined(ASMV) || !defined(RISCOS))
local int bi_valid;
#else
int bi_valid;
#endif
/* Number of valid bits in bi_buf.  All bits above the last valid bit
 * are always zero.
 */

#if (!defined(ASMV) || !defined(RISCOS))
local char *out_buf;
#else
char *out_buf;
#endif
/* Current output buffer. */

#if (!defined(ASMV) || !defined(RISCOS))
local unsigned out_offset;
#else
unsigned out_offset;
#endif
/* Current offset in output buffer.
 * On 16 bit machines, the buffer is limited to 64K.
 */

#if !defined(ASMV) || !defined(RISCOS)
local unsigned out_size;
#else
unsigned out_size;
#endif
/* Size of current output buffer */

/* Output a 16 bit value to the bit stream, lower (oldest) byte first */
#define PUTSHORT(w) \
{ if (out_offset >= out_size-1) \
    flush_outbuf(out_buf, &out_offset); \
  out_buf[out_offset++] = (char) ((w) & 0xff); \
  out_buf[out_offset++] = (char) ((ush)(w) >> 8); \
}

#define PUTBYTE(b) \
{ if (out_offset >= out_size) \
    flush_outbuf(out_buf, &out_offset); \
  out_buf[out_offset++] = (char) (b); \
}

#ifdef DEBUG
local uzoff_t bits_sent;   /* bit length of the compressed data */
extern uzoff_t isize;      /* byte length of input file */
#endif

extern long block_start;       /* window offset of current block */
extern unsigned near strstart; /* window offset of current string */


/* ===========================================================================
 * Local (static) routines in this file.
 */

local void init_block     OF((void));
local void pqdownheap     OF((ct_data near *tree, int k));
local void gen_bitlen     OF((tree_desc near *desc));
local void gen_codes      OF((ct_data near *tree, int max_code));
local void build_tree     OF((tree_desc near *desc));
local void scan_tree      OF((ct_data near *tree, int max_code));
local void send_tree      OF((ct_data near *tree, int max_code));
local int  build_bl_tree  OF((void));
local void send_all_trees OF((int lcodes, int dcodes, int blcodes));
local void compress_block OF((ct_data near *ltree, ct_data near *dtree));
local void set_file_type  OF((void));
#if (!defined(ASMV) || !defined(RISCOS))
local void send_bits      OF((int value, int length));
local unsigned bi_reverse OF((unsigned code, int len));
#endif
local void bi_windup      OF((void));
local void copy_block     OF((char *buf, unsigned len, int header));


#ifndef DEBUG
#  define send_code(c, tree) send_bits(tree[c].Code, tree[c].Len)
   /* Send a code of the given tree. c and tree must not have side effects */

#else /* DEBUG */
#  define send_code(c, tree) \
     { if (verbose>1) fprintf(mesg,"\ncd %3d ",(c)); \
       send_bits(tree[c].Code, tree[c].Len); }
#endif

#define d_code(dist) \
   ((dist) < 256 ? dist_code[dist] : dist_code[256+((dist)>>7)])
/* Mapping from a distance to a distance code. dist is the distance - 1 and
 * must not have side effects. dist_code[256] and dist_code[257] are never
 * used.
 */

#define Max(a,b) (a >= b ? a : b)
/* the arguments must not have side effects */

/* ===========================================================================
 * Allocate the match buffer, initialize the various tables and save the
 * location of the internal file attribute (ascii/binary) and method
 * (DEFLATE/STORE).
 */
void ct_init(attr, method)
    ush  *attr;   /* pointer to internal file attribute */
    int  *method; /* pointer to compression method */
{
    int n;        /* iterates over tree elements */
    int bits;     /* bit counter */
    int length;   /* length value */
    int code;     /* code value */
    int dist;     /* distance index */

    file_type = attr;
    file_method = method;
    cmpr_len_bits = 0L;
    cmpr_bytelen = (uzoff_t)0;
#ifdef DEBUG
    input_len = (uzoff_t)0;
#endif

    if (static_dtree[0].Len != 0) return; /* ct_init already called */

#ifdef DYN_ALLOC
    d_buf = (ush far *) zcalloc(DIST_BUFSIZE, sizeof(ush));
    l_buf = (uch far *) zcalloc(LIT_BUFSIZE/2, 2);
    /* Avoid using the value 64K on 16 bit machines */
    if (l_buf == NULL || d_buf == NULL)
        ziperr(ZE_MEM, "ct_init: out of memory");
#endif

    /* Initialize the mapping length (0..255) -> length code (0..28) */
    length = 0;
    for (code = 0; code < LENGTH_CODES-1; code++) {
        base_length[code] = length;
        for (n = 0; n < (1<<extra_lbits[code]); n++) {
            length_code[length++] = (uch)code;
        }
    }
    Assert(length == 256, "ct_init: length != 256");
    /* Note that the length 255 (match length 258) can be represented
     * in two different ways: code 284 + 5 bits or code 285, so we
     * overwrite length_code[255] to use the best encoding:
     */
    length_code[length-1] = (uch)code;

    /* Initialize the mapping dist (0..32K) -> dist code (0..29) */
    dist = 0;
    for (code = 0 ; code < 16; code++) {
        base_dist[code] = dist;
        for (n = 0; n < (1<<extra_dbits[code]); n++) {
            dist_code[dist++] = (uch)code;
        }
    }
    Assert(dist == 256, "ct_init: dist != 256");
    dist >>= 7; /* from now on, all distances are divided by 128 */
    for ( ; code < D_CODES; code++) {
        base_dist[code] = dist << 7;
        for (n = 0; n < (1<<(extra_dbits[code]-7)); n++) {
            dist_code[256 + dist++] = (uch)code;
        }
    }
    Assert(dist == 256, "ct_init: 256+dist != 512");

    /* Construct the codes of the static literal tree */
    for (bits = 0; bits <= MAX_BITS; bits++) bl_count[bits] = 0;
    n = 0;
    while (n <= 143) static_ltree[n++].Len = 8, bl_count[8]++;
    while (n <= 255) static_ltree[n++].Len = 9, bl_count[9]++;
    while (n <= 279) static_ltree[n++].Len = 7, bl_count[7]++;
    while (n <= 287) static_ltree[n++].Len = 8, bl_count[8]++;
    /* Codes 286 and 287 do not exist, but we must include them in the
     * tree construction to get a canonical Huffman tree (longest code
     * all ones)
     */
    gen_codes((ct_data near *)static_ltree, L_CODES+1);

    /* The static distance tree is trivial: */
    for (n = 0; n < D_CODES; n++) {
        static_dtree[n].Len = 5;
        static_dtree[n].Code = (ush)bi_reverse(n, 5);
    }

    /* Initialize the first block of the first file: */
    init_block();
}

/* ===========================================================================
 * Initialize a new block.
 */
local void init_block()
{
    int n; /* iterates over tree elements */

    /* Initialize the trees. */
    for (n = 0; n < L_CODES;  n++) dyn_ltree[n].Freq = 0;
    for (n = 0; n < D_CODES;  n++) dyn_dtree[n].Freq = 0;
    for (n = 0; n < BL_CODES; n++) bl_tree[n].Freq = 0;

    dyn_ltree[END_BLOCK].Freq = 1;
    opt_len = static_len = 0L;
    last_lit = last_dist = last_flags = 0;
    flags = 0; flag_bit = 1;
}

#define SMALLEST 1
/* Index within the heap array of least frequent node in the Huffman tree */


/* ===========================================================================
 * Remove the smallest element from the heap and recreate the heap with
 * one less element. Updates heap and heap_len.
 */
#define pqremove(tree, top) \
{\
    top = heap[SMALLEST]; \
    heap[SMALLEST] = heap[heap_len--]; \
    pqdownheap(tree, SMALLEST); \
}

/* ===========================================================================
 * Compares to subtrees, using the tree depth as tie breaker when
 * the subtrees have equal frequency. This minimizes the worst case length.
 */
#define smaller(tree, n, m) \
   (tree[n].Freq < tree[m].Freq || \
   (tree[n].Freq == tree[m].Freq && depth[n] <= depth[m]))

/* ===========================================================================
 * Restore the heap property by moving down the tree starting at node k,
 * exchanging a node with the smallest of its two sons if necessary, stopping
 * when the heap property is re-established (each father smaller than its
 * two sons).
 */
local void pqdownheap(tree, k)
    ct_data near *tree;  /* the tree to restore */
    int k;               /* node to move down */
{
    int v = heap[k];
    int j = k << 1;  /* left son of k */
    int htemp;       /* required because of bug in SASC compiler */

    while (j <= heap_len) {
        /* Set j to the smallest of the two sons: */
        if (j < heap_len && smaller(tree, heap[j+1], heap[j])) j++;

        /* Exit if v is smaller than both sons */
        htemp = heap[j];
        if (smaller(tree, v, htemp)) break;

        /* Exchange v with the smallest son */
        heap[k] = htemp;
        k = j;

        /* And continue down the tree, setting j to the left son of k */
        j <<= 1;
    }
    heap[k] = v;
}

/* ===========================================================================
 * Compute the optimal bit lengths for a tree and update the total bit length
 * for the current block.
 * IN assertion: the fields freq and dad are set, heap[heap_max] and
 *    above are the tree nodes sorted by increasing frequency.
 * OUT assertions: the field len is set to the optimal bit length, the
 *     array bl_count contains the frequencies for each bit length.
 *     The length opt_len is updated; static_len is also updated if stree is
 *     not null.
 */
local void gen_bitlen(desc)
    tree_desc near *desc; /* the tree descriptor */
{
    ct_data near *tree  = desc->dyn_tree;
    int near *extra     = desc->extra_bits;
    int base            = desc->extra_base;
    int max_code        = desc->max_code;
    int max_length      = desc->max_length;
    ct_data near *stree = desc->static_tree;
    int h;              /* heap index */
    int n, m;           /* iterate over the tree elements */
    int bits;           /* bit length */
    int xbits;          /* extra bits */
    ush f;              /* frequency */
    int overflow = 0;   /* number of elements with bit length too large */

    for (bits = 0; bits <= MAX_BITS; bits++) bl_count[bits] = 0;

    /* In a first pass, compute the optimal bit lengths (which may
     * overflow in the case of the bit length tree).
     */
    tree[heap[heap_max]].Len = 0; /* root of the heap */

    for (h = heap_max+1; h < HEAP_SIZE; h++) {
        n = heap[h];
        bits = tree[tree[n].Dad].Len + 1;
        if (bits > max_length) bits = max_length, overflow++;
        tree[n].Len = (ush)bits;
        /* We overwrite tree[n].Dad which is no longer needed */

        if (n > max_code) continue; /* not a leaf node */

        bl_count[bits]++;
        xbits = 0;
        if (n >= base) xbits = extra[n-base];
        f = tree[n].Freq;
        opt_len += (ulg)f * (bits + xbits);
        if (stree) static_len += (ulg)f * (stree[n].Len + xbits);
    }
    if (overflow == 0) return;

    Trace((stderr,"\nbit length overflow\n"));
    /* This happens for example on obj2 and pic of the Calgary corpus */

    /* Find the first bit length which could increase: */
    do {
        bits = max_length-1;
        while (bl_count[bits] == 0) bits--;
        bl_count[bits]--;           /* move one leaf down the tree */
        bl_count[bits+1] += (ush)2; /* move one overflow item as its brother */
        bl_count[max_length]--;
        /* The brother of the overflow item also moves one step up,
         * but this does not affect bl_count[max_length]
         */
        overflow -= 2;
    } while (overflow > 0);

    /* Now recompute all bit lengths, scanning in increasing frequency.
     * h is still equal to HEAP_SIZE. (It is simpler to reconstruct all
     * lengths instead of fixing only the wrong ones. This idea is taken
     * from 'ar' written by Haruhiko Okumura.)
     */
    for (bits = max_length; bits != 0; bits--) {
        n = bl_count[bits];
        while (n != 0) {
            m = heap[--h];
            if (m > max_code) continue;
            if (tree[m].Len != (ush)bits) {
                Trace((stderr,"code %d bits %d->%d\n", m, tree[m].Len, bits));
                opt_len += ((long)bits-(long)tree[m].Len)*(long)tree[m].Freq;
                tree[m].Len = (ush)bits;
            }
            n--;
        }
    }
}

/* ===========================================================================
 * Generate the codes for a given tree and bit counts (which need not be
 * optimal).
 * IN assertion: the array bl_count contains the bit length statistics for
 * the given tree and the field len is set for all tree elements.
 * OUT assertion: the field code is set for all tree elements of non
 *     zero code length.
 */
local void gen_codes (tree, max_code)
    ct_data near *tree;        /* the tree to decorate */
    int max_code;              /* largest code with non zero frequency */
{
    ush next_code[MAX_BITS+1]; /* next code value for each bit length */
    ush code = 0;              /* running code value */
    int bits;                  /* bit index */
    int n;                     /* code index */

    /* The distribution counts are first used to generate the code values
     * without bit reversal.
     */
    for (bits = 1; bits <= MAX_BITS; bits++) {
        next_code[bits] = code = (ush)((code + bl_count[bits-1]) << 1);
    }
    /* Check that the bit counts in bl_count are consistent. The last code
     * must be all ones.
     */
    Assert(code + bl_count[MAX_BITS]-1 == (1<< ((ush) MAX_BITS)) - 1,
            "inconsistent bit counts");
    Tracev((stderr,"\ngen_codes: max_code %d ", max_code));

    for (n = 0;  n <= max_code; n++) {
        int len = tree[n].Len;
        if (len == 0) continue;
        /* Now reverse the bits */
        tree[n].Code = (ush)bi_reverse(next_code[len]++, len);

        Tracec(tree != static_ltree, (stderr,"\nn %3d %c l %2d c %4x (%x) ",
             n, (isgraph(n) ? n : ' '), len, tree[n].Code, next_code[len]-1));
    }
}

/* ===========================================================================
 * Construct one Huffman tree and assigns the code bit strings and lengths.
 * Update the total bit length for the current block.
 * IN assertion: the field freq is set for all tree elements.
 * OUT assertions: the fields len and code are set to the optimal bit length
 *     and corresponding code. The length opt_len is updated; static_len is
 *     also updated if stree is not null. The field max_code is set.
 */
local void build_tree(desc)
    tree_desc near *desc; /* the tree descriptor */
{
    ct_data near *tree   = desc->dyn_tree;
    ct_data near *stree  = desc->static_tree;
    int elems            = desc->elems;
    int n, m;          /* iterate over heap elements */
    int max_code = -1; /* largest code with non zero frequency */
    int node = elems;  /* next internal node of the tree */

    /* Construct the initial heap, with least frequent element in
     * heap[SMALLEST]. The sons of heap[n] are heap[2*n] and heap[2*n+1].
     * heap[0] is not used.
     */
    heap_len = 0, heap_max = HEAP_SIZE;

    for (n = 0; n < elems; n++) {
        if (tree[n].Freq != 0) {
            heap[++heap_len] = max_code = n;
            depth[n] = 0;
        } else {
            tree[n].Len = 0;
        }
    }

    /* The pkzip format requires that at least one distance code exists,
     * and that at least one bit should be sent even if there is only one
     * possible code. So to avoid special checks later on we force at least
     * two codes of non zero frequency.
     */
    while (heap_len < 2) {
        int new = heap[++heap_len] = (max_code < 2 ? ++max_code : 0);
        tree[new].Freq = 1;
        depth[new] = 0;
        opt_len--; if (stree) static_len -= stree[new].Len;
        /* new is 0 or 1 so it does not have extra bits */
    }
    desc->max_code = max_code;

    /* The elements heap[heap_len/2+1 .. heap_len] are leaves of the tree,
     * establish sub-heaps of increasing lengths:
     */
    for (n = heap_len/2; n >= 1; n--) pqdownheap(tree, n);

    /* Construct the Huffman tree by repeatedly combining the least two
     * frequent nodes.
     */
    do {
        pqremove(tree, n);   /* n = node of least frequency */
        m = heap[SMALLEST];  /* m = node of next least frequency */

        heap[--heap_max] = n; /* keep the nodes sorted by frequency */
        heap[--heap_max] = m;

        /* Create a new node father of n and m */
        tree[node].Freq = (ush)(tree[n].Freq + tree[m].Freq);
        depth[node] = (uch) (Max(depth[n], depth[m]) + 1);
        tree[n].Dad = tree[m].Dad = (ush)node;
#ifdef DUMP_BL_TREE
        if (tree == bl_tree) {
            fprintf(mesg,"\nnode %d(%d), sons %d(%d) %d(%d)",
                    node, tree[node].Freq, n, tree[n].Freq, m, tree[m].Freq);
        }
#endif
        /* and insert the new node in the heap */
        heap[SMALLEST] = node++;
        pqdownheap(tree, SMALLEST);

    } while (heap_len >= 2);

    heap[--heap_max] = heap[SMALLEST];

    /* At this point, the fields freq and dad are set. We can now
     * generate the bit lengths.
     */
    gen_bitlen((tree_desc near *)desc);

    /* The field len is now set, we can generate the bit codes */
    gen_codes ((ct_data near *)tree, max_code);
}

/* ===========================================================================
 * Scan a literal or distance tree to determine the frequencies of the codes
 * in the bit length tree. Updates opt_len to take into account the repeat
 * counts. (The contribution of the bit length codes will be added later
 * during the construction of bl_tree.)
 */
local void scan_tree (tree, max_code)
    ct_data near *tree; /* the tree to be scanned */
    int max_code;       /* and its largest code of non zero frequency */
{
    int n;                     /* iterates over all tree elements */
    int prevlen = -1;          /* last emitted length */
    int curlen;                /* length of current code */
    int nextlen = tree[0].Len; /* length of next code */
    int count = 0;             /* repeat count of the current code */
    int max_count = 7;         /* max repeat count */
    int min_count = 4;         /* min repeat count */

    if (nextlen == 0) max_count = 138, min_count = 3;
    tree[max_code+1].Len = (ush)-1; /* guard */

    for (n = 0; n <= max_code; n++) {
        curlen = nextlen; nextlen = tree[n+1].Len;
        if (++count < max_count && curlen == nextlen) {
            continue;
        } else if (count < min_count) {
            bl_tree[curlen].Freq += (ush)count;
        } else if (curlen != 0) {
            if (curlen != prevlen) bl_tree[curlen].Freq++;
            bl_tree[REP_3_6].Freq++;
        } else if (count <= 10) {
            bl_tree[REPZ_3_10].Freq++;
        } else {
            bl_tree[REPZ_11_138].Freq++;
        }
        count = 0; prevlen = curlen;
        if (nextlen == 0) {
            max_count = 138, min_count = 3;
        } else if (curlen == nextlen) {
            max_count = 6, min_count = 3;
        } else {
            max_count = 7, min_count = 4;
        }
    }
}

/* ===========================================================================
 * Send a literal or distance tree in compressed form, using the codes in
 * bl_tree.
 */
local void send_tree (tree, max_code)
    ct_data near *tree; /* the tree to be scanned */
    int max_code;       /* and its largest code of non zero frequency */
{
    int n;                     /* iterates over all tree elements */
    int prevlen = -1;          /* last emitted length */
    int curlen;                /* length of current code */
    int nextlen = tree[0].Len; /* length of next code */
    int count = 0;             /* repeat count of the current code */
    int max_count = 7;         /* max repeat count */
    int min_count = 4;         /* min repeat count */

    /* tree[max_code+1].Len = -1; */  /* guard already set */
    if (nextlen == 0) max_count = 138, min_count = 3;

    for (n = 0; n <= max_code; n++) {
        curlen = nextlen; nextlen = tree[n+1].Len;
        if (++count < max_count && curlen == nextlen) {
            continue;
        } else if (count < min_count) {
            do { send_code(curlen, bl_tree); } while (--count != 0);

        } else if (curlen != 0) {
            if (curlen != prevlen) {
                send_code(curlen, bl_tree); count--;
            }
            Assert(count >= 3 && count <= 6, " 3_6?");
            send_code(REP_3_6, bl_tree); send_bits(count-3, 2);

        } else if (count <= 10) {
            send_code(REPZ_3_10, bl_tree); send_bits(count-3, 3);

        } else {
            send_code(REPZ_11_138, bl_tree); send_bits(count-11, 7);
        }
        count = 0; prevlen = curlen;
        if (nextlen == 0) {
            max_count = 138, min_count = 3;
        } else if (curlen == nextlen) {
            max_count = 6, min_count = 3;
        } else {
            max_count = 7, min_count = 4;
        }
    }
}

/* ===========================================================================
 * Construct the Huffman tree for the bit lengths and return the index in
 * bl_order of the last bit length code to send.
 */
local int build_bl_tree()
{
    int max_blindex;  /* index of last bit length code of non zero freq */

    /* Determine the bit length frequencies for literal and distance trees */
    scan_tree((ct_data near *)dyn_ltree, l_desc.max_code);
    scan_tree((ct_data near *)dyn_dtree, d_desc.max_code);

    /* Build the bit length tree: */
    build_tree((tree_desc near *)(&bl_desc));
    /* opt_len now includes the length of the tree representations, except
     * the lengths of the bit lengths codes and the 5+5+4 bits for the counts.
     */

    /* Determine the number of bit length codes to send. The pkzip format
     * requires that at least 4 bit length codes be sent. (appnote.txt says
     * 3 but the actual value used is 4.)
     */
    for (max_blindex = BL_CODES-1; max_blindex >= 3; max_blindex--) {
        if (bl_tree[bl_order[max_blindex]].Len != 0) break;
    }
    /* Update opt_len to include the bit length tree and counts */
    opt_len += 3*(max_blindex+1) + 5+5+4;
    Tracev((stderr, "\ndyn trees: dyn %ld, stat %ld", opt_len, static_len));

    return max_blindex;
}

/* ===========================================================================
 * Send the header for a block using dynamic Huffman trees: the counts, the
 * lengths of the bit length codes, the literal tree and the distance tree.
 * IN assertion: lcodes >= 257, dcodes >= 1, blcodes >= 4.
 */
local void send_all_trees(lcodes, dcodes, blcodes)
    int lcodes, dcodes, blcodes; /* number of codes for each tree */
{
    int rank;                    /* index in bl_order */

    Assert(lcodes >= 257 && dcodes >= 1 && blcodes >= 4, "not enough codes");
    Assert(lcodes <= L_CODES && dcodes <= D_CODES && blcodes <= BL_CODES,
            "too many codes");
    Tracev((stderr, "\nbl counts: "));
    send_bits(lcodes-257, 5);
    /* not +255 as stated in appnote.txt 1.93a or -256 in 2.04c */
    send_bits(dcodes-1,   5);
    send_bits(blcodes-4,  4); /* not -3 as stated in appnote.txt */
    for (rank = 0; rank < blcodes; rank++) {
        Tracev((stderr, "\nbl code %2d ", bl_order[rank]));
        send_bits(bl_tree[bl_order[rank]].Len, 3);
    }
    Tracev((stderr, "\nbl tree: sent %s",
     zip_fuzofft(bits_sent, NULL, NULL)));

    send_tree((ct_data near *)dyn_ltree, lcodes-1); /* send the literal tree */
    Tracev((stderr, "\nlit tree: sent %s",
     zip_fuzofft(bits_sent, NULL, NULL)));

    send_tree((ct_data near *)dyn_dtree, dcodes-1); /* send the distance tree */
    Tracev((stderr, "\ndist tree: sent %ld",
     zip_fuzofft(bits_sent, NULL, NULL)));
}

/* ===========================================================================
 * Determine the best encoding for the current block: dynamic trees, static
 * trees or store, and output the encoded block to the zip file. This function
 * returns the total compressed length (in bytes) for the file so far.
 */
/* zip64 support 08/29/2003 R.Nausedat */
uzoff_t flush_block(buf, stored_len, eof)
    char *buf;        /* input block, or NULL if too old */
    ulg stored_len;   /* length of input block */
    int eof;          /* true if this is the last block for a file */
{
    ulg opt_lenb, static_lenb; /* opt_len and static_len in bytes */
    int max_blindex;  /* index of last bit length code of non zero freq */

    flag_buf[last_flags] = flags; /* Save the flags for the last 8 items */

     /* Check if the file is ascii or binary */
    if (*file_type == (ush)UNKNOWN) set_file_type();

    /* Construct the literal and distance trees */
    build_tree((tree_desc near *)(&l_desc));
    Tracev((stderr, "\nlit data: dyn %ld, stat %ld", opt_len, static_len));

    build_tree((tree_desc near *)(&d_desc));
    Tracev((stderr, "\ndist data: dyn %ld, stat %ld", opt_len, static_len));
    /* At this point, opt_len and static_len are the total bit lengths of
     * the compressed block data, excluding the tree representations.
     */

    /* Build the bit length tree for the above two trees, and get the index
     * in bl_order of the last bit length code to send.
     */
    max_blindex = build_bl_tree();

    /* Determine the best encoding. Compute first the block length in bytes */
    opt_lenb = (opt_len+3+7)>>3;
    static_lenb = (static_len+3+7)>>3;
#ifdef DEBUG
    input_len += stored_len; /* for debugging only */
#endif

    Trace((stderr, "\nopt %lu(%lu) stat %lu(%lu) stored %lu lit %u dist %u ",
            opt_lenb, opt_len, static_lenb, static_len, stored_len,
            last_lit, last_dist));

    if (static_lenb <= opt_lenb) opt_lenb = static_lenb;

#ifndef PGP /* PGP can't handle stored blocks */
    /* If compression failed and this is the first and last block,
     * the whole file is transformed into a stored file:
     */
#ifdef FORCE_METHOD
    if (level == 1 && eof && file_method != NULL &&
        cmpr_bytelen == (uzoff_t)0 && cmpr_len_bits == 0L
       ) { /* force stored file */
#else
    if (stored_len <= opt_lenb && eof && file_method != NULL &&
        cmpr_bytelen == (uzoff_t)0 && cmpr_len_bits == 0L &&
        seekable() && !use_descriptors) {
#endif
        /* Since LIT_BUFSIZE <= 2*WSIZE, the input data must be there: */
        if (buf == NULL) error ("block vanished");

        copy_block(buf, (unsigned)stored_len, 0); /* without header */
        cmpr_bytelen = stored_len;
        *file_method = STORE;
    } else
#endif /* PGP */

#ifdef FORCE_METHOD
    if (level <= 2 && buf != (char*)NULL) { /* force stored block */
#else
    if (stored_len+4 <= opt_lenb && buf != (char*)NULL) {
                       /* 4: two words for the lengths */
#endif
        /* The test buf != NULL is only necessary if LIT_BUFSIZE > WSIZE.
         * Otherwise we can't have processed more than WSIZE input bytes since
         * the last block flush, because compression would have been
         * successful. If LIT_BUFSIZE <= WSIZE, it is never too late to
         * transform a block into a stored block.
         */
        send_bits((STORED_BLOCK<<1)+eof, 3);  /* send block type */
        cmpr_bytelen += ((cmpr_len_bits + 3 + 7) >> 3) + stored_len + 4;
        cmpr_len_bits = 0L;

        copy_block(buf, (unsigned)stored_len, 1); /* with header */

#ifdef FORCE_METHOD
    } else if (level == 3) { /* force static trees */
#else
    } else if (static_lenb == opt_lenb) {
#endif
        send_bits((STATIC_TREES<<1)+eof, 3);
        compress_block((ct_data near *)static_ltree, (ct_data near *)static_dtree);
        cmpr_len_bits += 3 + static_len;
        cmpr_bytelen += cmpr_len_bits >> 3;
        cmpr_len_bits &= 7L;
    } else {
        send_bits((DYN_TREES<<1)+eof, 3);
        send_all_trees(l_desc.max_code+1, d_desc.max_code+1, max_blindex+1);
        compress_block((ct_data near *)dyn_ltree, (ct_data near *)dyn_dtree);
        cmpr_len_bits += 3 + opt_len;
        cmpr_bytelen += cmpr_len_bits >> 3;
        cmpr_len_bits &= 7L;
    }
    Assert(((cmpr_bytelen << 3) + cmpr_len_bits) == bits_sent,
            "bad compressed size");
    init_block();

    if (eof) {
#if defined(PGP) && !defined(MMAP)
        /* Wipe out sensitive data for pgp */
# ifdef DYN_ALLOC
        extern uch *window;
# else
        extern uch window[];
# endif
        memset(window, 0, (unsigned)(2*WSIZE-1)); /* -1 needed if WSIZE=32K */
#else /* !PGP */
        Assert(input_len == isize, "bad input size");
#endif
        bi_windup();
        cmpr_len_bits += 7;  /* align on byte boundary */
    }
    Tracev((stderr,"\ncomprlen %s(%s) ",
     zip_fuzofft( cmpr_bytelen + (cmpr_len_bits>>3), NULL, NULL),
     zip_fuzofft( (cmpr_bytelen << 3) + cmpr_len_bits - 7*eof, NULL, NULL)));
    Trace((stderr, "\n"));

    return cmpr_bytelen + (cmpr_len_bits >> 3);
}

/* ===========================================================================
 * Save the match info and tally the frequency counts. Return true if
 * the current block must be flushed.
 */
int ct_tally (dist, lc)
    int dist;  /* distance of matched string */
    int lc;    /* match length-MIN_MATCH or unmatched char (if dist==0) */
{
    l_buf[last_lit++] = (uch)lc;
    if (dist == 0) {
        /* lc is the unmatched char */
        dyn_ltree[lc].Freq++;
    } else {
        /* Here, lc is the match length - MIN_MATCH */
        dist--;             /* dist = match distance - 1 */
        Assert((ush)dist < (ush)MAX_DIST &&
               (ush)lc <= (ush)(MAX_MATCH-MIN_MATCH) &&
               (ush)d_code(dist) < (ush)D_CODES,  "ct_tally: bad match");

        dyn_ltree[length_code[lc]+LITERALS+1].Freq++;
        dyn_dtree[d_code(dist)].Freq++;

        d_buf[last_dist++] = (ush)dist;
        flags |= flag_bit;
    }
    flag_bit <<= 1;

    /* Output the flags if they fill a byte: */
    if ((last_lit & 7) == 0) {
        flag_buf[last_flags++] = flags;
        flags = 0, flag_bit = 1;
    }
    /* Try to guess if it is profitable to stop the current block here */
    if (level > 2 && (last_lit & 0xfff) == 0) {
        /* Compute an upper bound for the compressed length */
        ulg out_length = (ulg)last_lit*8L;
        ulg in_length = (ulg)strstart-block_start;
        int dcode;
        for (dcode = 0; dcode < D_CODES; dcode++) {
            out_length += (ulg)dyn_dtree[dcode].Freq*(5L+extra_dbits[dcode]);
        }
        out_length >>= 3;
        Trace((stderr,"\nlast_lit %u, last_dist %u, in %ld, out ~%ld(%ld%%) ",
               last_lit, last_dist, in_length, out_length,
               100L - out_length*100L/in_length));
        if (last_dist < last_lit/2 && out_length < in_length/2) return 1;
    }
    return (last_lit == LIT_BUFSIZE-1 || last_dist == DIST_BUFSIZE);
    /* We avoid equality with LIT_BUFSIZE because of wraparound at 64K
     * on 16 bit machines and because stored blocks are restricted to
     * 64K-1 bytes.
     */
}

/* ===========================================================================
 * Send the block data compressed using the given Huffman trees
 */
local void compress_block(ltree, dtree)
    ct_data near *ltree; /* literal tree */
    ct_data near *dtree; /* distance tree */
{
    unsigned dist;      /* distance of matched string */
    int lc;             /* match length or unmatched char (if dist == 0) */
    unsigned lx = 0;    /* running index in l_buf */
    unsigned dx = 0;    /* running index in d_buf */
    unsigned fx = 0;    /* running index in flag_buf */
    uch flag = 0;       /* current flags */
    unsigned code;      /* the code to send */
    int extra;          /* number of extra bits to send */

    if (last_lit != 0) do {
        if ((lx & 7) == 0) flag = flag_buf[fx++];
        lc = l_buf[lx++];
        if ((flag & 1) == 0) {
            send_code(lc, ltree); /* send a literal byte */
            Tracecv(isgraph(lc), (stderr," '%c' ", lc));
        } else {
            /* Here, lc is the match length - MIN_MATCH */
            code = length_code[lc];
            send_code(code+LITERALS+1, ltree); /* send the length code */
            extra = extra_lbits[code];
            if (extra != 0) {
                lc -= base_length[code];
                send_bits(lc, extra);        /* send the extra length bits */
            }
            dist = d_buf[dx++];
            /* Here, dist is the match distance - 1 */
            code = d_code(dist);
            Assert(code < D_CODES, "bad d_code");

            send_code(code, dtree);       /* send the distance code */
            extra = extra_dbits[code];
            if (extra != 0) {
                dist -= base_dist[code];
                send_bits(dist, extra);   /* send the extra distance bits */
            }
        } /* literal or match pair ? */
        flag >>= 1;
    } while (lx < last_lit);

    send_code(END_BLOCK, ltree);
}

/* ===========================================================================
 * Set the file type to TEXT (ASCII) or BINARY, using following algorithm:
 * - TEXT, either ASCII or an ASCII-compatible extension such as ISO-8859,
 *   UTF-8, etc., when the following two conditions are satisfied:
 *    a) There are no non-portable control characters belonging to the
 *       "black list" (0..6, 14..25, 28..31).
 *    b) There is at least one printable character belonging to the
 *       "white list" (9 {TAB}, 10 {LF}, 13 {CR}, 32..255).
 * - BINARY otherwise.
 *
 * Note that the following partially-portable control characters form a
 * "gray list" that is ignored in this detection algorithm:
 * (7 {BEL}, 8 {BS}, 11 {VT}, 12 {FF}, 26 {SUB}, 27 {ESC}).
 *
 * Also note that, unlike in the previous 20% binary detection algorithm,
 * any control characters in the black list will set the file type to
 * BINARY.  If a text file contains a single accidental black character,
 * the file will be flagged as BINARY in the archive.
 *
 * IN assertion: the fields freq of dyn_ltree are set.
 */
local void set_file_type()
{
    /* bit-mask of black-listed bytes
     * bit is set if byte is black-listed
     * set bits 0..6, 14..25, and 28..31
     * 0xf3ffc07f = binary 11110011111111111100000001111111
     */
    unsigned long mask = 0xf3ffc07fL;
    int n;

    /* Check for non-textual ("black-listed") bytes. */
    for (n = 0; n <= 31; n++, mask >>= 1)
        if ((mask & 1) && (dyn_ltree[n].Freq != 0))
        {
            *file_type = BINARY;
            return;
        }

    /* Check for textual ("white-listed") bytes. */
    *file_type = ASCII;
    if (dyn_ltree[9].Freq != 0 || dyn_ltree[10].Freq != 0
            || dyn_ltree[13].Freq != 0)
        return;
    for (n = 32; n < LITERALS; n++)
        if (dyn_ltree[n].Freq != 0)
            return;

    /* This deflate stream is either empty, or
     * it has tolerated ("gray-listed") bytes only.
     */
    *file_type = BINARY;
}


/* ===========================================================================
 * Initialize the bit string routines.
 */
void bi_init (tgt_buf, tgt_size, flsh_allowed)
    char *tgt_buf;
    unsigned tgt_size;
    int flsh_allowed;
{
    out_buf = tgt_buf;
    out_size = tgt_size;
    out_offset = 0;
    flush_flg = flsh_allowed;

    bi_buf = 0;
    bi_valid = 0;
#ifdef DEBUG
    bits_sent = (uzoff_t)0;
#endif
}

#if (!defined(ASMV) || !defined(RISCOS))
/* ===========================================================================
 * Send a value on a given number of bits.
 * IN assertion: length <= 16 and value fits in length bits.
 */
local void send_bits(value, length)
    int value;  /* value to send */
    int length; /* number of bits */
{
#ifdef DEBUG
    Tracevv((stderr," l %2d v %4x ", length, value));
    Assert(length > 0 && length <= 15, "invalid length");
    bits_sent += (uzoff_t)length;
#endif
    /* If not enough room in bi_buf, use (bi_valid) bits from bi_buf and
     * (Buf_size - bi_valid) bits from value to flush the filled bi_buf,
     * then fill in the rest of (value), leaving (length - (Buf_size-bi_valid))
     * unused bits in bi_buf.
     */
    bi_buf |= (value << bi_valid);
    bi_valid += length;
    if (bi_valid > (int)Buf_size) {
        PUTSHORT(bi_buf);
        bi_valid -= Buf_size;
        bi_buf = (unsigned)value >> (length - bi_valid);
    }
}

/* ===========================================================================
 * Reverse the first len bits of a code, using straightforward code (a faster
 * method would use a table)
 * IN assertion: 1 <= len <= 15
 */
local unsigned bi_reverse(code, len)
    unsigned code; /* the value to invert */
    int len;       /* its bit length */
{
    register unsigned res = 0;
    do {
        res |= code & 1;
        code >>= 1, res <<= 1;
    } while (--len > 0);
    return res >> 1;
}
#endif /* !ASMV || !RISCOS */

/* ===========================================================================
 * Write out any remaining bits in an incomplete byte.
 */
local void bi_windup()
{
    if (bi_valid > 8) {
        PUTSHORT(bi_buf);
    } else if (bi_valid > 0) {
        PUTBYTE(bi_buf);
    }
    if (flush_flg) {
        flush_outbuf(out_buf, &out_offset);
    }
    bi_buf = 0;
    bi_valid = 0;
#ifdef DEBUG
    bits_sent = (bits_sent+7) & ~7;
#endif
}

/* ===========================================================================
 * Copy a stored block to the zip file, storing first the length and its
 * one's complement if requested.
 *
 * Buffer Overwrite fix
 *
 * A buffer flush has been added to fix a bug when encrypting deflated files
 * with embedded "copied blocks".  When encrypting, the flush_out() routine
 * modifies its data buffer because encryption is done "in-place" in
 * zfwrite(), whereas without encryption, the flush_out() data buffer is
 * left unaltered.  This can be a problem as noted below by the submitter.
 *
 * "But an exception comes when a block of stored data (data that could not
 * be compressed) is being encrypted. In this case, the data that is passed
 * to zfwrite (and is therefore encrypted-in-place) is actually a block of
 * data from within the sliding input window that is being managed by
 * deflate.c.
 *
 * "Since part of the sliding input window has now been overwritten by
 * encrypted (and essentially random) data, deflate.c's search for previous
 * text that matches the current text will usually fail but on rare
 * occasions will find a match with something in the encrypted data. This
 * incorrect match then causes incorrect information to be placed in the
 * ZIP file."
 *
 * The problem results in the zip file having bad data and so a bad CRC.
 * This does not happen often and to recreate the problem a large file
 * with non-compressable data is needed so that deflate chooses to store the
 * data.  A test file of 400 MB seems large enough to recreate the problem
 * using a command such as
 *     zip -1 -e crcerror.zip testfile.dat
 * maybe half the time.
 *
 * This problem has been fixed by copying the data into the deflate output
 * buffer before calling flush_outbuf(), when encryption is enabled.
 *
 * Thanks to the nice people at WinZip for identifying the problem and
 * passing it on.  Also see Changes.
 *
 * 2006-03-06 EG, CS
 */
local void copy_block(block, len, header)
    char *block;  /* the input data */
    unsigned len; /* its length */
    int header;   /* true if block header must be written */
{
    bi_windup();              /* align on byte boundary */

    if (header) {
        PUTSHORT((ush)len);
        PUTSHORT((ush)~len);
#ifdef DEBUG
        bits_sent += 2*16;
#endif
    }
    if (flush_flg) {
        flush_outbuf(out_buf, &out_offset);
        if (key != (char *)NULL) {  /* key is the global password pointer */
            /* Encryption modifies the data in the output buffer. But the
             * copied input data must remain intact for further deflate
             * string matching lookups.  Therefore, the input data is
             * copied into the compression output buffer for flushing
             * to the compressed/encrypted output stream.
             */
            while(len > 0) {
                out_offset = (len < out_size ? len : out_size);
                memcpy(out_buf, block, out_offset);
                block += out_offset;
                len -= out_offset;
                flush_outbuf(out_buf, &out_offset);
            }
        } else {
            /* Without encryption, the output routines do not touch the
             * written data, so there is no need for an additional copy
             * operation.
             */
            out_offset = len;
            flush_outbuf(block, &out_offset);
        }
    } else if (out_offset + len > out_size) {
        error("output buffer too small for in-memory compression");
    } else {
        memcpy(out_buf + out_offset, block, len);
        out_offset += len;
    }
#ifdef DEBUG
    bits_sent += (ulg)len<<3;
#endif
}

#endif /* !USE_ZLIB */
