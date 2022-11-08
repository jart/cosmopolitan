#ifndef COSMOPOLITAN_THIRD_PARTY_ZLIB_INFFAST_INTERNAL_H_
#define COSMOPOLITAN_THIRD_PARTY_ZLIB_INFFAST_INTERNAL_H_
#include "third_party/zlib/zlib.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_
/* clang-format off */

/* INFLATE_FAST_MIN_INPUT: the minimum number of input bytes needed so that
   we can safely call inflate_fast() with only one up-front bounds check. One
   length/distance code pair (15 bits for the length code, 5 bits for length
   extra, 15 bits for the distance code, 13 bits for distance extra) requires
   reading up to 48 input bits (6 bytes).
*/
#define INFLATE_FAST_MIN_INPUT 6

/* INFLATE_FAST_MIN_OUTPUT: the minimum number of output bytes needed so that
   we can safely call inflate_fast() with only one up-front bounds check. One
   length/distance code pair can output up to 258 bytes, which is the maximum
   length that can be coded.
 */
#define INFLATE_FAST_MIN_OUTPUT 258

void inflate_fast(z_streamp strm, unsigned start) _Hide;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_ZLIB_INFFAST_INTERNAL_H_ */
