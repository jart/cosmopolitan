#ifndef DEFLATE_H
#define DEFLATE_H
#include "third_party/zlib/zutil.h"

/* define NO_GZIP when compiling if you want to disable gzip header and
   trailer creation by deflate(). NO_GZIP would be used to avoid linking
   in the crc code when it is not needed. For shared libraries, gzip
   encoding should be left enabled. */
#ifndef NO_GZIP
#define GZIP
#endif

/* number of length codes, not counting the special END_BLOCK code */
#define LENGTH_CODES 29

/* number of literal bytes 0..255 */
#define LITERALS 256

/* number of Literal or Length codes, including the END_BLOCK code */
#define L_CODES (LITERALS + 1 + LENGTH_CODES)

/* number of distance codes */
#define D_CODES 30

/* number of codes used to transfer the bit lengths */
#define BL_CODES 19

/* maximum heap size */
#define HEAP_SIZE (2 * L_CODES + 1)

/* All codes must not exceed MAX_BITS bits */
#define MAX_BITS 15

/* size of bit buffer in bi_buf */
#define Buf_size 16

#define DIST_CODE_LEN 512 /* see definition of array dist_code below */

#define INIT_STATE 42 /* zlib header -> BUSY_STATE */
#ifdef GZIP
#define GZIP_STATE 57 /* gzip header -> BUSY_STATE | EXTRA_STATE */
#endif
#define EXTRA_STATE 69   /* gzip extra block -> NAME_STATE */
#define NAME_STATE 73    /* gzip file name -> COMMENT_STATE */
#define COMMENT_STATE 91 /* gzip comment -> HCRC_STATE */
#define HCRC_STATE 103   /* gzip header CRC -> BUSY_STATE */
#define BUSY_STATE 113   /* deflate -> FINISH_STATE */
#define FINISH_STATE 666 /* stream complete */

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/* Data structure describing a single value and its code string. */
typedef struct ct_data_s {
  union {
    uint16_t freq; /* frequency count */
    uint16_t code; /* bit string */
  } fc;
  union {
    uint16_t dad; /* father node in Huffman tree */
    uint16_t len; /* length of bit string */
  } dl;
} ct_data;

#define Freq fc.freq
#define Code fc.code
#define Dad dl.dad
#define Len dl.len

typedef struct static_tree_desc_s static_tree_desc;

typedef struct tree_desc_s {
  ct_data *dyn_tree;                 /* the dynamic tree */
  int max_code;                      /* largest code with non zero frequency */
  const static_tree_desc *stat_desc; /* the corresponding static tree */
} tree_desc;

typedef uint16_t Pos;
typedef Pos Posf;
typedef unsigned IPos;

/* A Pos is an index in the character window. We use short instead of int to
 * save space in the various tables. IPos is used only for parameter passing.
 */

struct DeflateState {
  z_streamp strm;            /* pointer back to this zlib stream */
  int status;                /* as the name implies */
  Bytef *pending_buf;        /* output still pending */
  uint64_t pending_buf_size; /* size of pending_buf */
  Bytef *pending_out;        /* next pending byte to output to the stream */
  uint64_t pending;          /* nb of bytes in the pending buffer */
  int wrap;                  /* bit 0 true for zlib, bit 1 true for gzip */
  gz_headerp gzhead;         /* gzip header information to write */
  uint64_t gzindex;          /* where in extra, name, or comment */
  Byte method;               /* can only be DEFLATED */
  int last_flush; /* value of flush param for previous deflate call */
  unsigned crc0[4 * 5];
  /* used by deflate.c: */

  uInt w_size; /* LZ77 window size (32K by default) */
  uInt w_bits; /* log2(w_size)  (8..16) */
  uInt w_mask; /* w_size - 1 */

  /* Sliding window. Input bytes are read into the second half of the
     window, and move to the first half later to keep a dictionary of at
     least wSize bytes. With this organization, matches are limited to a
     distance of wSize-MAX_MATCH bytes, but this ensures that IO is
     always performed with a length multiple of the block size. Also, it
     limits the window size to 64K, which is quite useful on MSDOS. To
     do: use the user input buffer as sliding window. */
  Bytef *window;

  /* Actual size of window: 2*wSize, except when the user input buffer
    is directly used as sliding window. */
  uint64_t window_size;

  /* Link to older string with same hash index. To limit the size of
     this array to 64K, this link is maintained only for the last 32K
     strings. Index in this array is thus a window index modulo 32K. */
  Posf *prev;

  Posf *head; /* Heads of the hash chains or NIL. */

  uInt ins_h;     /* hash index of string to be inserted */
  uInt hash_size; /* number of elements in hash table */
  uInt hash_bits; /* log2(hash_size) */
  uInt hash_mask; /* hash_size-1 */

  /* Number of bits by which ins_h must be shifted at each input step.
     It must be such that after MIN_MATCH steps, the oldest byte no
     longer takes part in the hash key, that is: hash_shift * MIN_MATCH
     >= hash_bits */
  uInt hash_shift;

  /* Window position at the beginning of the current output block. Gets
     negative when the window is moved backwards. */
  long block_start;

  uInt match_length;   /* length of best match */
  IPos prev_match;     /* previous match */
  int match_available; /* set if previous match exists */
  uInt strstart;       /* start of string to insert */
  uInt match_start;    /* start of matching string */
  uInt lookahead;      /* number of valid bytes ahead in window */

  /* Length of the best match at previous step. Matches not greater than
     this are discarded. This is used in the lazy match evaluation. */
  uInt prev_length;

  /* To speed up deflation, hash chains are never searched beyond this
     length. A higher limit improves compression ratio but degrades the
     speed. */
  uInt max_chain_length;

  /* Attempt to find a better match only when the current match is
     strictly smaller than this value. This mechanism is used only for
     compression levels >= 4. */
  uInt max_lazy_match;

  /* Insert new strings in the hash table only if the match length is not
     greater than this length. This saves time but degrades compression.
     max_insert_length is used only for compression levels <= 3. */
#define max_insert_length max_lazy_match

  int level;    /* compression level (1..9) */
  int strategy; /* favor or force Huffman coding*/

  /* Use a faster search when the previous match is longer than this */
  uInt good_match;

  int nice_match; /* Stop searching when current match exceeds this */

  /* used by trees.c: */
  /* Didn't use ct_data typedef below to suppress compiler warning */
  struct ct_data_s dyn_ltree[HEAP_SIZE];       /* literal and length tree */
  struct ct_data_s dyn_dtree[2 * D_CODES + 1]; /* distance tree */
  struct ct_data_s bl_tree[2 * BL_CODES + 1]; /* Huffman tree for bit lengths */

  struct tree_desc_s l_desc;  /* desc. for literal tree */
  struct tree_desc_s d_desc;  /* desc. for distance tree */
  struct tree_desc_s bl_desc; /* desc. for bit length tree */

  /* number of codes at each bit length for an optimal tree */
  uint16_t bl_count[MAX_BITS + 1];

  /* The sons of heap[n] are heap[2*n] and heap[2*n+1]. heap[0] is not
     used. The same heap array is used to build all trees. */
  int heap[2 * L_CODES + 1]; /* heap used to build the Huffman trees */
  int heap_len;              /* number of elements in the heap */
  int heap_max;              /* element of largest frequency */

  /* Depth of each subtree used as tie breaker for trees of equal
     frequency. */
  uint8_t depth[2 * L_CODES + 1];

  uint8_t *sym_buf; /* buffer for distances and literals/lengths */

  /* Size of match buffer for literals/lengths.  There are 4 reasons for
     limiting lit_bufsize to 64K:
       - frequencies can be kept in 16 bit counters
       - if compression is not successful for the first block, all input
         data is still in the window so we can still emit a stored block even
         when input comes from standard input.  (This can also be done for
         all blocks if lit_bufsize is not greater than 32K.)
       - if compression is not successful for a file smaller than 64K, we can
         even emit a stored file instead of a stored block (saving 5 bytes).
         This is applicable only for zip (not gzip or zlib).
       - creating new Huffman trees less frequently may not provide fast
         adaptation to changes in the input data statistics. (Take for
         example a binary file with poorly compressible code followed by
         a highly compressible string table.) Smaller buffer sizes give
         fast adaptation but have of course the overhead of transmitting
         trees more frequently.
       - I can't count above 4 */
  uInt lit_bufsize;

  uInt sym_next; /* running index in sym_buf */
  uInt sym_end;  /* symbol table full when sym_next reaches this */

  uint64_t opt_len;    /* bit length of current block with optimal trees */
  uint64_t static_len; /* bit length of current block with static trees */
  uInt matches;        /* number of string matches in current block */
  uInt insert;         /* bytes at end of window left to insert */

#ifdef ZLIB_DEBUG
  uint64_t compressed_len; /* total bit length of compressed file mod 2^32 */
  uint64_t bits_sent;      /* bit length of compressed data sent mod 2^32 */
#endif

  /* Output buffer. bits are inserted starting at the bottom (least
     significant bits). */
  uint16_t bi_buf;

  /* Number of valid bits in bi_buf. All bits above the last valid bit
     are always zero. */
  int bi_valid;

  /* High water mark offset in window for initialized bytes -- bytes
     above this are set to zero in order to avoid memory check warnings
     when longest match routines access bytes past the input. This is
     then updated to the new high water mark. */
  uint64_t high_water;
};

/* Output a byte on the stream.
 * IN assertion: there is enough room in pending_buf.
 */
#define put_byte(s, c) \
  { s->pending_buf[s->pending++] = (Bytef)(c); }

#define MIN_LOOKAHEAD (MAX_MATCH + MIN_MATCH + 1)
/* Minimum amount of lookahead, except at the end of the input file.
 * See deflate.c for comments about the MIN_MATCH+1.
 */

#define MAX_DIST(s) ((s)->w_size - MIN_LOOKAHEAD)
/* In order to simplify the code, particularly on 16 bit machines, match
 * distances are limited to MAX_DIST instead of WSIZE.
 */

#define WIN_INIT MAX_MATCH
/* Number of bytes after end of data in window to initialize in order to avoid
   memory checker errors from longest match routines */

/* in trees.c */
void _tr_init(struct DeflateState *s) hidden;
int _tr_tally(struct DeflateState *s, unsigned dist, unsigned lc) hidden;
void _tr_flush_block(struct DeflateState *s, charf *buf, uint64_t stored_len,
                     int last) hidden;
void _tr_flush_bits(struct DeflateState *s) hidden;
void _tr_align(struct DeflateState *s) hidden;
void _tr_stored_block(struct DeflateState *s, charf *buf, uint64_t stored_len,
                      int last) hidden;

/* Mapping from a distance to a distance code. dist is the distance - 1
 * and must not have side effects. kZlibDistCode[256] and
 * kZlibDistCode[257] are never used.
 */
#define d_code(dist) \
  ((dist) < 256 ? kZlibDistCode[dist] : kZlibDistCode[256 + ((dist) >> 7)])

#ifndef ZLIB_DEBUG
/* Inline versions of _tr_tally for speed: */

extern const ct_data kZlibStaticDtree[D_CODES] hidden;
extern const ct_data kZlibStaticLtree[L_CODES + 2] hidden;
extern const int kZlibBaseDist[D_CODES] hidden;
extern const int kZlibBaseLength[LENGTH_CODES] hidden;
extern const uint8_t kZlibDistCode[DIST_CODE_LEN] hidden;
extern const uint8_t kZlibLengthCode[MAX_MATCH - MIN_MATCH + 1] hidden;

#define _tr_tally_lit(s, c, flush)       \
  {                                      \
    uint8_t cc = (c);                    \
    s->sym_buf[s->sym_next++] = 0;       \
    s->sym_buf[s->sym_next++] = 0;       \
    s->sym_buf[s->sym_next++] = cc;      \
    s->dyn_ltree[cc].Freq++;             \
    flush = (s->sym_next == s->sym_end); \
  }
#define _tr_tally_dist(s, distance, length, flush)            \
  {                                                           \
    uint8_t len = (uint8_t)(length);                          \
    uint16_t dist = (uint16_t)(distance);                     \
    s->sym_buf[s->sym_next++] = dist;                         \
    s->sym_buf[s->sym_next++] = dist >> 8;                    \
    s->sym_buf[s->sym_next++] = len;                          \
    dist--;                                                   \
    s->dyn_ltree[kZlibLengthCode[len] + LITERALS + 1].Freq++; \
    s->dyn_dtree[d_code(dist)].Freq++;                        \
    flush = (s->sym_next == s->sym_end);                      \
  }
#else
#define _tr_tally_lit(s, c, flush) flush = _tr_tally(s, 0, c)
#define _tr_tally_dist(s, distance, length, flush) \
  flush = _tr_tally(s, distance, length)
#endif

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* DEFLATE_H */
