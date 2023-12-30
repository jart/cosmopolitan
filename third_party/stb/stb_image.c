/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "third_party/stb/stb_image.h"
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/fmt/conv.h"
#include "libc/serialize.h"
#include "libc/intrin/bswap.h"
#include "libc/limits.h"
#include "libc/log/gdb.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/math.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/x/x.h"
#include "third_party/aarch64/arm_neon.internal.h"
#include "third_party/intel/ammintrin.internal.h"

asm(".ident\t\"\\n\\n\
stb_image (Public Domain)\\n\
Credit: Sean Barrett, et al.\\n\
http://nothings.org/stb\"");

#ifdef __x86_64__
#define STBI_SSE2
#define idct_block_kernel stbi__idct_simd
#elif defined(__aarch64__)
#define STBI_NEON
#define idct_block_kernel stbi__idct_simd
#else
#define idct_block_kernel stbi__idct_block
#endif

#define ROL(w, k) (((w) << (k)) | ((w) >> (-(k) & (sizeof(w) * CHAR_BIT - 1))))

#ifndef STBI_MAX_DIMENSIONS
#define STBI_MAX_DIMENSIONS (1 << 24)
#endif

// stbi__context structure is our basic context used by all images, so it
// contains all the IO context, plus some basic image information
typedef struct {
  uint32_t img_x, img_y;
  int img_n, img_out_n;
  stbi_io_callbacks io;
  void *io_user_data;
  int read_from_callbacks;
  int buflen;
  unsigned char buffer_start[128];
  int callback_already_read;
  unsigned char *img_buffer, *img_buffer_end;
  unsigned char *img_buffer_original, *img_buffer_original_end;
} stbi__context;

static const unsigned char kPngSig[8] = {137, 80, 78, 71, 13, 10, 26, 10};

static void stbi__refill_buffer(stbi__context *s);

// initialize a memory-decode context
static void stbi__start_mem(stbi__context *s, unsigned char const *buffer,
                            int len) {
  s->io.read = NULL;
  s->read_from_callbacks = 0;
  s->callback_already_read = 0;
  s->img_buffer = s->img_buffer_original = (unsigned char *)buffer;
  s->img_buffer_end = s->img_buffer_original_end =
      (unsigned char *)buffer + len;
}

// initialize a callback-based context
static void stbi__start_callbacks(stbi__context *s, stbi_io_callbacks *c,
                                  void *user) {
  s->io = *c;
  s->io_user_data = user;
  s->buflen = sizeof(s->buffer_start);
  s->read_from_callbacks = 1;
  s->callback_already_read = 0;
  s->img_buffer = s->img_buffer_original = s->buffer_start;
  stbi__refill_buffer(s);
  s->img_buffer_original_end = s->img_buffer_end;
}

static int stbi__stdio_read(void *user, char *data, int size) {
  return fread(data, 1, size, user);
}

static void stbi__stdio_skip(void *user, int n) {
  int ch;
  fseek(user, n, SEEK_CUR);
  ch = fgetc(user);
  if (ch != EOF) {
    ungetc(ch, user);
  }
}

static int stbi__stdio_eof(void *user) {
  return feof(user) || ferror(user);
}

static stbi_io_callbacks stbi__stdio_callbacks = {
    stbi__stdio_read,
    stbi__stdio_skip,
    stbi__stdio_eof,
};

static void stbi__start_file(stbi__context *s, FILE *f) {
  stbi__start_callbacks(s, &stbi__stdio_callbacks, (void *)f);
}

static void stbi__rewind(stbi__context *s) {
  // conceptually rewind SHOULD rewind to the beginning of the stream,
  // but we just rewind to the beginning of the initial buffer, because
  // we only use it after doing 'test', which only ever looks at at most 92
  // bytes
  s->img_buffer = s->img_buffer_original;
  s->img_buffer_end = s->img_buffer_original_end;
}

enum { STBI_ORDER_RGB, STBI_ORDER_BGR };

typedef struct {
  int bits_per_channel;
  int num_channels;
} stbi__result_info;

static int stbi__jpeg_test(stbi__context *);
static void *stbi__jpeg_load(stbi__context *, int *, int *, int *, int,
                             stbi__result_info *);
static int stbi__jpeg_info(stbi__context *, int *, int *, int *);
static int stbi__png_test(stbi__context *);
static void *stbi__png_load(stbi__context *, int *, int *, int *, int,
                            stbi__result_info *);
static int stbi__png_info(stbi__context *, int *, int *, int *);
static int stbi__png_is16(stbi__context *);
static int stbi__gif_test(stbi__context *);
static void *stbi__gif_load(stbi__context *, int *, int *, int *, int,
                            stbi__result_info *);
static void *stbi__load_gif_main(stbi__context *, int **, int *, int *, int *,
                                 int *, int);
static int stbi__gif_info(stbi__context *, int *, int *, int *);
static int stbi__pnm_test(stbi__context *);
static void *stbi__pnm_load(stbi__context *, int *, int *, int *, int,
                            stbi__result_info *);
static int stbi__pnm_info(stbi__context *, int *, int *, int *);

static const char *stbi__g_failure_reason;

static int stbi__vertically_flip_on_load = 0;

const char *stbi_failure_reason(void) {
  return stbi__g_failure_reason;
}

static int stbi__err(const char *specific_details,
                     const char *general_details) {
  // DebugBreak();
  // WARNF("%s: %s", general_details, specific_details);
  stbi__g_failure_reason = general_details;
  return 0;
}

// stb_image uses ints pervasively, including for offset calculations.
// therefore the largest decoded image size we can support with the
// current code, even on 64-bit targets, is INT_MAX. this is not a
// significant limitation for the intended use case.
//
// we do, however, need to make sure our size calculations don't
// overflow. hence a few helper functions for size calculations that
// multiply integers together, making sure that they're non-negative
// and no overflow occurs.

// return 1 if the sum is valid, 0 on overflow.
// negative terms are considered invalid.
static int stbi__addsizes_valid(int a, int b) {
  if (b < 0) return 0;
  // now 0 <= b <= INT_MAX, hence also
  // 0 <= INT_MAX - b <= INTMAX.
  // And "a + b <= INT_MAX" (which might overflow) is the
  // same as a <= INT_MAX - b (no overflow)
  return a <= INT_MAX - b;
}

// returns 1 if the product is valid, 0 on overflow.
// negative factors are considered invalid.
static int stbi__mul2sizes_valid(int a, int b) {
  if (a < 0 || b < 0) return 0;
  if (b == 0) return 1;  // mul-by-0 is always safe
  // portable way to check for no overflows in a*b
  return a <= INT_MAX / b;
}

// returns 1 if "a * b + add" has no negative terms/factors
// and doesn't overflow
static int stbi__mad2sizes_valid(int a, int b, int add) {
  return stbi__mul2sizes_valid(a, b) && stbi__addsizes_valid(a * b, add);
}

// returns 1 if "a * b * c + add" has no negative terms/factors
// and doesn't overflow
static int stbi__mad3sizes_valid(int a, int b, int c, int add) {
  return stbi__mul2sizes_valid(a, b) && stbi__mul2sizes_valid(a * b, c) &&
         stbi__addsizes_valid(a * b * c, add);
}

// returns 1 if "a * b * c * d + add" has no negative terms/factors
// and doesn't overflow
static int stbi__mad4sizes_valid(int a, int b, int c, int d, int add) {
  return stbi__mul2sizes_valid(a, b) && stbi__mul2sizes_valid(a * b, c) &&
         stbi__mul2sizes_valid(a * b * c, d) &&
         stbi__addsizes_valid(a * b * c * d, add);
}

// mallocs with size overflow checking
static void *stbi__malloc_mad2(int a, int b, int add) {
  if (!stbi__mad2sizes_valid(a, b, add)) return NULL;
  return xmalloc(a * b + add);
}

static void *stbi__malloc_mad3(int a, int b, int c, int add) {
  if (!stbi__mad3sizes_valid(a, b, c, add)) return NULL;
  return xmalloc(a * b * c + add);
}

static void *stbi__malloc_mad4(int a, int b, int c, int d, int add) {
  if (!stbi__mad4sizes_valid(a, b, c, d, add)) return NULL;
  return xmalloc(a * b * c * d + add);
}

// returns 1 if the sum of two signed ints is valid
// (between -2^31 and 2^31-1 inclusive), 0 on overflow.
static int stbi__addints_valid(int a, int b) {
  if ((a >= 0) != (b >= 0)) {
    // a and b have different signs, so no overflow
    return 1;
  }
  if (a < 0 && b < 0) {
    // same as a + b >= INT_MIN; INT_MIN - b cannot overflow since b < 0.
    return a >= INT_MIN - b;
  }
  return a <= INT_MAX - b;
}

// returns 1 if the product of two ints fits in a signed short,
// 0 on overflow.
static int stbi__mul2shorts_valid(int a, int b) {
  if (b == 0 || b == -1) {
    // multiplication by 0 is always 0;
    // check for -1 so SHRT_MIN / b doesn't overflow
    return 1;
  }
  if ((a >= 0) == (b >= 0)) {
    // product is positive, so similar to mul2sizes_valid
    return a <= SHRT_MAX / b;
  }
  if (b < 0) {
    // same as a * b >= SHRT_MIN
    return a <= SHRT_MIN / b;
  }
  return a >= SHRT_MIN / b;
}

#define stbi__errpf(x, y) \
  ({                      \
    stbi__err(x, y);      \
    NULL;                 \
  })

#define stbi__errpuc(x, y) \
  ({                       \
    stbi__err(x, y);       \
    NULL;                  \
  })

void stbi_image_free(void *retval_from_stbi_load) {
  free(retval_from_stbi_load);
}

void stbi_set_flip_vertically_on_load(int flag_true_if_should_flip) {
  stbi__vertically_flip_on_load = flag_true_if_should_flip;
}

static void *stbi__load_main(stbi__context *s, int *x, int *y, int *comp,
                             int req_comp, stbi__result_info *ri) {
  bzero(ri, sizeof(*ri));
  ri->bits_per_channel = 8;
  ri->num_channels = 0;

  // test the formats with a very explicit header first (at least a FOURCC
  // or distinctive magic number first)
  if (stbi__png_test(s)) return stbi__png_load(s, x, y, comp, req_comp, ri);
  if (stbi__gif_test(s)) return stbi__gif_load(s, x, y, comp, req_comp, ri);

  // then the formats that can end up attempting to load with just 1 or 2
  // bytes matching expectations; these are prone to false positives, so
  // try them later
  if (stbi__jpeg_test(s)) return stbi__jpeg_load(s, x, y, comp, req_comp, ri);
  if (stbi__pnm_test(s)) return stbi__pnm_load(s, x, y, comp, req_comp, ri);

  return stbi__errpuc("unknown image type",
                      "Image not of any known type, or corrupt");
}

unsigned char *stbi__convert_16_to_8(uint16_t *orig, int w, int h,
                                     int channels) {
  int i;
  int img_len = w * h * channels;
  unsigned char *reduced = xmalloc(img_len);
  for (i = 0; i < img_len; ++i) {
    // top half of each byte is sufficient
    // approx of 16->8 bit scaling
    reduced[i] = (orig[i] >> 8) & 0xff;
  }
  free(orig);
  return reduced;
}

uint16_t *stbi__convert_8_to_16(unsigned char *orig, int w, int h,
                                int channels) {
  int i;
  int img_len = w * h * channels;
  uint16_t *enlarged = xmalloc(img_len * 2);
  for (i = 0; i < img_len; ++i) {
    // replicate to high and low byte, maps 0->0, 255->0xffff
    enlarged[i] = (uint16_t)((orig[i] << 8) + orig[i]);
  }
  free(orig);
  return enlarged;
}

static void stbi__vertical_flip(void *image, int w, int h,
                                int bytes_per_pixel) {
  int row;
  size_t bytes_per_row, bytes_left, bytes_copy;
  unsigned char *row0, *row1, *bytes, temp[2048];
  bytes = image;
  bytes_per_row = bytes_per_pixel * w;
  for (row = 0; row < (h >> 1); row++) {
    row0 = bytes + row * bytes_per_row;
    row1 = bytes + (h - row - 1) * bytes_per_row;
    // swap row0 with row1
    bytes_left = bytes_per_row;
    while (bytes_left) {
      bytes_copy = bytes_left < sizeof(temp) ? bytes_left : sizeof(temp);
      memcpy(temp, row0, bytes_copy);
      memcpy(row0, row1, bytes_copy);
      memcpy(row1, temp, bytes_copy);
      row0 += bytes_copy;
      row1 += bytes_copy;
      bytes_left -= bytes_copy;
    }
  }
}

static void stbi__vertical_flip_slices(void *image, int w, int h, int z,
                                       int bytes_per_pixel) {
  unsigned char *bytes;
  int slice, slice_size;
  bytes = image;
  slice_size = w * h * bytes_per_pixel;
  for (slice = 0; slice < z; ++slice) {
    stbi__vertical_flip(bytes, w, h, bytes_per_pixel);
    bytes += slice_size;
  }
}

static unsigned char *stbi__load_and_postprocess_8bit(stbi__context *s, int *x,
                                                      int *y, int *comp,
                                                      int req_comp) {
  void *result;
  stbi__result_info ri;
  result = stbi__load_main(s, x, y, comp, req_comp, &ri);
  if (result == NULL) return NULL;
  assert(ri.bits_per_channel == 8 || ri.bits_per_channel == 16);
  if (ri.bits_per_channel != 8) {
    // https://github.com/nothings/stb/pull/1497
    unsigned char *converted = stbi__convert_16_to_8(
        (uint16_t *)result, *x, *y, req_comp == 0 ? *comp : req_comp);
    if (converted == NULL) {
      free(result);
      return NULL;
    }
    result = converted;
    ri.bits_per_channel = 8;
  }
  // @TODO: move stbi__convert_format to here
  if (stbi__vertically_flip_on_load) {
    int channels = req_comp ? req_comp : *comp;
    stbi__vertical_flip(result, *x, *y, channels * sizeof(unsigned char));
  }
  return (unsigned char *)result;
}

static uint16_t *stbi__load_and_postprocess_16bit(stbi__context *s, int *x,
                                                  int *y, int *comp,
                                                  int req_comp) {
  void *result;
  stbi__result_info ri;
  result = stbi__load_main(s, x, y, comp, req_comp, &ri);
  if (result == NULL) return NULL;
  assert(ri.bits_per_channel == 8 || ri.bits_per_channel == 16);
  if (ri.bits_per_channel != 16) {
    // https://github.com/nothings/stb/pull/1497
    uint16_t *converted = stbi__convert_8_to_16(
        (unsigned char *)result, *x, *y, req_comp == 0 ? *comp : req_comp);
    if (converted == NULL) {
      free(result);
      return NULL;
    }
    result = converted;
    ri.bits_per_channel = 16;
  }
  // @TODO: move stbi__convert_format16 to here
  // @TODO: special case RGB-to-Y (and RGBA-to-YA) for 8-bit-to-16-bit case to
  // keep more precision
  if (stbi__vertically_flip_on_load) {
    int channels = req_comp ? req_comp : *comp;
    stbi__vertical_flip(result, *x, *y, channels * sizeof(uint16_t));
  }

  return (uint16_t *)result;
}

static FILE *stbi__fopen(char const *filename, char const *mode) {
  return fopen(filename, mode);
}

unsigned char *stbi_load(char const *filename, int *x, int *y, int *comp,
                         int req_comp) {
  FILE *f = stbi__fopen(filename, "rb");
  unsigned char *result;
  if (!f) return stbi__errpuc("can't fopen", "Unable to open file");
  result = stbi_load_from_file(f, x, y, comp, req_comp);
  fclose(f);
  return result;
}

unsigned char *stbi_load_from_file(FILE *f, int *x, int *y, int *comp,
                                   int req_comp) {
  unsigned char *result;
  stbi__context s;
  stbi__start_file(&s, f);
  result = stbi__load_and_postprocess_8bit(&s, x, y, comp, req_comp);
  if (result) {
    // need to 'unget' all the characters in the IO buffer
    // https://github.com/nothings/stb/pull/1420
    if (fseek(f, -(int)(s.img_buffer_end - s.img_buffer), SEEK_CUR)) {
      // fseek() failed; we can no longer maintain the file cursor position
      // guarantee of this function, so return null.
      free(result);
      return stbi__errpuc("bad file",
                          "fseek() failed; seek position unreliable");
    }
  }
  return result;
}

uint16_t *stbi_load_from_file_16(FILE *f, int *x, int *y, int *comp,
                                 int req_comp) {
  uint16_t *result;
  stbi__context s;
  stbi__start_file(&s, f);
  result = stbi__load_and_postprocess_16bit(&s, x, y, comp, req_comp);
  if (result) {
    // need to 'unget' all the characters in the IO buffer
    // https://github.com/nothings/stb/pull/1420
    if (fseek(f, -(int)(s.img_buffer_end - s.img_buffer), SEEK_CUR)) {
      // fseek() failed; we can no longer maintain the file cursor position
      // guarantee of this function, so return null.
      free(result);
      return (uint16_t *)stbi__errpuc(
          "bad file", "fseek() failed; seek position unreliable");
    }
  }
  return result;
}

unsigned short *stbi_load_16(char const *filename, int *x, int *y, int *comp,
                             int req_comp) {
  FILE *f = stbi__fopen(filename, "rb");
  uint16_t *result;
  if (!f) return stbi__errpuc("can't fopen", "Unable to open file");
  result = stbi_load_from_file_16(f, x, y, comp, req_comp);
  fclose(f);
  return result;
}

unsigned short *stbi_load_16_from_memory(unsigned char const *buffer, int len,
                                         int *x, int *y, int *channels_in_file,
                                         int desired_channels) {
  stbi__context s;
  stbi__start_mem(&s, buffer, len);
  return stbi__load_and_postprocess_16bit(&s, x, y, channels_in_file,
                                          desired_channels);
}

unsigned short *stbi_load_16_from_callbacks(stbi_io_callbacks const *clbk,
                                            void *user, int *x, int *y,
                                            int *channels_in_file,
                                            int desired_channels) {
  stbi__context s;
  stbi__start_callbacks(&s, (stbi_io_callbacks *)clbk, user);
  return stbi__load_and_postprocess_16bit(&s, x, y, channels_in_file,
                                          desired_channels);
}

unsigned char *stbi_load_from_memory(unsigned char const *buffer, int len,
                                     int *x, int *y, int *comp, int req_comp) {
  stbi__context s;
  stbi__start_mem(&s, buffer, len);
  return stbi__load_and_postprocess_8bit(&s, x, y, comp, req_comp);
}

unsigned char *stbi_load_from_callbacks(stbi_io_callbacks const *clbk,
                                        void *user, int *x, int *y, int *comp,
                                        int req_comp) {
  stbi__context s;
  stbi__start_callbacks(&s, (stbi_io_callbacks *)clbk, user);
  return stbi__load_and_postprocess_8bit(&s, x, y, comp, req_comp);
}

unsigned char *stbi_load_gif_from_memory(unsigned char const *buffer, int len,
                                         int **delays, int *x, int *y, int *z,
                                         int *comp, int req_comp) {
  unsigned char *result;
  stbi__context s;
  stbi__start_mem(&s, buffer, len);
  result =
      (unsigned char *)stbi__load_gif_main(&s, delays, x, y, z, comp, req_comp);
  if (stbi__vertically_flip_on_load) {
    stbi__vertical_flip_slices(result, *x, *y, *z, *comp);
  }
  return result;
}

enum { STBI__SCAN_load = 0, STBI__SCAN_type, STBI__SCAN_header };

static void stbi__refill_buffer(stbi__context *s) {
  int n = (s->io.read)(s->io_user_data, (char *)s->buffer_start, s->buflen);
  s->callback_already_read += (int)(s->img_buffer - s->img_buffer_original);
  if (n == 0) {
    // at end of file, treat same as if from memory, but need to handle case
    // where s->img_buffer isn't pointing to safe memory, e.g. 0-byte file
    s->read_from_callbacks = 0;
    s->img_buffer = s->buffer_start;
    s->img_buffer_end = s->buffer_start + 1;
    *s->img_buffer = 0;
  } else {
    s->img_buffer = s->buffer_start;
    s->img_buffer_end = s->buffer_start + n;
  }
}

forceinline unsigned char stbi__get8(stbi__context *s) {
  if (s->img_buffer < s->img_buffer_end) return *s->img_buffer++;
  if (s->read_from_callbacks) {
    stbi__refill_buffer(s);
    return *s->img_buffer++;
  }
  return 0;
}

forceinline int stbi__at_eof(stbi__context *s) {
  if (s->io.read) {
    if (!(s->io.eof)(s->io_user_data)) return 0;
    // if feof() is true, check if buffer = end
    // special case: we've only got the special 0 character at the end
    if (s->read_from_callbacks == 0) return 1;
  }

  return s->img_buffer >= s->img_buffer_end;
}

static void stbi__skip(stbi__context *s, int n) {
  if (n == 0) return;  // already there!
  if (n < 0) {
    s->img_buffer = s->img_buffer_end;
    return;
  }
  if (s->io.read) {
    int blen = (int)(s->img_buffer_end - s->img_buffer);
    if (blen < n) {
      s->img_buffer = s->img_buffer_end;
      (s->io.skip)(s->io_user_data, n - blen);
      return;
    }
  }
  s->img_buffer += n;
}

static int stbi__getn(stbi__context *s, unsigned char *buffer, int n) {
  if (s->io.read) {
    int blen = (int)(s->img_buffer_end - s->img_buffer);
    if (blen < n) {
      int res, count;
      memcpy(buffer, s->img_buffer, blen);
      count = (s->io.read)(s->io_user_data, (char *)buffer + blen, n - blen);
      res = (count == (n - blen));
      s->img_buffer = s->img_buffer_end;
      return res;
    }
  }
  if (s->img_buffer + n <= s->img_buffer_end) {
    memcpy(buffer, s->img_buffer, n);
    s->img_buffer += n;
    return 1;
  } else {
    return 0;
  }
}

static int stbi__get16le(stbi__context *s) {
  int z = stbi__get8(s);
  return z + (stbi__get8(s) << 8);
}

static int stbi__get16be(stbi__context *s) {
  int z = stbi__get8(s);
  return (z << 8) + stbi__get8(s);
}

static uint32_t stbi__get32be(stbi__context *s) {
  uint32_t z = stbi__get16be(s);
  return (z << 16) + stbi__get16be(s);
}

#define STBI__BYTECAST(x) \
  ((unsigned char)((x)&255))  // truncate int to byte without warnings

//////////////////////////////////////////////////////////////////////////////
//
//  generic converter from built-in img_n to req_comp
//    individual types do this automatically as much as possible (e.g. jpeg
//    does all cases internally since it needs to colorspace convert anyway,
//    and it never has alpha, so very few cases). png can automatically
//    interleave an alpha=255 channel, but falls back to this for other cases
//
//  assume data buffer is malloced, so malloc a new one and free that one
//  only failure mode is malloc failing

static unsigned char stbi__compute_y(int r, int g, int b) {
  return (unsigned char)(((r * 77) + (g * 150) + (29 * b)) >> 8);
}

static unsigned char *stbi__convert_format(unsigned char *data, int img_n,
                                           int req_comp, unsigned int x,
                                           unsigned int y) {
  int i, j;
  unsigned char *good, *src, *dest;
  if (req_comp == img_n) return data;
  assert(req_comp >= 1 && req_comp <= 4);
  good = stbi__malloc_mad3(req_comp, x, y, 0);
  for (j = 0; j < (int)y; ++j) {
    src = data + j * x * img_n;
    dest = good + j * x * req_comp;
#define STBI__COMBO(a, b) ((a)*8 + (b))
#define STBI__CASE(a, b)  \
  case STBI__COMBO(a, b): \
    for (i = x - 1; i >= 0; --i, src += a, dest += b)
    // convert source image with img_n components to one with req_comp
    // components; avoid switch per pixel, so use switch per scanline and
    // massive macros
    switch (STBI__COMBO(img_n, req_comp)) {
      STBI__CASE(1, 2) {
        dest[0] = src[0];
        dest[1] = 255;
      }
      break;
      STBI__CASE(1, 3) {
        dest[0] = dest[1] = dest[2] = src[0];
      }
      break;
      STBI__CASE(1, 4) {
        dest[0] = dest[1] = dest[2] = src[0];
        dest[3] = 255;
      }
      break;
      STBI__CASE(2, 1) {
        dest[0] = src[0];
      }
      break;
      STBI__CASE(2, 3) {
        dest[0] = dest[1] = dest[2] = src[0];
      }
      break;
      STBI__CASE(2, 4) {
        dest[0] = dest[1] = dest[2] = src[0];
        dest[3] = src[1];
      }
      break;
      STBI__CASE(3, 4) {
        dest[0] = src[0];
        dest[1] = src[1];
        dest[2] = src[2];
        dest[3] = 255;
      }
      break;
      STBI__CASE(3, 1) {
        dest[0] = stbi__compute_y(src[0], src[1], src[2]);
      }
      break;
      STBI__CASE(3, 2) {
        dest[0] = stbi__compute_y(src[0], src[1], src[2]);
        dest[1] = 255;
      }
      break;
      STBI__CASE(4, 1) {
        dest[0] = stbi__compute_y(src[0], src[1], src[2]);
      }
      break;
      STBI__CASE(4, 2) {
        dest[0] = stbi__compute_y(src[0], src[1], src[2]);
        dest[1] = src[3];
      }
      break;
      STBI__CASE(4, 3) {
        dest[0] = src[0];
        dest[1] = src[1];
        dest[2] = src[2];
      }
      break;
      default:
        assert(0);
        free(data);
        free(good);
        return stbi__errpuc("unsupported", "Unsupported format conversion");
    }
#undef STBI__CASE
  }
  free(data);
  return good;
}

static uint16_t stbi__compute_y_16(int r, int g, int b) {
  return (uint16_t)(((r * 77) + (g * 150) + (29 * b)) >> 8);
}

static uint16_t *stbi__convert_format16(uint16_t *data, int img_n, int req_comp,
                                        unsigned int x, unsigned int y) {
  int i, j;
  uint16_t *good;

  if (req_comp == img_n) return data;
  assert(req_comp >= 1 && req_comp <= 4);

  good = xmalloc(req_comp * x * y * 2);

  for (j = 0; j < (int)y; ++j) {
    uint16_t *src = data + j * x * img_n;
    uint16_t *dest = good + j * x * req_comp;

#define STBI__COMBO(a, b) ((a)*8 + (b))
#define STBI__CASE(a, b)  \
  case STBI__COMBO(a, b): \
    for (i = x - 1; i >= 0; --i, src += a, dest += b)
    // convert source image with img_n components to one with req_comp
    // components; avoid switch per pixel, so use switch per scanline and
    // massive macros
    switch (STBI__COMBO(img_n, req_comp)) {
      STBI__CASE(1, 2) {
        dest[0] = src[0];
        dest[1] = 0xffff;
      }
      break;
      STBI__CASE(1, 3) {
        dest[0] = dest[1] = dest[2] = src[0];
      }
      break;
      STBI__CASE(1, 4) {
        dest[0] = dest[1] = dest[2] = src[0];
        dest[3] = 0xffff;
      }
      break;
      STBI__CASE(2, 1) {
        dest[0] = src[0];
      }
      break;
      STBI__CASE(2, 3) {
        dest[0] = dest[1] = dest[2] = src[0];
      }
      break;
      STBI__CASE(2, 4) {
        dest[0] = dest[1] = dest[2] = src[0];
        dest[3] = src[1];
      }
      break;
      STBI__CASE(3, 4) {
        dest[0] = src[0];
        dest[1] = src[1];
        dest[2] = src[2];
        dest[3] = 0xffff;
      }
      break;
      STBI__CASE(3, 1) {
        dest[0] = stbi__compute_y_16(src[0], src[1], src[2]);
      }
      break;
      STBI__CASE(3, 2) {
        dest[0] = stbi__compute_y_16(src[0], src[1], src[2]);
        dest[1] = 0xffff;
      }
      break;
      STBI__CASE(4, 1) {
        dest[0] = stbi__compute_y_16(src[0], src[1], src[2]);
      }
      break;
      STBI__CASE(4, 2) {
        dest[0] = stbi__compute_y_16(src[0], src[1], src[2]);
        dest[1] = src[3];
      }
      break;
      STBI__CASE(4, 3) {
        dest[0] = src[0];
        dest[1] = src[1];
        dest[2] = src[2];
      }
      break;
      default:
        assert(0);
        free(data);
        free(good);
        return (uint16_t *)stbi__errpuc("unsupported",
                                        "Unsupported format conversion");
    }
#undef STBI__CASE
  }

  free(data);
  return good;
}

//////////////////////////////////////////////////////////////////////////////
//
//  "baseline" JPEG/JFIF decoder
//
//    simple implementation
//      - doesn't support delayed output of y-dimension
//      - simple interface (only one output format: 8-bit interleaved RGB)
//      - doesn't try to recover corrupt jpegs
//      - doesn't allow partial loading, loading multiple at once
//      - still fast on x86 (copying globals into locals doesn't help x86)
//      - allocates lots of intermediate memory (full size of all components)
//        - non-interleaved case requires this anyway
//        - allows good upsampling (see next)
//    high-quality
//      - upsampled channels are bilinearly interpolated, even across blocks
//      - quality integer IDCT derived from IJG's 'slow'
//    performance
//      - fast huffman; reasonable integer IDCT
//      - some SIMD kernels for common paths on targets with SSE2/NEON
//      - uses a lot of intermediate memory, could cache poorly

// huffman decoding acceleration
#define FAST_BITS 9  // larger handles more cases; smaller stomps less cache

typedef struct {
  unsigned char fast[1 << FAST_BITS];
  // weirdly, repacking this into AoS is a 10% speed loss, instead of a win
  uint16_t code[256];
  unsigned char values[256];
  unsigned char size[257];
  unsigned int maxcode[18];
  int delta[17];  // old 'firstsymbol' - old 'firstcode'
} stbi__huffman;

typedef struct {
  stbi__context *s;
  stbi__huffman huff_dc[4];
  stbi__huffman huff_ac[4];
  uint16_t dequant[4][64];
  int16_t fast_ac[4][1 << FAST_BITS];

  // sizes for components, interleaved MCUs
  int img_h_max, img_v_max;
  int img_mcu_x, img_mcu_y;
  int img_mcu_w, img_mcu_h;

  // definition of jpeg image component
  struct {
    int id;
    int h, v;
    int tq;
    int hd, ha;
    int dc_pred;

    int x, y, w2, h2;
    unsigned char *data;
    unsigned char *linebuf;
    short *coeff;          // progressive only
    int coeff_w, coeff_h;  // number of 8x8 coefficient blocks
  } img_comp[4];

  uint32_t code_buffer;  // jpeg entropy-coded buffer
  int code_bits;         // number of valid bits
  unsigned char marker;  // marker seen while filling entropy buffer
  int nomore;            // flag if we saw a marker so must stop

  int progressive;
  int spec_start;
  int spec_end;
  int succ_high;
  int succ_low;
  int eob_run;
  int jfif;
  int app14_color_transform;  // Adobe APP14 tag
  int rgb;

  int scan_n, order[4];
  int restart_interval, todo;

  // kernels
  unsigned char *(*resample_row_hv_2_kernel)(unsigned char *out,
                                             unsigned char *in_near,
                                             unsigned char *in_far, int w,
                                             int hs);
} stbi__jpeg;

static int stbi__build_huffman(stbi__huffman *h, int *count) {
  int i, j, k = 0;
  unsigned int code;
  // build size list for each symbol (from JPEG spec)
  for (i = 0; i < 16; ++i) {
    for (j = 0; j < count[i]; ++j) {
      h->size[k++] = (unsigned char)(i + 1);
      if (k >= 257) {
        return stbi__err("bad size list", "Corrupt JPEG");
      }
    }
  }
  h->size[k] = 0;

  // compute actual symbols (from jpeg spec)
  code = 0;
  k = 0;
  for (j = 1; j <= 16; ++j) {
    // compute delta to add to code to compute symbol id
    h->delta[j] = k - code;
    if (h->size[k] == j) {
      while (h->size[k] == j) h->code[k++] = (uint16_t)(code++);
      if (code - 1 >= (1u << j)) {
        return stbi__err("bad code lengths", "Corrupt JPEG");
      }
    }
    // compute largest code + 1 for this size, preshifted as needed later
    h->maxcode[j] = code << (16 - j);
    code <<= 1;
  }
  h->maxcode[j] = 0xffffffff;

  // build non-spec acceleration table; 255 is flag for not-accelerated
  memset(h->fast, 255, 1 << FAST_BITS);
  for (i = 0; i < k; ++i) {
    int s = h->size[i];
    if (s <= FAST_BITS) {
      int c = h->code[i] << (FAST_BITS - s);
      int m = 1 << (FAST_BITS - s);
      for (j = 0; j < m; ++j) {
        h->fast[c + j] = (unsigned char)i;
      }
    }
  }
  return 1;
}

// build a table that decodes both magnitude and value of small ACs in
// one go.
static void stbi__build_fast_ac(int16_t *fast_ac, stbi__huffman *h) {
  int i;
  for (i = 0; i < (1 << FAST_BITS); ++i) {
    unsigned char fast = h->fast[i];
    fast_ac[i] = 0;
    if (fast < 255) {
      int rs = h->values[fast];
      int run = (rs >> 4) & 15;
      int magbits = rs & 15;
      int len = h->size[fast];
      if (magbits && len + magbits <= FAST_BITS) {
        // magnitude code followed by receive_extend code
        int k = ((i << len) & ((1 << FAST_BITS) - 1)) >> (FAST_BITS - magbits);
        int m = 1 << (magbits - 1);
        if (k < m) k += (~0U << magbits) + 1;
        // if the result is small enough, we can fit it in fast_ac table
        if (k >= -128 && k <= 127)
          fast_ac[i] = (int16_t)((k * 256) + (run * 16) + (len + magbits));
      }
    }
  }
}

static void stbi__grow_buffer_unsafe(stbi__jpeg *j) {
  do {
    unsigned b = j->nomore ? 0 : stbi__get8(j->s);
    if (b == 0xff) {
      int c = stbi__get8(j->s);
      while (c == 0xff) c = stbi__get8(j->s);  // consume fill bytes
      if (c != 0) {
        j->marker = (unsigned char)c;
        j->nomore = 1;
        return;
      }
    }
    j->code_buffer |= b << (24 - j->code_bits);
    j->code_bits += 8;
  } while (j->code_bits <= 24);
}

// (1 << n) - 1
static const uint32_t stbi__bmask[17] = {0,    1,    3,     7,     15,   31,
                                         63,   127,  255,   511,   1023, 2047,
                                         4095, 8191, 16383, 32767, 65535};

// decode a jpeg huffman value from the bitstream
forceinline int stbi__jpeg_huff_decode(stbi__jpeg *j, stbi__huffman *h) {
  unsigned int temp;
  int c, k;

  if (j->code_bits < 16) stbi__grow_buffer_unsafe(j);

  // look at the top FAST_BITS and determine what symbol ID it is,
  // if the code is <= FAST_BITS
  c = (j->code_buffer >> (32 - FAST_BITS)) & ((1 << FAST_BITS) - 1);
  k = h->fast[c];
  if (k < 255) {
    int s = h->size[k];
    if (s > j->code_bits) return -1;
    j->code_buffer <<= s;
    j->code_bits -= s;
    return h->values[k];
  }

  // naive test is to shift the code_buffer down so k bits are
  // valid, then test against maxcode. To speed this up, we've
  // preshifted maxcode left so that it has (16-k) 0s at the
  // end; in other words, regardless of the number of bits, it
  // wants to be compared against something shifted to have 16;
  // that way we don't need to shift inside the loop.
  temp = j->code_buffer >> 16;
  for (k = FAST_BITS + 1;; ++k)
    if (temp < h->maxcode[k]) break;
  if (k == 17) {
    WARNF("j->code_bits: %d", j->code_bits);
    // error! code not found
    j->code_bits -= 16;
    WARNF("Symbol: %d", k);
    return -1;
  }

  if (k > j->code_bits) return -1;

  // convert the huffman code to the symbol id
  c = ((j->code_buffer >> (32 - k)) & stbi__bmask[k]) + h->delta[k];
  if (c < 0 || c >= 256) {
    // symbol id out of bounds!
    return -1;
  }
  assert((((j->code_buffer) >> (32 - h->size[c])) & stbi__bmask[h->size[c]]) ==
         h->code[c]);

  // convert the id to a symbol
  j->code_bits -= k;
  j->code_buffer <<= k;
  return h->values[c];
}

// bias[n] = (-1 << n) + 1
static const int stbi__jbias[16] = {0,     -1,    -3,     -7,    -15,   -31,
                                    -63,   -127,  -255,   -511,  -1023, -2047,
                                    -4095, -8191, -16383, -32767};

// combined JPEG 'receive' and JPEG 'extend', since baseline
// always extends everything it receives.
forceinline int stbi__extend_receive(stbi__jpeg *j, int n) {
  int sgn;
  unsigned int k;
  if (j->code_bits < n) stbi__grow_buffer_unsafe(j);
  if (j->code_bits < n) {
    // ran out of bits from stream, return 0s intead of continuing
    return 0;
  }
  // sign bit is always in MSB;
  // 0 if MSB clear (positive), 1 if MSB set (negative)
  sgn = j->code_buffer >> 31;
  k = ROL(j->code_buffer, n);
  j->code_buffer = k & ~stbi__bmask[n];
  k &= stbi__bmask[n];
  j->code_bits -= n;
  return k + (stbi__jbias[n] & (sgn - 1));
}

// get some unsigned bits
forceinline int stbi__jpeg_get_bits(stbi__jpeg *j, int n) {
  unsigned int k;
  if (j->code_bits < n) stbi__grow_buffer_unsafe(j);
  if (j->code_bits < n) {
    // ran out of bits from stream, return 0s intead of continuing
    return 0;
  }
  k = ROL(j->code_buffer, n);
  j->code_buffer = k & ~stbi__bmask[n];
  k &= stbi__bmask[n];
  j->code_bits -= n;
  return k;
}

forceinline int stbi__jpeg_get_bit(stbi__jpeg *j) {
  unsigned int k;
  if (j->code_bits < 1) stbi__grow_buffer_unsafe(j);
  if (j->code_bits < 1) {
    // ran out of bits from stream, return 0s intead of continuing
    return 0;
  }
  k = j->code_buffer;
  j->code_buffer <<= 1;
  --j->code_bits;
  return k & 0x80000000;
}

// given a value that's at position X in the zigzag stream,
// where does it appear in the 8x8 matrix coded as row-major?
static const unsigned char stbi__jpeg_dezigzag[64 + 15] = {
    0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5, 12, 19, 26, 33, 40,
    48, 41, 34, 27, 20, 13, 6, 7, 14, 21, 28, 35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51, 58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61,
    54, 47, 55, 62, 63,
    // let corrupt input sample past end
    63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63};

// decode one 64-entry block
static int stbi__jpeg_decode_block(stbi__jpeg *j, short data[64],
                                   stbi__huffman *hdc, stbi__huffman *hac,
                                   int16_t *fac, int b, uint16_t *dequant) {
  unsigned int zig;
  int diff, dc, k, t, c, r, s, rs;
  if (j->code_bits < 16) stbi__grow_buffer_unsafe(j);
  t = stbi__jpeg_huff_decode(j, hdc);
  if (t < 0 || t > 15) return stbi__err("bad huffman code", "Corrupt JPEG");
  // 0 all the ac values now so we can do it 32-bits at a time
  bzero(data, 64 * sizeof(data[0]));
  diff = t ? stbi__extend_receive(j, t) : 0;
  if (!stbi__addints_valid(j->img_comp[b].dc_pred, diff)) {
    return stbi__err("bad delta", "Corrupt JPEG");
  }
  dc = j->img_comp[b].dc_pred + diff;
  j->img_comp[b].dc_pred = dc;
  if (!stbi__mul2shorts_valid(dc, dequant[0])) {
    return stbi__err("can't merge dc and ac", "Corrupt JPEG");
  }
  data[0] = (short)(dc * dequant[0]);
  // decode AC components, see JPEG spec
  k = 1;
  do {
    if (j->code_bits < 16) stbi__grow_buffer_unsafe(j);
    c = (j->code_buffer >> (32 - FAST_BITS)) & ((1 << FAST_BITS) - 1);
    r = fac[c];
    if (r) {               // fast-AC path
      k += (r >> 4) & 15;  // run
      s = r & 15;          // combined length
      if (s > j->code_bits) {
        return stbi__err("bad huffman code",
                         "Combined length longer than code bits available");
      }
      j->code_buffer <<= s;
      j->code_bits -= s;
      // decode into unzigzag'd location
      zig = stbi__jpeg_dezigzag[k++];
      data[zig] = (short)((r >> 8) * dequant[zig]);
    } else {
      rs = stbi__jpeg_huff_decode(j, hac);
      if (rs < 0) return stbi__err("bad huffman code", "Corrupt JPEG");
      s = rs & 15;
      r = rs >> 4;
      if (s == 0) {
        if (rs != 0xf0) break;  // end block
        k += 16;
      } else {
        k += r;
        // decode into unzigzag'd location
        zig = stbi__jpeg_dezigzag[k++];
        data[zig] = (short)(stbi__extend_receive(j, s) * dequant[zig]);
      }
    }
  } while (k < 64);
  return 1;
}

static int stbi__jpeg_decode_block_prog_dc(stbi__jpeg *j, short data[64],
                                           stbi__huffman *hdc, int b) {
  int t;
  int diff, dc;
  if (j->spec_end != 0) {
    return stbi__err("can't merge dc and ac", "Corrupt JPEG");
  }
  if (j->code_bits < 16) stbi__grow_buffer_unsafe(j);
  if (j->succ_high == 0) {
    // first scan for DC coefficient, must be first
    bzero(data, 64 * sizeof(data[0]));  // 0 all the ac values now
    t = stbi__jpeg_huff_decode(j, hdc);
    if (t < 0 || t > 15) {
      return stbi__err("can't merge dc and ac", "Corrupt JPEG");
    }
    diff = t ? stbi__extend_receive(j, t) : 0;
    if (!stbi__addints_valid(j->img_comp[b].dc_pred, diff)) {
      return stbi__err("bad delta", "Corrupt JPEG");
    }
    dc = j->img_comp[b].dc_pred + diff;
    j->img_comp[b].dc_pred = dc;
    if (!stbi__mul2shorts_valid(dc, 1 << j->succ_low)) {
      return stbi__err("can't merge dc and ac", "Corrupt JPEG");
    }
    data[0] = (short)(dc * (1u << j->succ_low));
  } else {
    // refinement scan for DC coefficient
    if (stbi__jpeg_get_bit(j)) data[0] += (short)(1u << j->succ_low);
  }
  return 1;
}

// @OPTIMIZE: store non-zigzagged during the decode passes,
// and only de-zigzag when dequantizing
static int stbi__jpeg_decode_block_prog_ac(stbi__jpeg *j, short data[64],
                                           stbi__huffman *hac, int16_t *fac) {
  short bit;
  unsigned zig;
  int k, c, r, s, rs, shift;
  if (j->spec_start == 0) {
    return stbi__err("can't merge dc and ac", "Corrupt JPEG");
  }
  if (j->succ_high == 0) {
    shift = j->succ_low;
    if (j->eob_run) {
      --j->eob_run;
      return 1;
    }
    k = j->spec_start;
    do {
      if (j->code_bits < 16) stbi__grow_buffer_unsafe(j);
      c = (j->code_buffer >> (32 - FAST_BITS)) & ((1 << FAST_BITS) - 1);
      r = fac[c];
      if (r) {               // fast-AC path
        k += (r >> 4) & 15;  // run
        s = r & 15;          // combined length
        if (s > j->code_bits) {
          return stbi__err("bad huffman code",
                           "Combined length longer than code bits available");
        }
        j->code_buffer <<= s;
        j->code_bits -= s;
        zig = stbi__jpeg_dezigzag[k++];
        data[zig] = (short)((r >> 8) * (1u << shift));
      } else {
        rs = stbi__jpeg_huff_decode(j, hac);
        if (rs < 0) return stbi__err("bad huffman code", "Corrupt JPEG");
        s = rs & 15;
        r = rs >> 4;
        if (s == 0) {
          if (r < 15) {
            j->eob_run = (1 << r);
            if (r) j->eob_run += stbi__jpeg_get_bits(j, r);
            --j->eob_run;
            break;
          }
          k += 16;
        } else {
          k += r;
          zig = stbi__jpeg_dezigzag[k++];
          data[zig] = (short)(stbi__extend_receive(j, s) * (1u << shift));
        }
      }
    } while (k <= j->spec_end);
  } else {
    // refinement scan for these AC coefficients
    bit = (short)(1u << j->succ_low);
    if (j->eob_run) {
      --j->eob_run;
      for (k = j->spec_start; k <= j->spec_end; ++k) {
        short *p = &data[stbi__jpeg_dezigzag[k]];
        if (*p != 0)
          if (stbi__jpeg_get_bit(j))
            if ((*p & bit) == 0) {
              if (*p > 0)
                *p += bit;
              else
                *p -= bit;
            }
      }
    } else {
      k = j->spec_start;
      do {
        rs = stbi__jpeg_huff_decode(j, hac);
        if (rs < 0) return stbi__err("bad huffman code", "Corrupt JPEG");
        s = rs & 15;
        r = rs >> 4;
        if (s == 0) {
          if (r < 15) {
            j->eob_run = (1 << r) - 1;
            if (r) j->eob_run += stbi__jpeg_get_bits(j, r);
            r = 64;  // force end of block
          } else {
            // r=15 s=0 should write 16 0s, so we just do
            // a run of 15 0s and then write s (which is 0),
            // so we don't have to do anything special here
          }
        } else {
          if (s != 1) return stbi__err("bad huffman code", "Corrupt JPEG");
          // sign bit
          if (stbi__jpeg_get_bit(j)) {
            s = bit;
          } else {
            s = -bit;
          }
        }
        // advance by r
        while (k <= j->spec_end) {
          short *p = &data[stbi__jpeg_dezigzag[k++]];
          if (*p != 0) {
            if (stbi__jpeg_get_bit(j))
              if ((*p & bit) == 0) {
                if (*p > 0)
                  *p += bit;
                else
                  *p -= bit;
              }
          } else {
            if (r == 0) {
              *p = (short)s;
              break;
            }
            --r;
          }
        }
      } while (k <= j->spec_end);
    }
  }
  return 1;
}

// take a -128..127 value and stbi__clamp it and convert to 0..255
forceinline unsigned char stbi__clamp(int x) {
  // trick to use a single test to catch both cases
  if ((unsigned int)x > 255) {
    if (x < 0) return 0;
    if (x > 255) return 255;
  }
  return (unsigned char)x;
}

#define stbi__f2f(x) ((int)(((x)*4096 + 0.5)))
#define stbi__fsh(x) ((x)*4096)

// derived from jidctint -- DCT_ISLOW
#define STBI__IDCT_1D(s0, s1, s2, s3, s4, s5, s6, s7)     \
  int t0, t1, t2, t3, p1, p2, p3, p4, p5, x0, x1, x2, x3; \
  p2 = s2;                                                \
  p3 = s6;                                                \
  p1 = (p2 + p3) * stbi__f2f(0.5411961f);                 \
  t2 = p1 + p3 * stbi__f2f(-1.847759065f);                \
  t3 = p1 + p2 * stbi__f2f(0.765366865f);                 \
  p2 = s0;                                                \
  p3 = s4;                                                \
  t0 = stbi__fsh(p2 + p3);                                \
  t1 = stbi__fsh(p2 - p3);                                \
  x0 = t0 + t3;                                           \
  x3 = t0 - t3;                                           \
  x1 = t1 + t2;                                           \
  x2 = t1 - t2;                                           \
  t0 = s7;                                                \
  t1 = s5;                                                \
  t2 = s3;                                                \
  t3 = s1;                                                \
  p3 = t0 + t2;                                           \
  p4 = t1 + t3;                                           \
  p1 = t0 + t3;                                           \
  p2 = t1 + t2;                                           \
  p5 = (p3 + p4) * stbi__f2f(1.175875602f);               \
  t0 = t0 * stbi__f2f(0.298631336f);                      \
  t1 = t1 * stbi__f2f(2.053119869f);                      \
  t2 = t2 * stbi__f2f(3.072711026f);                      \
  t3 = t3 * stbi__f2f(1.501321110f);                      \
  p1 = p5 + p1 * stbi__f2f(-0.899976223f);                \
  p2 = p5 + p2 * stbi__f2f(-2.562915447f);                \
  p3 = p3 * stbi__f2f(-1.961570560f);                     \
  p4 = p4 * stbi__f2f(-0.390180644f);                     \
  t3 += p1 + p4;                                          \
  t2 += p2 + p3;                                          \
  t1 += p2 + p4;                                          \
  t0 += p1 + p3;

static void stbi__idct_block(unsigned char *out, int out_stride,
                             short data[64]) {
  int i, val[64], *v = val;
  unsigned char *o;
  short *d = data;

  // columns
  for (i = 0; i < 8; ++i, ++d, ++v) {
    // if all zeroes, shortcut -- this avoids dequantizing 0s and IDCTing
    if (d[8] == 0 && d[16] == 0 && d[24] == 0 && d[32] == 0 && d[40] == 0 &&
        d[48] == 0 && d[56] == 0) {
      //    no shortcut                 0     seconds
      //    (1|2|3|4|5|6|7)==0          0     seconds
      //    all separate               -0.047 seconds
      //    1 && 2|3 && 4|5 && 6|7:    -0.047 seconds
      int dcterm = d[0] * 4;
      v[0] = v[8] = v[16] = v[24] = v[32] = v[40] = v[48] = v[56] = dcterm;
    } else {
      STBI__IDCT_1D(d[0], d[8], d[16], d[24], d[32], d[40], d[48], d[56])
      // constants scaled things up by 1<<12; let's bring them back
      // down, but keep 2 extra bits of precision
      x0 += 512;
      x1 += 512;
      x2 += 512;
      x3 += 512;
      v[0] = (x0 + t3) >> 10;
      v[56] = (x0 - t3) >> 10;
      v[8] = (x1 + t2) >> 10;
      v[48] = (x1 - t2) >> 10;
      v[16] = (x2 + t1) >> 10;
      v[40] = (x2 - t1) >> 10;
      v[24] = (x3 + t0) >> 10;
      v[32] = (x3 - t0) >> 10;
    }
  }

  for (i = 0, v = val, o = out; i < 8; ++i, v += 8, o += out_stride) {
    // no fast case since the first 1D IDCT spread components out
    STBI__IDCT_1D(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7])
    // constants scaled things up by 1<<12, plus we had 1<<2 from first
    // loop, plus horizontal and vertical each scale by sqrt(8) so together
    // we've got an extra 1<<3, so 1<<17 total we need to remove.
    // so we want to round that, which means adding 0.5 * 1<<17,
    // aka 65536. Also, we'll end up with -128 to 127 that we want
    // to encode as 0..255 by adding 128, so we'll add that before the shift
    x0 += 65536 + (128 << 17);
    x1 += 65536 + (128 << 17);
    x2 += 65536 + (128 << 17);
    x3 += 65536 + (128 << 17);
    // tried computing the shifts into temps, or'ing the temps to see
    // if any were out of range, but that was slower
    o[0] = stbi__clamp((x0 + t3) >> 17);
    o[7] = stbi__clamp((x0 - t3) >> 17);
    o[1] = stbi__clamp((x1 + t2) >> 17);
    o[6] = stbi__clamp((x1 - t2) >> 17);
    o[2] = stbi__clamp((x2 + t1) >> 17);
    o[5] = stbi__clamp((x2 - t1) >> 17);
    o[3] = stbi__clamp((x3 + t0) >> 17);
    o[4] = stbi__clamp((x3 - t0) >> 17);
  }
}

#ifdef STBI_SSE2
// sse2 integer IDCT. not the fastest possible implementation but it
// produces bit-identical results to the generic C version so it's
// fully "transparent".
static void stbi__idct_simd(unsigned char *out, int out_stride,
                            short data[64]) {
  // This is constructed to match our regular (generic) integer IDCT exactly.
  __m128i row0, row1, row2, row3, row4, row5, row6, row7;
  __m128i tmp;

// dot product constant: even elems=x, odd elems=y
#define dct_const(x, y) _mm_setr_epi16((x), (y), (x), (y), (x), (y), (x), (y))

// out(0) = c0[even]*x + c0[odd]*y   (c0, x, y 16-bit, out 32-bit)
// out(1) = c1[even]*x + c1[odd]*y
#define dct_rot(out0, out1, x, y, c0, c1)        \
  __m128i c0##lo = _mm_unpacklo_epi16((x), (y)); \
  __m128i c0##hi = _mm_unpackhi_epi16((x), (y)); \
  __m128i out0##_l = _mm_madd_epi16(c0##lo, c0); \
  __m128i out0##_h = _mm_madd_epi16(c0##hi, c0); \
  __m128i out1##_l = _mm_madd_epi16(c0##lo, c1); \
  __m128i out1##_h = _mm_madd_epi16(c0##hi, c1)

// out = in << 12  (in 16-bit, out 32-bit)
#define dct_widen(out, in)                                              \
  __m128i out##_l =                                                     \
      _mm_srai_epi32(_mm_unpacklo_epi16(_mm_setzero_si128(), (in)), 4); \
  __m128i out##_h =                                                     \
      _mm_srai_epi32(_mm_unpackhi_epi16(_mm_setzero_si128(), (in)), 4)

// wide add
#define dct_wadd(out, a, b)                      \
  __m128i out##_l = _mm_add_epi32(a##_l, b##_l); \
  __m128i out##_h = _mm_add_epi32(a##_h, b##_h)

// wide sub
#define dct_wsub(out, a, b)                      \
  __m128i out##_l = _mm_sub_epi32(a##_l, b##_l); \
  __m128i out##_h = _mm_sub_epi32(a##_h, b##_h)

// butterfly a/b, add bias, then shift by "s" and pack
#define dct_bfly32o(out0, out1, a, b, bias, s)                               \
  {                                                                          \
    __m128i abiased_l = _mm_add_epi32(a##_l, bias);                          \
    __m128i abiased_h = _mm_add_epi32(a##_h, bias);                          \
    dct_wadd(sum, abiased, b);                                               \
    dct_wsub(dif, abiased, b);                                               \
    out0 =                                                                   \
        _mm_packs_epi32(_mm_srai_epi32(sum_l, s), _mm_srai_epi32(sum_h, s)); \
    out1 =                                                                   \
        _mm_packs_epi32(_mm_srai_epi32(dif_l, s), _mm_srai_epi32(dif_h, s)); \
  }

// 8-bit interleave step (for transposes)
#define dct_interleave8(a, b)  \
  tmp = a;                     \
  a = _mm_unpacklo_epi8(a, b); \
  b = _mm_unpackhi_epi8(tmp, b)

// 16-bit interleave step (for transposes)
#define dct_interleave16(a, b)  \
  tmp = a;                      \
  a = _mm_unpacklo_epi16(a, b); \
  b = _mm_unpackhi_epi16(tmp, b)

#define dct_pass(bias, shift)                        \
  {                                                  \
    /* even part */                                  \
    dct_rot(t2e, t3e, row2, row6, rot0_0, rot0_1);   \
    __m128i sum04 = _mm_add_epi16(row0, row4);       \
    __m128i dif04 = _mm_sub_epi16(row0, row4);       \
    dct_widen(t0e, sum04);                           \
    dct_widen(t1e, dif04);                           \
    dct_wadd(x0, t0e, t3e);                          \
    dct_wsub(x3, t0e, t3e);                          \
    dct_wadd(x1, t1e, t2e);                          \
    dct_wsub(x2, t1e, t2e);                          \
    /* odd part */                                   \
    dct_rot(y0o, y2o, row7, row3, rot2_0, rot2_1);   \
    dct_rot(y1o, y3o, row5, row1, rot3_0, rot3_1);   \
    __m128i sum17 = _mm_add_epi16(row1, row7);       \
    __m128i sum35 = _mm_add_epi16(row3, row5);       \
    dct_rot(y4o, y5o, sum17, sum35, rot1_0, rot1_1); \
    dct_wadd(x4, y0o, y4o);                          \
    dct_wadd(x5, y1o, y5o);                          \
    dct_wadd(x6, y2o, y5o);                          \
    dct_wadd(x7, y3o, y4o);                          \
    dct_bfly32o(row0, row7, x0, x7, bias, shift);    \
    dct_bfly32o(row1, row6, x1, x6, bias, shift);    \
    dct_bfly32o(row2, row5, x2, x5, bias, shift);    \
    dct_bfly32o(row3, row4, x3, x4, bias, shift);    \
  }

  __m128i rot0_0 = dct_const(stbi__f2f(0.5411961f),
                             stbi__f2f(0.5411961f) + stbi__f2f(-1.847759065f));
  __m128i rot0_1 = dct_const(stbi__f2f(0.5411961f) + stbi__f2f(0.765366865f),
                             stbi__f2f(0.5411961f));
  __m128i rot1_0 = dct_const(stbi__f2f(1.175875602f) + stbi__f2f(-0.899976223f),
                             stbi__f2f(1.175875602f));
  __m128i rot1_1 =
      dct_const(stbi__f2f(1.175875602f),
                stbi__f2f(1.175875602f) + stbi__f2f(-2.562915447f));
  __m128i rot2_0 = dct_const(stbi__f2f(-1.961570560f) + stbi__f2f(0.298631336f),
                             stbi__f2f(-1.961570560f));
  __m128i rot2_1 =
      dct_const(stbi__f2f(-1.961570560f),
                stbi__f2f(-1.961570560f) + stbi__f2f(3.072711026f));
  __m128i rot3_0 = dct_const(stbi__f2f(-0.390180644f) + stbi__f2f(2.053119869f),
                             stbi__f2f(-0.390180644f));
  __m128i rot3_1 =
      dct_const(stbi__f2f(-0.390180644f),
                stbi__f2f(-0.390180644f) + stbi__f2f(1.501321110f));

  // rounding biases in column/row passes, see stbi__idct_block for explanation.
  __m128i bias_0 = _mm_set1_epi32(512);
  __m128i bias_1 = _mm_set1_epi32(65536 + (128 << 17));

  // load
  row0 = _mm_load_si128((const __m128i *)(data + 0 * 8));
  row1 = _mm_load_si128((const __m128i *)(data + 1 * 8));
  row2 = _mm_load_si128((const __m128i *)(data + 2 * 8));
  row3 = _mm_load_si128((const __m128i *)(data + 3 * 8));
  row4 = _mm_load_si128((const __m128i *)(data + 4 * 8));
  row5 = _mm_load_si128((const __m128i *)(data + 5 * 8));
  row6 = _mm_load_si128((const __m128i *)(data + 6 * 8));
  row7 = _mm_load_si128((const __m128i *)(data + 7 * 8));

  // column pass
  dct_pass(bias_0, 10);

  {
    // 16bit 8x8 transpose pass 1
    dct_interleave16(row0, row4);
    dct_interleave16(row1, row5);
    dct_interleave16(row2, row6);
    dct_interleave16(row3, row7);

    // transpose pass 2
    dct_interleave16(row0, row2);
    dct_interleave16(row1, row3);
    dct_interleave16(row4, row6);
    dct_interleave16(row5, row7);

    // transpose pass 3
    dct_interleave16(row0, row1);
    dct_interleave16(row2, row3);
    dct_interleave16(row4, row5);
    dct_interleave16(row6, row7);
  }

  // row pass
  dct_pass(bias_1, 17);

  {
    // pack
    __m128i p0 = _mm_packus_epi16(row0, row1);  // a0a1a2a3...a7b0b1b2b3...b7
    __m128i p1 = _mm_packus_epi16(row2, row3);
    __m128i p2 = _mm_packus_epi16(row4, row5);
    __m128i p3 = _mm_packus_epi16(row6, row7);

    // 8bit 8x8 transpose pass 1
    dct_interleave8(p0, p2);  // a0e0a1e1...
    dct_interleave8(p1, p3);  // c0g0c1g1...

    // transpose pass 2
    dct_interleave8(p0, p1);  // a0c0e0g0...
    dct_interleave8(p2, p3);  // b0d0f0h0...

    // transpose pass 3
    dct_interleave8(p0, p2);  // a0b0c0d0...
    dct_interleave8(p1, p3);  // a4b4c4d4...

    // store
    _mm_storel_epi64((__m128i *)out, p0);
    out += out_stride;
    _mm_storel_epi64((__m128i *)out, _mm_shuffle_epi32(p0, 0x4e));
    out += out_stride;
    _mm_storel_epi64((__m128i *)out, p2);
    out += out_stride;
    _mm_storel_epi64((__m128i *)out, _mm_shuffle_epi32(p2, 0x4e));
    out += out_stride;
    _mm_storel_epi64((__m128i *)out, p1);
    out += out_stride;
    _mm_storel_epi64((__m128i *)out, _mm_shuffle_epi32(p1, 0x4e));
    out += out_stride;
    _mm_storel_epi64((__m128i *)out, p3);
    out += out_stride;
    _mm_storel_epi64((__m128i *)out, _mm_shuffle_epi32(p3, 0x4e));
  }

#undef dct_const
#undef dct_rot
#undef dct_widen
#undef dct_wadd
#undef dct_wsub
#undef dct_bfly32o
#undef dct_interleave8
#undef dct_interleave16
#undef dct_pass
}

#endif  // STBI_SSE2

#ifdef STBI_NEON

// NEON integer IDCT. should produce bit-identical
// results to the generic C version.
static void stbi__idct_simd(unsigned char *out, int out_stride,
                            short data[64]) {
  int16x8_t row0, row1, row2, row3, row4, row5, row6, row7;

  int16x4_t rot0_0 = vdup_n_s16(stbi__f2f(0.5411961f));
  int16x4_t rot0_1 = vdup_n_s16(stbi__f2f(-1.847759065f));
  int16x4_t rot0_2 = vdup_n_s16(stbi__f2f(0.765366865f));
  int16x4_t rot1_0 = vdup_n_s16(stbi__f2f(1.175875602f));
  int16x4_t rot1_1 = vdup_n_s16(stbi__f2f(-0.899976223f));
  int16x4_t rot1_2 = vdup_n_s16(stbi__f2f(-2.562915447f));
  int16x4_t rot2_0 = vdup_n_s16(stbi__f2f(-1.961570560f));
  int16x4_t rot2_1 = vdup_n_s16(stbi__f2f(-0.390180644f));
  int16x4_t rot3_0 = vdup_n_s16(stbi__f2f(0.298631336f));
  int16x4_t rot3_1 = vdup_n_s16(stbi__f2f(2.053119869f));
  int16x4_t rot3_2 = vdup_n_s16(stbi__f2f(3.072711026f));
  int16x4_t rot3_3 = vdup_n_s16(stbi__f2f(1.501321110f));

#define dct_long_mul(out, inq, coeff)                      \
  int32x4_t out##_l = vmull_s16(vget_low_s16(inq), coeff); \
  int32x4_t out##_h = vmull_s16(vget_high_s16(inq), coeff)

#define dct_long_mac(out, acc, inq, coeff)                          \
  int32x4_t out##_l = vmlal_s16(acc##_l, vget_low_s16(inq), coeff); \
  int32x4_t out##_h = vmlal_s16(acc##_h, vget_high_s16(inq), coeff)

#define dct_widen(out, inq)                               \
  int32x4_t out##_l = vshll_n_s16(vget_low_s16(inq), 12); \
  int32x4_t out##_h = vshll_n_s16(vget_high_s16(inq), 12)

// wide add
#define dct_wadd(out, a, b)                    \
  int32x4_t out##_l = vaddq_s32(a##_l, b##_l); \
  int32x4_t out##_h = vaddq_s32(a##_h, b##_h)

// wide sub
#define dct_wsub(out, a, b)                    \
  int32x4_t out##_l = vsubq_s32(a##_l, b##_l); \
  int32x4_t out##_h = vsubq_s32(a##_h, b##_h)

// butterfly a/b, then shift using "shiftop" by "s" and pack
#define dct_bfly32o(out0, out1, a, b, shiftop, s)              \
  {                                                            \
    dct_wadd(sum, a, b);                                       \
    dct_wsub(dif, a, b);                                       \
    out0 = vcombine_s16(shiftop(sum_l, s), shiftop(sum_h, s)); \
    out1 = vcombine_s16(shiftop(dif_l, s), shiftop(dif_h, s)); \
  }

#define dct_pass(shiftop, shift)                     \
  {                                                  \
    /* even part */                                  \
    int16x8_t sum26 = vaddq_s16(row2, row6);         \
    dct_long_mul(p1e, sum26, rot0_0);                \
    dct_long_mac(t2e, p1e, row6, rot0_1);            \
    dct_long_mac(t3e, p1e, row2, rot0_2);            \
    int16x8_t sum04 = vaddq_s16(row0, row4);         \
    int16x8_t dif04 = vsubq_s16(row0, row4);         \
    dct_widen(t0e, sum04);                           \
    dct_widen(t1e, dif04);                           \
    dct_wadd(x0, t0e, t3e);                          \
    dct_wsub(x3, t0e, t3e);                          \
    dct_wadd(x1, t1e, t2e);                          \
    dct_wsub(x2, t1e, t2e);                          \
    /* odd part */                                   \
    int16x8_t sum15 = vaddq_s16(row1, row5);         \
    int16x8_t sum17 = vaddq_s16(row1, row7);         \
    int16x8_t sum35 = vaddq_s16(row3, row5);         \
    int16x8_t sum37 = vaddq_s16(row3, row7);         \
    int16x8_t sumodd = vaddq_s16(sum17, sum35);      \
    dct_long_mul(p5o, sumodd, rot1_0);               \
    dct_long_mac(p1o, p5o, sum17, rot1_1);           \
    dct_long_mac(p2o, p5o, sum35, rot1_2);           \
    dct_long_mul(p3o, sum37, rot2_0);                \
    dct_long_mul(p4o, sum15, rot2_1);                \
    dct_wadd(sump13o, p1o, p3o);                     \
    dct_wadd(sump24o, p2o, p4o);                     \
    dct_wadd(sump23o, p2o, p3o);                     \
    dct_wadd(sump14o, p1o, p4o);                     \
    dct_long_mac(x4, sump13o, row7, rot3_0);         \
    dct_long_mac(x5, sump24o, row5, rot3_1);         \
    dct_long_mac(x6, sump23o, row3, rot3_2);         \
    dct_long_mac(x7, sump14o, row1, rot3_3);         \
    dct_bfly32o(row0, row7, x0, x7, shiftop, shift); \
    dct_bfly32o(row1, row6, x1, x6, shiftop, shift); \
    dct_bfly32o(row2, row5, x2, x5, shiftop, shift); \
    dct_bfly32o(row3, row4, x3, x4, shiftop, shift); \
  }

  // load
  row0 = vld1q_s16(data + 0 * 8);
  row1 = vld1q_s16(data + 1 * 8);
  row2 = vld1q_s16(data + 2 * 8);
  row3 = vld1q_s16(data + 3 * 8);
  row4 = vld1q_s16(data + 4 * 8);
  row5 = vld1q_s16(data + 5 * 8);
  row6 = vld1q_s16(data + 6 * 8);
  row7 = vld1q_s16(data + 7 * 8);

  // add DC bias
  row0 = vaddq_s16(row0, vsetq_lane_s16(1024, vdupq_n_s16(0), 0));

  // column pass
  dct_pass(vrshrn_n_s32, 10);

  // 16bit 8x8 transpose
  {
// these three map to a single VTRN.16, VTRN.32, and VSWP, respectively.
// whether compilers actually get this is another story, sadly.
#define dct_trn16(x, y)              \
  {                                  \
    int16x8x2_t t = vtrnq_s16(x, y); \
    x = t.val[0];                    \
    y = t.val[1];                    \
  }
#define dct_trn32(x, y)                                                \
  {                                                                    \
    int32x4x2_t t =                                                    \
        vtrnq_s32(vreinterpretq_s32_s16(x), vreinterpretq_s32_s16(y)); \
    x = vreinterpretq_s16_s32(t.val[0]);                               \
    y = vreinterpretq_s16_s32(t.val[1]);                               \
  }
#define dct_trn64(x, y)                                     \
  {                                                         \
    int16x8_t x0 = x;                                       \
    int16x8_t y0 = y;                                       \
    x = vcombine_s16(vget_low_s16(x0), vget_low_s16(y0));   \
    y = vcombine_s16(vget_high_s16(x0), vget_high_s16(y0)); \
  }

    // pass 1
    dct_trn16(row0, row1);  // a0b0a2b2a4b4a6b6
    dct_trn16(row2, row3);
    dct_trn16(row4, row5);
    dct_trn16(row6, row7);

    // pass 2
    dct_trn32(row0, row2);  // a0b0c0d0a4b4c4d4
    dct_trn32(row1, row3);
    dct_trn32(row4, row6);
    dct_trn32(row5, row7);

    // pass 3
    dct_trn64(row0, row4);  // a0b0c0d0e0f0g0h0
    dct_trn64(row1, row5);
    dct_trn64(row2, row6);
    dct_trn64(row3, row7);

#undef dct_trn16
#undef dct_trn32
#undef dct_trn64
  }

  // row pass
  // vrshrn_n_s32 only supports shifts up to 16, we need
  // 17. so do a non-rounding shift of 16 first then follow
  // up with a rounding shift by 1.
  dct_pass(vshrn_n_s32, 16);

  {
    // pack and round
    uint8x8_t p0 = vqrshrun_n_s16(row0, 1);
    uint8x8_t p1 = vqrshrun_n_s16(row1, 1);
    uint8x8_t p2 = vqrshrun_n_s16(row2, 1);
    uint8x8_t p3 = vqrshrun_n_s16(row3, 1);
    uint8x8_t p4 = vqrshrun_n_s16(row4, 1);
    uint8x8_t p5 = vqrshrun_n_s16(row5, 1);
    uint8x8_t p6 = vqrshrun_n_s16(row6, 1);
    uint8x8_t p7 = vqrshrun_n_s16(row7, 1);

    // again, these can translate into one instruction, but often don't.
#define dct_trn8_8(x, y)           \
  {                                \
    uint8x8x2_t t = vtrn_u8(x, y); \
    x = t.val[0];                  \
    y = t.val[1];                  \
  }
#define dct_trn8_16(x, y)                                                      \
  {                                                                            \
    uint16x4x2_t t = vtrn_u16(vreinterpret_u16_u8(x), vreinterpret_u16_u8(y)); \
    x = vreinterpret_u8_u16(t.val[0]);                                         \
    y = vreinterpret_u8_u16(t.val[1]);                                         \
  }
#define dct_trn8_32(x, y)                                                      \
  {                                                                            \
    uint32x2x2_t t = vtrn_u32(vreinterpret_u32_u8(x), vreinterpret_u32_u8(y)); \
    x = vreinterpret_u8_u32(t.val[0]);                                         \
    y = vreinterpret_u8_u32(t.val[1]);                                         \
  }

    // sadly can't use interleaved stores here since we only write
    // 8 bytes to each scan line!

    // 8x8 8-bit transpose pass 1
    dct_trn8_8(p0, p1);
    dct_trn8_8(p2, p3);
    dct_trn8_8(p4, p5);
    dct_trn8_8(p6, p7);

    // pass 2
    dct_trn8_16(p0, p2);
    dct_trn8_16(p1, p3);
    dct_trn8_16(p4, p6);
    dct_trn8_16(p5, p7);

    // pass 3
    dct_trn8_32(p0, p4);
    dct_trn8_32(p1, p5);
    dct_trn8_32(p2, p6);
    dct_trn8_32(p3, p7);

    // store
    vst1_u8(out, p0);
    out += out_stride;
    vst1_u8(out, p1);
    out += out_stride;
    vst1_u8(out, p2);
    out += out_stride;
    vst1_u8(out, p3);
    out += out_stride;
    vst1_u8(out, p4);
    out += out_stride;
    vst1_u8(out, p5);
    out += out_stride;
    vst1_u8(out, p6);
    out += out_stride;
    vst1_u8(out, p7);

#undef dct_trn8_8
#undef dct_trn8_16
#undef dct_trn8_32
  }

#undef dct_long_mul
#undef dct_long_mac
#undef dct_widen
#undef dct_wadd
#undef dct_wsub
#undef dct_bfly32o
#undef dct_pass
}

#endif  // STBI_NEON

#define STBI__MARKER_none 0xff
// if there's a pending marker from the entropy stream, return that
// otherwise, fetch from the stream and get a marker. if there's no
// marker, return 0xff, which is never a valid marker value
static unsigned char stbi__get_marker(stbi__jpeg *j) {
  unsigned char x;
  if (j->marker != STBI__MARKER_none) {
    x = j->marker;
    j->marker = STBI__MARKER_none;
    return x;
  }
  x = stbi__get8(j->s);
  if (x != 0xff) return STBI__MARKER_none;
  while (x == 0xff) x = stbi__get8(j->s);  // consume repeated 0xff fill bytes
  return x;
}

// in each scan, we'll have scan_n components, and the order
// of the components is specified by order[]
#define STBI__RESTART(x) ((x) >= 0xd0 && (x) <= 0xd7)

// after a restart interval, stbi__jpeg_reset the entropy decoder and
// the dc prediction
static void stbi__jpeg_reset(stbi__jpeg *j) {
  j->code_bits = 0;
  j->code_buffer = 0;
  j->nomore = 0;
  j->img_comp[0].dc_pred = j->img_comp[1].dc_pred = j->img_comp[2].dc_pred =
      j->img_comp[3].dc_pred = 0;
  j->marker = STBI__MARKER_none;
  j->todo = j->restart_interval ? j->restart_interval : 0x7fffffff;
  j->eob_run = 0;
  // no more than 1<<31 MCUs if no restart_interal? that's plenty safe,
  // since we don't even allow 1<<30 pixels
}

static int stbi__parse_entropy_coded_data(stbi__jpeg *z) {
  stbi__jpeg_reset(z);
  if (!z->progressive) {
    if (z->scan_n == 1) {
      int i, j;
      short data[64] forcealign(16);
      int n = z->order[0];
      // non-interleaved data, we just need to process one block at a time,
      // in trivial scanline order
      // number of blocks to do just depends on how many actual "pixels" this
      // component has, independent of interleaved MCU blocking and such
      int w = (z->img_comp[n].x + 7) >> 3;
      int h = (z->img_comp[n].y + 7) >> 3;
      for (j = 0; j < h; ++j) {
        for (i = 0; i < w; ++i) {
          int ha = z->img_comp[n].ha;
          if (!stbi__jpeg_decode_block(z, data, z->huff_dc + z->img_comp[n].hd,
                                       z->huff_ac + ha, z->fast_ac[ha], n,
                                       z->dequant[z->img_comp[n].tq]))
            return 0;
          idct_block_kernel(
              z->img_comp[n].data + z->img_comp[n].w2 * j * 8 + i * 8,
              z->img_comp[n].w2, data);
          // every data block is an MCU, so countdown the restart interval
          if (--z->todo <= 0) {
            if (z->code_bits < 24) stbi__grow_buffer_unsafe(z);
            // if it's NOT a restart, then just bail, so we get corrupt data
            // rather than no data
            if (!STBI__RESTART(z->marker)) return 1;
            stbi__jpeg_reset(z);
          }
        }
      }
      return 1;
    } else {  // interleaved
      int i, j, k, x, y;
      short data[64] forcealign(16);
      for (j = 0; j < z->img_mcu_y; ++j) {
        for (i = 0; i < z->img_mcu_x; ++i) {
          // scan an interleaved mcu... process scan_n components in order
          for (k = 0; k < z->scan_n; ++k) {
            int n = z->order[k];
            // scan out an mcu's worth of this component; that's just determined
            // by the basic H and V specified for the component
            for (y = 0; y < z->img_comp[n].v; ++y) {
              for (x = 0; x < z->img_comp[n].h; ++x) {
                int x2 = (i * z->img_comp[n].h + x) * 8;
                int y2 = (j * z->img_comp[n].v + y) * 8;
                int ha = z->img_comp[n].ha;
                if (!stbi__jpeg_decode_block(z, data,
                                             z->huff_dc + z->img_comp[n].hd,
                                             z->huff_ac + ha, z->fast_ac[ha], n,
                                             z->dequant[z->img_comp[n].tq]))
                  return 0;
                idct_block_kernel(
                    z->img_comp[n].data + z->img_comp[n].w2 * y2 + x2,
                    z->img_comp[n].w2, data);
              }
            }
          }
          // after all interleaved components, that's an interleaved MCU,
          // so now count down the restart interval
          if (--z->todo <= 0) {
            if (z->code_bits < 24) stbi__grow_buffer_unsafe(z);
            if (!STBI__RESTART(z->marker)) return 1;
            stbi__jpeg_reset(z);
          }
        }
      }
      return 1;
    }
  } else {
    if (z->scan_n == 1) {
      int i, j;
      int n = z->order[0];
      // non-interleaved data, we just need to process one block at a time,
      // in trivial scanline order
      // number of blocks to do just depends on how many actual "pixels" this
      // component has, independent of interleaved MCU blocking and such
      int w = (z->img_comp[n].x + 7) >> 3;
      int h = (z->img_comp[n].y + 7) >> 3;
      for (j = 0; j < h; ++j) {
        for (i = 0; i < w; ++i) {
          short *data =
              z->img_comp[n].coeff + 64 * (i + j * z->img_comp[n].coeff_w);
          if (z->spec_start == 0) {
            if (!stbi__jpeg_decode_block_prog_dc(
                    z, data, &z->huff_dc[z->img_comp[n].hd], n))
              return 0;
          } else {
            int ha = z->img_comp[n].ha;
            if (!stbi__jpeg_decode_block_prog_ac(z, data, &z->huff_ac[ha],
                                                 z->fast_ac[ha]))
              return 0;
          }
          // every data block is an MCU, so countdown the restart interval
          if (--z->todo <= 0) {
            if (z->code_bits < 24) stbi__grow_buffer_unsafe(z);
            if (!STBI__RESTART(z->marker)) return 1;
            stbi__jpeg_reset(z);
          }
        }
      }
      return 1;
    } else {  // interleaved
      int i, j, k, x, y;
      for (j = 0; j < z->img_mcu_y; ++j) {
        for (i = 0; i < z->img_mcu_x; ++i) {
          // scan an interleaved mcu... process scan_n components in order
          for (k = 0; k < z->scan_n; ++k) {
            int n = z->order[k];
            // scan out an mcu's worth of this component; that's just determined
            // by the basic H and V specified for the component
            for (y = 0; y < z->img_comp[n].v; ++y) {
              for (x = 0; x < z->img_comp[n].h; ++x) {
                int x2 = (i * z->img_comp[n].h + x);
                int y2 = (j * z->img_comp[n].v + y);
                short *data = z->img_comp[n].coeff +
                              64 * (x2 + y2 * z->img_comp[n].coeff_w);
                if (!stbi__jpeg_decode_block_prog_dc(
                        z, data, &z->huff_dc[z->img_comp[n].hd], n))
                  return 0;
              }
            }
          }
          // after all interleaved components, that's an interleaved MCU,
          // so now count down the restart interval
          if (--z->todo <= 0) {
            if (z->code_bits < 24) stbi__grow_buffer_unsafe(z);
            if (!STBI__RESTART(z->marker)) return 1;
            stbi__jpeg_reset(z);
          }
        }
      }
      return 1;
    }
  }
}

static void stbi__jpeg_dequantize(short *data, uint16_t *dequant) {
  int i;
  for (i = 0; i < 64; ++i) data[i] *= dequant[i];
}

static void stbi__jpeg_finish(stbi__jpeg *z) {
  if (z->progressive) {
    // dequantize and idct the data
    int i, j, n;
    for (n = 0; n < z->s->img_n; ++n) {
      int w = (z->img_comp[n].x + 7) >> 3;
      int h = (z->img_comp[n].y + 7) >> 3;
      for (j = 0; j < h; ++j) {
        for (i = 0; i < w; ++i) {
          short *data =
              z->img_comp[n].coeff + 64 * (i + j * z->img_comp[n].coeff_w);
          stbi__jpeg_dequantize(data, z->dequant[z->img_comp[n].tq]);
          idct_block_kernel(
              z->img_comp[n].data + z->img_comp[n].w2 * j * 8 + i * 8,
              z->img_comp[n].w2, data);
        }
      }
    }
  }
}

static int stbi__process_marker(stbi__jpeg *z, int m) {
  int L;
  switch (m) {
    case STBI__MARKER_none:  // no marker found
      return stbi__err("expected marker", "Corrupt JPEG");

    case 0xDD:  // DRI - specify restart interval
      if (stbi__get16be(z->s) != 4)
        return stbi__err("bad DRI len", "Corrupt JPEG");
      z->restart_interval = stbi__get16be(z->s);
      return 1;

    case 0xDB:  // DQT - define quantization table
      L = stbi__get16be(z->s) - 2;
      while (L > 0) {
        int q = stbi__get8(z->s);
        int p = q >> 4, sixteen = (p != 0);
        int t = q & 15, i;
        if (p != 0 && p != 1) return stbi__err("bad DQT type", "Corrupt JPEG");
        if (t > 3) return stbi__err("bad DQT table", "Corrupt JPEG");

        for (i = 0; i < 64; ++i)
          z->dequant[t][stbi__jpeg_dezigzag[i]] =
              (uint16_t)(sixteen ? stbi__get16be(z->s) : stbi__get8(z->s));
        L -= (sixteen ? 129 : 65);
      }
      return L == 0;

    case 0xC4:  // DHT - define huffman table
      L = stbi__get16be(z->s) - 2;
      while (L > 0) {
        unsigned char *v;
        int sizes[16], i, n = 0;
        int q = stbi__get8(z->s);
        int tc = q >> 4;
        int th = q & 15;
        if (tc > 1 || th > 3)
          return stbi__err("bad DHT header", "Corrupt JPEG");
        for (i = 0; i < 16; ++i) {
          sizes[i] = stbi__get8(z->s);
          n += sizes[i];
        }
        if (n > 256) {
          // Loop over i < n would write past end of values!
          return stbi__err("bad DHT header", "Corrupt JPEG");
        }
        L -= 17;
        if (tc == 0) {
          if (!stbi__build_huffman(z->huff_dc + th, sizes)) return 0;
          v = z->huff_dc[th].values;
        } else {
          if (!stbi__build_huffman(z->huff_ac + th, sizes)) return 0;
          v = z->huff_ac[th].values;
        }
        for (i = 0; i < n; ++i) v[i] = stbi__get8(z->s);
        if (tc != 0) stbi__build_fast_ac(z->fast_ac[th], z->huff_ac + th);
        L -= n;
      }
      return L == 0;
  }

  // check for comment block or APP blocks
  if ((m >= 0xE0 && m <= 0xEF) || m == 0xFE) {
    L = stbi__get16be(z->s);
    if (L < 2) {
      if (m == 0xFE)
        return stbi__err("bad COM len", "Corrupt JPEG");
      else
        return stbi__err("bad APP len", "Corrupt JPEG");
    }
    L -= 2;

    if (m == 0xE0 && L >= 5) {  // JFIF APP0 segment
      static const unsigned char tag[5] = {'J', 'F', 'I', 'F', '\0'};
      int ok = 1;
      int i;
      for (i = 0; i < 5; ++i)
        if (stbi__get8(z->s) != tag[i]) ok = 0;
      L -= 5;
      if (ok) z->jfif = m;
    } else if (m == 0xEE && L >= 12) {  // Adobe APP14 segment
      static const unsigned char tag[6] = {'A', 'd', 'o', 'b', 'e', '\0'};
      int ok = 1;
      int i;
      for (i = 0; i < 6; ++i)
        if (stbi__get8(z->s) != tag[i]) ok = 0;
      L -= 6;
      if (ok) {
        stbi__get8(z->s);                             // version
        stbi__get16be(z->s);                          // flags0
        stbi__get16be(z->s);                          // flags1
        z->app14_color_transform = stbi__get8(z->s);  // color transform
        L -= 6;
      }
    }

    stbi__skip(z->s, L);
    return 1;
  }

  return stbi__err("unknown marker", "Corrupt JPEG");
}

// after we see SOS
static int stbi__process_scan_header(stbi__jpeg *z) {
  int i;
  int Ls = stbi__get16be(z->s);
  z->scan_n = stbi__get8(z->s);
  if (z->scan_n < 1 || z->scan_n > 4 || z->scan_n > (int)z->s->img_n)
    return stbi__err("bad SOS component count", "Corrupt JPEG");
  if (Ls != 6 + 2 * z->scan_n) return stbi__err("bad SOS len", "Corrupt JPEG");
  for (i = 0; i < z->scan_n; ++i) {
    int id = stbi__get8(z->s), which;
    int q = stbi__get8(z->s);
    for (which = 0; which < z->s->img_n; ++which)
      if (z->img_comp[which].id == id) break;
    if (which == z->s->img_n) return 0;  // no match
    z->img_comp[which].hd = q >> 4;
    if (z->img_comp[which].hd > 3)
      return stbi__err("bad DC huff", "Corrupt JPEG");
    z->img_comp[which].ha = q & 15;
    if (z->img_comp[which].ha > 3)
      return stbi__err("bad AC huff", "Corrupt JPEG");
    z->order[i] = which;
  }

  {
    int aa;
    z->spec_start = stbi__get8(z->s);
    z->spec_end = stbi__get8(z->s);  // should be 63, but might be 0
    aa = stbi__get8(z->s);
    z->succ_high = (aa >> 4);
    z->succ_low = (aa & 15);
    if (z->progressive) {
      if (z->spec_start > 63 || z->spec_end > 63 ||
          z->spec_start > z->spec_end || z->succ_high > 13 || z->succ_low > 13)
        return stbi__err("bad SOS", "Corrupt JPEG");
    } else {
      if (z->spec_start != 0) return stbi__err("bad SOS", "Corrupt JPEG");
      if (z->succ_high != 0 || z->succ_low != 0)
        return stbi__err("bad SOS", "Corrupt JPEG");
      z->spec_end = 63;
    }
  }

  return 1;
}

static int stbi__free_jpeg_components(stbi__jpeg *z, int ncomp, int why) {
  int i;
  for (i = 0; i < ncomp; ++i) {
    if (z->img_comp[i].data) {
      free(z->img_comp[i].data);
      z->img_comp[i].data = NULL;
    }
    if (z->img_comp[i].coeff) {
      free(z->img_comp[i].coeff);
      z->img_comp[i].coeff = NULL;
    }
    if (z->img_comp[i].linebuf) {
      free(z->img_comp[i].linebuf);
      z->img_comp[i].linebuf = NULL;
    }
  }
  return why;
}

static int stbi__process_frame_header(stbi__jpeg *z, int scan) {
  stbi__context *s = z->s;
  int Lf, p, i, q, h_max = 1, v_max = 1, c;
  Lf = stbi__get16be(s);
  if (Lf < 11) {
    // JPEG
    return stbi__err("bad SOF len", "Corrupt JPEG");
  }
  p = stbi__get8(s);
  if (p != 8) {
    // JPEG baseline
    return stbi__err("only 8-bit", "JPEG format not supported: 8-bit only");
  }
  s->img_y = stbi__get16be(s);
  if (s->img_y == 0) {
    // Legal, but we don't handle it--but neither does IJG
    return stbi__err("no header height",
                     "JPEG format not supported: delayed height");
  }
  s->img_x = stbi__get16be(s);
  if (s->img_x == 0) {
    // JPEG requires
    return stbi__err("0 width", "Corrupt JPEG");
  }
  if (s->img_y > STBI_MAX_DIMENSIONS) {
    return stbi__err("too large", "Very large image (corrupt?)");
  }
  if (s->img_x > STBI_MAX_DIMENSIONS) {
    return stbi__err("too large", "Very large image (corrupt?)");
  }
  c = stbi__get8(s);
  if (c != 3 && c != 1 && c != 4) {
    return stbi__err("bad component count", "Corrupt JPEG");
  }
  s->img_n = c;
  for (i = 0; i < c; ++i) {
    z->img_comp[i].data = NULL;
    z->img_comp[i].linebuf = NULL;
  }

  if (Lf != 8 + 3 * s->img_n) return stbi__err("bad SOF len", "Corrupt JPEG");

  z->rgb = 0;
  for (i = 0; i < s->img_n; ++i) {
    static const unsigned char rgb[3] = {'R', 'G', 'B'};
    z->img_comp[i].id = stbi__get8(s);
    if (s->img_n == 3 && z->img_comp[i].id == rgb[i]) ++z->rgb;
    q = stbi__get8(s);
    z->img_comp[i].h = (q >> 4);
    if (!z->img_comp[i].h || z->img_comp[i].h > 4)
      return stbi__err("bad H", "Corrupt JPEG");
    z->img_comp[i].v = q & 15;
    if (!z->img_comp[i].v || z->img_comp[i].v > 4)
      return stbi__err("bad V", "Corrupt JPEG");
    z->img_comp[i].tq = stbi__get8(s);
    if (z->img_comp[i].tq > 3) return stbi__err("bad TQ", "Corrupt JPEG");
  }

  if (scan != STBI__SCAN_load) return 1;

  if (!stbi__mad3sizes_valid(s->img_x, s->img_y, s->img_n, 0))
    return stbi__err("too large", "Image too large to decode");

  for (i = 0; i < s->img_n; ++i) {
    if (z->img_comp[i].h > h_max) h_max = z->img_comp[i].h;
    if (z->img_comp[i].v > v_max) v_max = z->img_comp[i].v;
  }

  // check that plane subsampling factors are integer ratios;
  // our resamplers can't deal with fractional ratios
  // and I've never seen a non-corrupted JPEG file actually use them
  for (i = 0; i < s->img_n; ++i) {
    if (h_max % z->img_comp[i].h != 0) {
      return stbi__err("bad H", "Corrupt JPEG");
    }
    if (v_max % z->img_comp[i].v != 0) {
      return stbi__err("bad V", "Corrupt JPEG");
    }
  }

  // compute interleaved mcu info
  z->img_h_max = h_max;
  z->img_v_max = v_max;
  z->img_mcu_w = h_max * 8;
  z->img_mcu_h = v_max * 8;
  // these sizes can't be more than 17 bits
  z->img_mcu_x = (s->img_x + z->img_mcu_w - 1) / z->img_mcu_w;
  z->img_mcu_y = (s->img_y + z->img_mcu_h - 1) / z->img_mcu_h;

  for (i = 0; i < s->img_n; ++i) {
    // number of effective pixels (e.g. for non-interleaved MCU)
    z->img_comp[i].x = (s->img_x * z->img_comp[i].h + h_max - 1) / h_max;
    z->img_comp[i].y = (s->img_y * z->img_comp[i].v + v_max - 1) / v_max;
    // to simplify generation, we'll allocate enough memory to decode
    // the bogus oversized data from using interleaved MCUs and their
    // big blocks (e.g. a 16x16 iMCU on an image of width 33); we won't
    // discard the extra data until colorspace conversion
    //
    // img_mcu_x, img_mcu_y: <=17 bits; comp[i].h and .v are <=4 (checked
    // earlier) so these muls can't overflow with 32-bit ints (which we require)
    z->img_comp[i].w2 = z->img_mcu_x * z->img_comp[i].h * 8;
    z->img_comp[i].h2 = z->img_mcu_y * z->img_comp[i].v * 8;
    z->img_comp[i].coeff = NULL;
    z->img_comp[i].linebuf = NULL;
    z->img_comp[i].data =
        stbi__malloc_mad2(z->img_comp[i].w2, z->img_comp[i].h2, 15);
    if (z->progressive) {
      // w2, h2 are multiples of 8 (see above)
      z->img_comp[i].coeff_w = z->img_comp[i].w2 / 8;
      z->img_comp[i].coeff_h = z->img_comp[i].h2 / 8;
      z->img_comp[i].coeff = stbi__malloc_mad3(
          z->img_comp[i].w2, z->img_comp[i].h2, sizeof(short), 15);
    }
  }

  return 1;
}

// use comparisons since in some cases we handle more than one case (e.g. SOF)
#define stbi__DNL(x) ((x) == 0xdc)
#define stbi__SOI(x) ((x) == 0xd8)
#define stbi__EOI(x) ((x) == 0xd9)
#define stbi__SOF(x) ((x) == 0xc0 || (x) == 0xc1 || (x) == 0xc2)
#define stbi__SOS(x) ((x) == 0xda)

#define stbi__SOF_progressive(x) ((x) == 0xc2)

static int stbi__decode_jpeg_header(stbi__jpeg *z, int scan) {
  int m;
  z->jfif = 0;
  z->app14_color_transform = -1;  // valid values are 0,1,2
  z->marker = STBI__MARKER_none;  // initialize cached marker to empty
  m = stbi__get_marker(z);
  if (!stbi__SOI(m)) return 0;
  if (scan == STBI__SCAN_type) return 1;
  m = stbi__get_marker(z);
  while (!stbi__SOF(m)) {
    if (!stbi__process_marker(z, m)) return 0;
    m = stbi__get_marker(z);
    while (m == STBI__MARKER_none) {
      // some files have extra padding after their blocks, so ok, we'll scan
      if (stbi__at_eof(z->s)) return stbi__err("no SOF", "Corrupt JPEG");
      m = stbi__get_marker(z);
    }
  }
  z->progressive = stbi__SOF_progressive(m);
  if (!stbi__process_frame_header(z, scan)) return 0;
  return 1;
}

static unsigned char stbi__skip_jpeg_junk_at_end(stbi__jpeg *j) {
  // some JPEGs have junk at end, skip over it but if we find what looks
  // like a valid marker, resume there
  while (!stbi__at_eof(j->s)) {
    unsigned char x = stbi__get8(j->s);
    while (x == 0xff) {  // might be a marker
      if (stbi__at_eof(j->s)) return STBI__MARKER_none;
      x = stbi__get8(j->s);
      if (x != 0x00 && x != 0xff) {
        // not a stuffed zero or lead-in to another marker, looks
        // like an actual marker, return it
        return x;
      }
      // stuffed zero has x=0 now which ends the loop, meaning we go
      // back to regular scan loop.
      // repeated 0xff keeps trying to read the next byte of the marker.
    }
  }
  return STBI__MARKER_none;
}

// decode image to YCbCr format
static int stbi__decode_jpeg_image(stbi__jpeg *j) {
  int m;
  for (m = 0; m < 4; m++) {
    j->img_comp[m].data = NULL;
    j->img_comp[m].coeff = NULL;
  }
  j->restart_interval = 0;
  if (!stbi__decode_jpeg_header(j, STBI__SCAN_load)) return 0;
  m = stbi__get_marker(j);
  while (!stbi__EOI(m)) {
    if (stbi__SOS(m)) {
      if (!stbi__process_scan_header(j)) return 0;
      if (!stbi__parse_entropy_coded_data(j)) return 0;
      if (j->marker == STBI__MARKER_none) {
        j->marker = stbi__skip_jpeg_junk_at_end(j);
        // if we reach eof without hitting a marker, stbi__get_marker() below
        // will fail and we'll eventually return 0
      }
      m = stbi__get_marker(j);
      if (STBI__RESTART(m)) m = stbi__get_marker(j);
    } else if (stbi__DNL(m)) {
      int Ld = stbi__get16be(j->s);
      uint32_t NL = stbi__get16be(j->s);
      if (Ld != 4) return stbi__err("bad DNL len", "Corrupt JPEG");
      if (NL != j->s->img_y) return stbi__err("bad DNL height", "Corrupt JPEG");
      m = stbi__get_marker(j);
    } else {
      if (!stbi__process_marker(j, m)) return 1;
      m = stbi__get_marker(j);
    }
  }
  if (j->progressive) stbi__jpeg_finish(j);
  return 1;
}

// static jfif-centered resampling (across block boundaries)

typedef unsigned char *(*resample_row_func)(unsigned char *out,
                                            unsigned char *in0,
                                            unsigned char *in1, int w, int hs);

#define stbi__div4(x) ((unsigned char)((x) >> 2))

static unsigned char *resample_row_1(unsigned char *out, unsigned char *in_near,
                                     unsigned char *in_far, int w, int hs) {
  return in_near;
}

static unsigned char *stbi__resample_row_v_2(unsigned char *out,
                                             unsigned char *in_near,
                                             unsigned char *in_far, int w,
                                             int hs) {
  // need to generate two samples vertically for every one in input
  int i;
  for (i = 0; i < w; ++i) out[i] = stbi__div4(3 * in_near[i] + in_far[i] + 2);
  return out;
}

static unsigned char *stbi__resample_row_h_2(unsigned char *out,
                                             unsigned char *in_near,
                                             unsigned char *in_far, int w,
                                             int hs) {
  // need to generate two samples horizontally for every one in input
  int i;
  unsigned char *input = in_near;
  if (w == 1) {
    // if only one sample, can't do any interpolation
    out[0] = out[1] = input[0];
    return out;
  }
  out[0] = input[0];
  out[1] = stbi__div4(input[0] * 3 + input[1] + 2);
  for (i = 1; i < w - 1; ++i) {
    int n = 3 * input[i] + 2;
    out[i * 2 + 0] = stbi__div4(n + input[i - 1]);
    out[i * 2 + 1] = stbi__div4(n + input[i + 1]);
  }
  out[i * 2 + 0] = stbi__div4(input[w - 2] * 3 + input[w - 1] + 2);
  out[i * 2 + 1] = input[w - 1];
  return out;
}

#define stbi__div16(x) ((unsigned char)((x) >> 4))

static unsigned char *stbi__resample_row_hv_2(unsigned char *out,
                                              unsigned char *in_near,
                                              unsigned char *in_far, int w,
                                              int hs) {
  // need to generate 2x2 samples for every one in input
  int i, t0, t1;
  if (w == 1) {
    out[0] = out[1] = stbi__div4(3 * in_near[0] + in_far[0] + 2);
    return out;
  }
  t1 = 3 * in_near[0] + in_far[0];
  out[0] = stbi__div4(t1 + 2);
  for (i = 1; i < w; ++i) {
    t0 = t1;
    t1 = 3 * in_near[i] + in_far[i];
    out[i * 2 - 1] = stbi__div16(3 * t0 + t1 + 8);
    out[i * 2] = stbi__div16(3 * t1 + t0 + 8);
  }
  out[w * 2 - 1] = stbi__div4(t1 + 2);
  return out;
}

#if defined(STBI_SSE2)
static unsigned char *stbi__resample_row_hv_2_simd(unsigned char *out,
                                                   unsigned char *in_near,
                                                   unsigned char *in_far, int w,
                                                   int hs) {
  // need to generate 2x2 samples for every one in input
  int i = 0, t0, t1;
  if (w == 1) {
    out[0] = out[1] = stbi__div4(3 * in_near[0] + in_far[0] + 2);
    return out;
  }
  t1 = 3 * in_near[0] + in_far[0];
  // process groups of 8 pixels for as long as we can.
  // note we can't handle the last pixel in a row in this loop
  // because we need to handle the filter boundary conditions.
  for (; i < ((w - 1) & ~7); i += 8) {
    // load and perform the vertical filtering pass
    // this uses 3*x + y = 4*x + (y - x)
    __m128i zero = _mm_setzero_si128();
    __m128i farb = _mm_loadl_epi64((__m128i *)(in_far + i));
    __m128i nearb = _mm_loadl_epi64((__m128i *)(in_near + i));
    __m128i farw = _mm_unpacklo_epi8(farb, zero);
    __m128i nearw = _mm_unpacklo_epi8(nearb, zero);
    __m128i diff = _mm_sub_epi16(farw, nearw);
    __m128i nears = _mm_slli_epi16(nearw, 2);
    __m128i curr = _mm_add_epi16(nears, diff);  // current row
    // horizontal filter works the same based on shifted vers of current
    // row. "prev" is current row shifted right by 1 pixel; we need to
    // insert the previous pixel value (from t1).
    // "next" is current row shifted left by 1 pixel, with first pixel
    // of next block of 8 pixels added in.
    __m128i prv0 = _mm_slli_si128(curr, 2);
    __m128i nxt0 = _mm_srli_si128(curr, 2);
    __m128i prev = _mm_insert_epi16(prv0, t1, 0);
    __m128i next =
        _mm_insert_epi16(nxt0, 3 * in_near[i + 8] + in_far[i + 8], 7);
    // horizontal filter, polyphase implementation since it's convenient:
    // even pixels = 3*cur + prev = cur*4 + (prev - cur)
    // odd  pixels = 3*cur + next = cur*4 + (next - cur)
    // note the shared term.
    __m128i bias = _mm_set1_epi16(8);
    __m128i curs = _mm_slli_epi16(curr, 2);
    __m128i prvd = _mm_sub_epi16(prev, curr);
    __m128i nxtd = _mm_sub_epi16(next, curr);
    __m128i curb = _mm_add_epi16(curs, bias);
    __m128i even = _mm_add_epi16(prvd, curb);
    __m128i odd = _mm_add_epi16(nxtd, curb);
    // interleave even and odd pixels, then undo scaling.
    __m128i int0 = _mm_unpacklo_epi16(even, odd);
    __m128i int1 = _mm_unpackhi_epi16(even, odd);
    __m128i de0 = _mm_srli_epi16(int0, 4);
    __m128i de1 = _mm_srli_epi16(int1, 4);
    // pack and write output
    __m128i outv = _mm_packus_epi16(de0, de1);
    _mm_storeu_si128((__m128i *)(out + i * 2), outv);
    // "previous" value for next iter
    t1 = 3 * in_near[i + 7] + in_far[i + 7];
  }
  t0 = t1;
  t1 = 3 * in_near[i] + in_far[i];
  out[i * 2] = stbi__div16(3 * t1 + t0 + 8);
  for (++i; i < w; ++i) {
    t0 = t1;
    t1 = 3 * in_near[i] + in_far[i];
    out[i * 2 - 1] = stbi__div16(3 * t0 + t1 + 8);
    out[i * 2] = stbi__div16(3 * t1 + t0 + 8);
  }
  out[w * 2 - 1] = stbi__div4(t1 + 2);
  return out;
}
#endif

static unsigned char *stbi__resample_row_nearest(unsigned char *out,
                                                 unsigned char *in_near,
                                                 unsigned char *in_far, int w,
                                                 int hs) {
  int i, j;
  for (i = 0; i < w; ++i) {
    for (j = 0; j < hs; ++j) {
      out[i * hs + j] = in_near[i];
    }
  }
  return out;
}

// this is a reduced-precision calculation of YCbCr-to-RGB introduced
// to make sure the code produces the same results in both SIMD and scalar
#define stbi__float2fixed(x) (((int)((x)*4096.0f + 0.5f)) << 8)
static void stbi__YCbCr_to_RGB_row(unsigned char *out, const unsigned char *y,
                                   const unsigned char *pcb,
                                   const unsigned char *pcr, int count,
                                   int step) {
  int i;
  for (i = 0; i < count; ++i) {
    int y_fixed = (y[i] << 20) + (1 << 19);  // rounding
    int r, g, b;
    int cr = pcr[i] - 128;
    int cb = pcb[i] - 128;
    r = y_fixed + cr * stbi__float2fixed(1.40200f);
    g = y_fixed + (cr * -stbi__float2fixed(0.71414f)) +
        ((cb * -stbi__float2fixed(0.34414f)) & 0xffff0000);
    b = y_fixed + cb * stbi__float2fixed(1.77200f);
    r >>= 20;
    g >>= 20;
    b >>= 20;
    if ((unsigned)r > 255) {
      if (r < 0)
        r = 0;
      else
        r = 255;
    }
    if ((unsigned)g > 255) {
      if (g < 0)
        g = 0;
      else
        g = 255;
    }
    if ((unsigned)b > 255) {
      if (b < 0)
        b = 0;
      else
        b = 255;
    }
    out[0] = (unsigned char)r;
    out[1] = (unsigned char)g;
    out[2] = (unsigned char)b;
    out[3] = 255;
    out += step;
  }
}

#if defined(STBI_SSE2) || defined(STBI_NEON)
static void stbi__YCbCr_to_RGB_simd(unsigned char *out, unsigned char const *y,
                                    unsigned char const *pcb,
                                    unsigned char const *pcr, int count,
                                    int step) {
  int i = 0;

#ifdef STBI_SSE2
  // step == 3 is pretty ugly on the final interleave, and i'm not convinced
  // it's useful in practice (you wouldn't use it for textures, for example).
  // so just accelerate step == 4 case.
  if (step == 4) {
    // this is a fairly straightforward implementation and not super-optimized.
    __m128i signflip = _mm_set1_epi8(-0x80);
    __m128i cr_const0 = _mm_set1_epi16((short)(1.40200f * 4096.0f + 0.5f));
    __m128i cr_const1 = _mm_set1_epi16(-(short)(0.71414f * 4096.0f + 0.5f));
    __m128i cb_const0 = _mm_set1_epi16(-(short)(0.34414f * 4096.0f + 0.5f));
    __m128i cb_const1 = _mm_set1_epi16((short)(1.77200f * 4096.0f + 0.5f));
    __m128i y_bias = _mm_set1_epi8((char)(unsigned char)128);
    __m128i xw = _mm_set1_epi16(255);  // alpha channel

    for (; i + 7 < count; i += 8) {
      // load
      __m128i y_bytes = _mm_loadl_epi64((__m128i *)(y + i));
      __m128i cr_bytes = _mm_loadl_epi64((__m128i *)(pcr + i));
      __m128i cb_bytes = _mm_loadl_epi64((__m128i *)(pcb + i));
      __m128i cr_biased = _mm_xor_si128(cr_bytes, signflip);  // -128
      __m128i cb_biased = _mm_xor_si128(cb_bytes, signflip);  // -128

      // unpack to short (and left-shift cr, cb by 8)
      __m128i yw = _mm_unpacklo_epi8(y_bias, y_bytes);
      __m128i crw = _mm_unpacklo_epi8(_mm_setzero_si128(), cr_biased);
      __m128i cbw = _mm_unpacklo_epi8(_mm_setzero_si128(), cb_biased);

      // color transform
      __m128i yws = _mm_srli_epi16(yw, 4);
      __m128i cr0 = _mm_mulhi_epi16(cr_const0, crw);
      __m128i cb0 = _mm_mulhi_epi16(cb_const0, cbw);
      __m128i cb1 = _mm_mulhi_epi16(cbw, cb_const1);
      __m128i cr1 = _mm_mulhi_epi16(crw, cr_const1);
      __m128i rws = _mm_add_epi16(cr0, yws);
      __m128i gwt = _mm_add_epi16(cb0, yws);
      __m128i bws = _mm_add_epi16(yws, cb1);
      __m128i gws = _mm_add_epi16(gwt, cr1);

      // descale
      __m128i rw = _mm_srai_epi16(rws, 4);
      __m128i bw = _mm_srai_epi16(bws, 4);
      __m128i gw = _mm_srai_epi16(gws, 4);

      // back to byte, set up for transpose
      __m128i brb = _mm_packus_epi16(rw, bw);
      __m128i gxb = _mm_packus_epi16(gw, xw);

      // transpose to interleave channels
      __m128i t0 = _mm_unpacklo_epi8(brb, gxb);
      __m128i t1 = _mm_unpackhi_epi8(brb, gxb);
      __m128i o0 = _mm_unpacklo_epi16(t0, t1);
      __m128i o1 = _mm_unpackhi_epi16(t0, t1);

      // store
      _mm_storeu_si128((__m128i *)(out + 0), o0);
      _mm_storeu_si128((__m128i *)(out + 16), o1);
      out += 32;
    }
  }
#endif

#ifdef STBI_NEON
  // in this version, step=3 support would be easy to add. but is there demand?
  if (step == 4) {
    // this is a fairly straightforward implementation and not super-optimized.
    uint8x8_t signflip = vdup_n_u8(0x80);
    int16x8_t cr_const0 = vdupq_n_s16((short)(1.40200f * 4096.0f + 0.5f));
    int16x8_t cr_const1 = vdupq_n_s16(-(short)(0.71414f * 4096.0f + 0.5f));
    int16x8_t cb_const0 = vdupq_n_s16(-(short)(0.34414f * 4096.0f + 0.5f));
    int16x8_t cb_const1 = vdupq_n_s16((short)(1.77200f * 4096.0f + 0.5f));

    for (; i + 7 < count; i += 8) {
      // load
      uint8x8_t y_bytes = vld1_u8(y + i);
      uint8x8_t cr_bytes = vld1_u8(pcr + i);
      uint8x8_t cb_bytes = vld1_u8(pcb + i);
      int8x8_t cr_biased = vreinterpret_s8_u8(vsub_u8(cr_bytes, signflip));
      int8x8_t cb_biased = vreinterpret_s8_u8(vsub_u8(cb_bytes, signflip));

      // expand to s16
      int16x8_t yws = vreinterpretq_s16_u16(vshll_n_u8(y_bytes, 4));
      int16x8_t crw = vshll_n_s8(cr_biased, 7);
      int16x8_t cbw = vshll_n_s8(cb_biased, 7);

      // color transform
      int16x8_t cr0 = vqdmulhq_s16(crw, cr_const0);
      int16x8_t cb0 = vqdmulhq_s16(cbw, cb_const0);
      int16x8_t cr1 = vqdmulhq_s16(crw, cr_const1);
      int16x8_t cb1 = vqdmulhq_s16(cbw, cb_const1);
      int16x8_t rws = vaddq_s16(yws, cr0);
      int16x8_t gws = vaddq_s16(vaddq_s16(yws, cb0), cr1);
      int16x8_t bws = vaddq_s16(yws, cb1);

      // undo scaling, round, convert to byte
      uint8x8x4_t o;
      o.val[0] = vqrshrun_n_s16(rws, 4);
      o.val[1] = vqrshrun_n_s16(gws, 4);
      o.val[2] = vqrshrun_n_s16(bws, 4);
      o.val[3] = vdup_n_u8(255);

      // store, interleaving r/g/b/a
      vst4_u8(out, o);
      out += 8 * 4;
    }
  }
#endif

  for (; i < count; ++i) {
    int y_fixed = (y[i] << 20) + (1 << 19);  // rounding
    int r, g, b;
    int cr = pcr[i] - 128;
    int cb = pcb[i] - 128;
    r = y_fixed + cr * stbi__float2fixed(1.40200f);
    g = y_fixed + cr * -stbi__float2fixed(0.71414f) +
        ((cb * -stbi__float2fixed(0.34414f)) & 0xffff0000);
    b = y_fixed + cb * stbi__float2fixed(1.77200f);
    r >>= 20;
    g >>= 20;
    b >>= 20;
    if ((unsigned)r > 255) {
      if (r < 0)
        r = 0;
      else
        r = 255;
    }
    if ((unsigned)g > 255) {
      if (g < 0)
        g = 0;
      else
        g = 255;
    }
    if ((unsigned)b > 255) {
      if (b < 0)
        b = 0;
      else
        b = 255;
    }
    out[0] = (unsigned char)r;
    out[1] = (unsigned char)g;
    out[2] = (unsigned char)b;
    out[3] = 255;
    out += step;
  }
}
#endif

// set up the kernels
static void stbi__setup_jpeg(stbi__jpeg *j) {
  j->resample_row_hv_2_kernel = stbi__resample_row_hv_2;
#if 0
  if (stbi__sse2_available()) {
    j->resample_row_hv_2_kernel = stbi__resample_row_hv_2_simd;
  }
#endif
}

// clean up the temporary component buffers
static void stbi__cleanup_jpeg(stbi__jpeg *j) {
  stbi__free_jpeg_components(j, j->s->img_n, 0);
}

typedef struct {
  resample_row_func resample;
  unsigned char *line0, *line1;
  int hs, vs;   // expansion factor in each axis
  int w_lores;  // horizontal pixels pre-expansion
  int ystep;    // how far through vertical expansion we are
  int ypos;     // which pre-expansion row we're on
} stbi__resample;

// fast 0..255 * 0..255 => 0..255 rounded multiplication
static unsigned char stbi__blinn_8x8(unsigned char x, unsigned char y) {
  unsigned t;
  t = x * y + 128;
  return (t + (t >> 8)) >> 8;
}

static unsigned char *load_jpeg_image(stbi__jpeg *z, int *out_x, int *out_y,
                                      int *comp, int req_comp) {
  int n, decode_n, is_rgb;
  z->s->img_n = 0;  // make stbi__cleanup_jpeg safe

  // validate req_comp
  if (req_comp < 0 || req_comp > 4) {
    return stbi__errpuc("bad req_comp", "Internal error");
  }

  // load a jpeg image from whichever source, but leave in YCbCr format
  if (!stbi__decode_jpeg_image(z)) {
    stbi__cleanup_jpeg(z);
    return NULL;
  }

  // determine actual number of components to generate
  n = req_comp ? req_comp : z->s->img_n >= 3 ? 3 : 1;
  is_rgb = z->s->img_n == 3 &&
           (z->rgb == 3 || (z->app14_color_transform == 0 && !z->jfif));
  if (z->s->img_n == 3 && n < 3 && !is_rgb) {
    decode_n = 1;
  } else {
    decode_n = z->s->img_n;
  }

  // nothing to do if no components requested; check this now to avoid
  // accessing uninitialized coutput[0] later
  if (decode_n <= 0) {
    stbi__cleanup_jpeg(z);
    return NULL;
  }

  // resample and color-convert
  {
    int k;
    unsigned int i, j;
    unsigned char *output;
    unsigned char *coutput[4];
    stbi__resample res_comp[4];
    bzero(coutput, sizeof(coutput));

    for (k = 0; k < decode_n; ++k) {
      stbi__resample *r = &res_comp[k];

      // allocate line buffer big enough for upsampling off the edges
      // with upsample factor of 4
      z->img_comp[k].linebuf = xmalloc(z->s->img_x + 3);

      r->hs = z->img_h_max / z->img_comp[k].h;
      r->vs = z->img_v_max / z->img_comp[k].v;
      r->ystep = r->vs >> 1;
      r->w_lores = (z->s->img_x + r->hs - 1) / r->hs;
      r->ypos = 0;
      r->line0 = r->line1 = z->img_comp[k].data;

      if (r->hs == 1 && r->vs == 1) {
        r->resample = resample_row_1;
      } else if (r->hs == 1 && r->vs == 2) {
        r->resample = stbi__resample_row_v_2;
      } else if (r->hs == 2 && r->vs == 1) {
        r->resample = stbi__resample_row_h_2;
      } else if (r->hs == 2 && r->vs == 2) {
        r->resample = z->resample_row_hv_2_kernel;
      } else {
        r->resample = stbi__resample_row_nearest;
      }
    }

    // can't error after this so, this is safe
    output = stbi__malloc_mad3(n, z->s->img_x, z->s->img_y, 1);

    // now go ahead and resample
    for (j = 0; j < z->s->img_y; ++j) {
      unsigned char *out = output + n * z->s->img_x * j;
      for (k = 0; k < decode_n; ++k) {
        stbi__resample *r = &res_comp[k];
        int y_bot = r->ystep >= (r->vs >> 1);
        coutput[k] =
            r->resample(z->img_comp[k].linebuf, y_bot ? r->line1 : r->line0,
                        y_bot ? r->line0 : r->line1, r->w_lores, r->hs);
        if (++r->ystep >= r->vs) {
          r->ystep = 0;
          r->line0 = r->line1;
          if (++r->ypos < z->img_comp[k].y) r->line1 += z->img_comp[k].w2;
        }
      }
      if (n >= 3) {
        unsigned char *y = coutput[0];
        if (z->s->img_n == 3) {
          if (is_rgb) {
            for (i = 0; i < z->s->img_x; ++i) {
              out[0] = y[i];
              out[1] = coutput[1][i];
              out[2] = coutput[2][i];
              out[3] = 255;
              out += n;
            }
          } else {
            stbi__YCbCr_to_RGB_row(out, y, coutput[1], coutput[2], z->s->img_x,
                                   n);
          }
        } else if (z->s->img_n == 4) {
          if (z->app14_color_transform == 0) {  // CMYK
            for (i = 0; i < z->s->img_x; ++i) {
              unsigned char m = coutput[3][i];
              out[0] = stbi__blinn_8x8(coutput[0][i], m);
              out[1] = stbi__blinn_8x8(coutput[1][i], m);
              out[2] = stbi__blinn_8x8(coutput[2][i], m);
              out[3] = 255;
              out += n;
            }
          } else if (z->app14_color_transform == 2) {  // YCCK
            stbi__YCbCr_to_RGB_row(out, y, coutput[1], coutput[2], z->s->img_x,
                                   n);
            for (i = 0; i < z->s->img_x; ++i) {
              unsigned char m = coutput[3][i];
              out[0] = stbi__blinn_8x8(255 - out[0], m);
              out[1] = stbi__blinn_8x8(255 - out[1], m);
              out[2] = stbi__blinn_8x8(255 - out[2], m);
              out += n;
            }
          } else {  // YCbCr + alpha?  Ignore the fourth channel for now
            stbi__YCbCr_to_RGB_row(out, y, coutput[1], coutput[2], z->s->img_x,
                                   n);
          }
        } else
          for (i = 0; i < z->s->img_x; ++i) {
            out[0] = out[1] = out[2] = y[i];
            out[3] = 255;  // not used if n==3
            out += n;
          }
      } else {
        if (is_rgb) {
          if (n == 1)
            for (i = 0; i < z->s->img_x; ++i)
              *out++ =
                  stbi__compute_y(coutput[0][i], coutput[1][i], coutput[2][i]);
          else {
            for (i = 0; i < z->s->img_x; ++i, out += 2) {
              out[0] =
                  stbi__compute_y(coutput[0][i], coutput[1][i], coutput[2][i]);
              out[1] = 255;
            }
          }
        } else if (z->s->img_n == 4 && z->app14_color_transform == 0) {
          for (i = 0; i < z->s->img_x; ++i) {
            unsigned char m = coutput[3][i];
            unsigned char r = stbi__blinn_8x8(coutput[0][i], m);
            unsigned char g = stbi__blinn_8x8(coutput[1][i], m);
            unsigned char b = stbi__blinn_8x8(coutput[2][i], m);
            out[0] = stbi__compute_y(r, g, b);
            out[1] = 255;
            out += n;
          }
        } else if (z->s->img_n == 4 && z->app14_color_transform == 2) {
          for (i = 0; i < z->s->img_x; ++i) {
            out[0] = stbi__blinn_8x8(255 - coutput[0][i], coutput[3][i]);
            out[1] = 255;
            out += n;
          }
        } else {
          unsigned char *y = coutput[0];
          if (n == 1)
            for (i = 0; i < z->s->img_x; ++i) out[i] = y[i];
          else
            for (i = 0; i < z->s->img_x; ++i) {
              *out++ = y[i];
              *out++ = 255;
            }
        }
      }
    }
    stbi__cleanup_jpeg(z);
    *out_x = z->s->img_x;
    *out_y = z->s->img_y;
    if (comp)
      *comp =
          z->s->img_n >= 3 ? 3 : 1;  // report original components, not output
    return output;
  }
}

static dontinline void *stbi__jpeg_load(stbi__context *s, int *x, int *y,
                                        int *comp, int req_comp,
                                        stbi__result_info *ri) {
  unsigned char *result;
  stbi__jpeg *j;
  j = malloc(sizeof(stbi__jpeg));
  if (!j) return stbi__errpuc("outofmem", "Out of memory");
  bzero(j, sizeof(stbi__jpeg));
  j->s = s;
  stbi__setup_jpeg(j);
  result = load_jpeg_image(j, x, y, comp, req_comp);
  free(j);
  return result;
}

static int stbi__jpeg_test(stbi__context *s) {
  int r;
  stbi__jpeg *j;
  j = malloc(sizeof(stbi__jpeg));
  if (!j) return stbi__err("outofmem", "Out of memory");
  bzero(j, sizeof(stbi__jpeg));
  j->s = s;
  stbi__setup_jpeg(j);
  r = stbi__decode_jpeg_header(j, STBI__SCAN_type);
  stbi__rewind(s);
  free(j);
  return r;
}

static int stbi__jpeg_info_raw(stbi__jpeg *j, int *x, int *y, int *comp) {
  if (!stbi__decode_jpeg_header(j, STBI__SCAN_header)) {
    stbi__rewind(j->s);
    return 0;
  }
  if (x) *x = j->s->img_x;
  if (y) *y = j->s->img_y;
  if (comp) *comp = j->s->img_n >= 3 ? 3 : 1;
  return 1;
}

static int stbi__jpeg_info(stbi__context *s, int *x, int *y, int *comp) {
  int result;
  stbi__jpeg *j = (stbi__jpeg *)(malloc(sizeof(stbi__jpeg)));
  if (!j) return stbi__err("outofmem", "Out of memory");
  bzero(j, sizeof(stbi__jpeg));
  j->s = s;
  result = stbi__jpeg_info_raw(j, x, y, comp);
  free(j);
  return result;
}

// public domain zlib decode    v0.2  Sean Barrett 2006-11-18
//    simple implementation
//      - all input must be provided in an upfront buffer
//      - all output is written to a single output buffer (can malloc/realloc)
//    performance
//      - fast huffman

// fast-way is faster to check than jpeg huffman, but slow way is slower
#define STBI__ZFAST_BITS 9  // accelerate all cases in default tables
#define STBI__ZFAST_MASK ((1 << STBI__ZFAST_BITS) - 1)
#define STBI__ZNSYMS     288  // number of symbols in literal/length alphabet

// zlib-style huffman encoding
// (jpegs packs from left, zlib from right, so can't share code)
typedef struct {
  uint16_t fast[1 << STBI__ZFAST_BITS];
  uint16_t firstcode[16];
  int maxcode[17];
  uint16_t firstsymbol[16];
  unsigned char size[STBI__ZNSYMS];
  uint16_t value[STBI__ZNSYMS];
} stbi__zhuffman;

static uint32_t ReverseBits32(uint32_t x) {
  x = bswap_32(x);
  x = (x & 0xaaaaaaaa) >> 1 | (x & 0x55555555) << 1;
  x = (x & 0xcccccccc) >> 2 | (x & 0x33333333) << 2;
  x = (x & 0xf0f0f0f0) >> 4 | (x & 0x0f0f0f0f) << 4;
  return x;
}

static int ReverseBits16(int x) {
  return ReverseBits32(x) >> 16;
}

forceinline int stbi__bit_reverse(int v, int bits) {
  assert(bits <= 16);
  // to bit reverse n bits, reverse 16 and shift
  // e.g. 11 bits, bit reverse and shift away 5
  return ReverseBits16(v) >> (16 - bits);
}

static int stbi__zbuild_huffman(stbi__zhuffman *z,
                                const unsigned char *sizelist, int num) {
  int i, k = 0;
  int code, next_code[16], sizes[17];
  // DEFLATE spec for generating codes
  bzero(sizes, sizeof(sizes));
  bzero(z->fast, sizeof(z->fast));
  for (i = 0; i < num; ++i) ++sizes[sizelist[i]];
  sizes[0] = 0;
  for (i = 1; i < 16; ++i)
    if (sizes[i] > (1 << i)) return stbi__err("bad sizes", "Corrupt PNG");
  code = 0;
  for (i = 1; i < 16; ++i) {
    next_code[i] = code;
    z->firstcode[i] = (uint16_t)code;
    z->firstsymbol[i] = (uint16_t)k;
    code = (code + sizes[i]);
    if (sizes[i])
      if (code - 1 >= (1 << i))
        return stbi__err("bad codelengths", "Corrupt PNG");
    z->maxcode[i] = code << (16 - i);  // preshift for inner loop
    code <<= 1;
    k += sizes[i];
  }
  z->maxcode[16] = 0x10000;  // sentinel
  for (i = 0; i < num; ++i) {
    int s = sizelist[i];
    if (s) {
      int c = next_code[s] - z->firstcode[s] + z->firstsymbol[s];
      uint16_t fastv = (uint16_t)((s << 9) | i);
      z->size[c] = (unsigned char)s;
      z->value[c] = (uint16_t)i;
      if (s <= STBI__ZFAST_BITS) {
        int j = stbi__bit_reverse(next_code[s], s);
        while (j < (1 << STBI__ZFAST_BITS)) {
          z->fast[j] = fastv;
          j += (1 << s);
        }
      }
      ++next_code[s];
    }
  }
  return 1;
}

// zlib-from-memory implementation for PNG reading
//    because PNG allows splitting the zlib stream arbitrarily,
//    and it's annoying structurally to have PNG call ZLIB call PNG,
//    we require PNG read all the IDATs and combine them into a single
//    memory buffer
typedef struct {
  unsigned char *zbuffer, *zbuffer_end;
  int num_bits;
  int hit_zeof_once;
  uint32_t code_buffer;
  char *zout;
  char *zout_start;
  char *zout_end;
  int z_expandable;
  stbi__zhuffman z_length, z_distance;
} stbi__zbuf;

forceinline int stbi__zeof(stbi__zbuf *z) {
  return (z->zbuffer >= z->zbuffer_end);
}

forceinline unsigned char stbi__zget8(stbi__zbuf *z) {
  return stbi__zeof(z) ? 0 : *z->zbuffer++;
}

static void stbi__fill_bits(stbi__zbuf *z) {
  do {
    if (z->code_buffer >= (1u << z->num_bits)) {
      z->zbuffer = z->zbuffer_end;  // treat this as EOF so we fail.
      return;
    }
    z->code_buffer |= (unsigned int)stbi__zget8(z) << z->num_bits;
    z->num_bits += 8;
  } while (z->num_bits <= 24);
}

forceinline unsigned int stbi__zreceive(stbi__zbuf *z, int n) {
  unsigned int k;
  if (z->num_bits < n) stbi__fill_bits(z);
  k = z->code_buffer & ((1 << n) - 1);
  z->code_buffer >>= n;
  z->num_bits -= n;
  return k;
}

static int stbi__zhuffman_decode_slowpath(stbi__zbuf *a, stbi__zhuffman *z) {
  int b, s, k;
  // not resolved by fast table, so compute it the slow way
  // use jpeg approach, which requires MSbits at top
  k = stbi__bit_reverse(a->code_buffer, 16);
  for (s = STBI__ZFAST_BITS + 1;; ++s)
    if (k < z->maxcode[s]) break;
  if (s >= 16) return -1;  // invalid code!
  // code size is s, so:
  b = (k >> (16 - s)) - z->firstcode[s] + z->firstsymbol[s];
  if (b >= STBI__ZNSYMS) {
    // some data was corrupt somewhere!
    return -1;
  }
  if (z->size[b] != s) {
    // was originally an assert, but report failure instead.
    return -1;
  }
  a->code_buffer >>= s;
  a->num_bits -= s;
  return z->value[b];
}

forceinline int stbi__zhuffman_decode(stbi__zbuf *a, stbi__zhuffman *z) {
  int b, s;
  if (a->num_bits < 16) {
    if (stbi__zeof(a)) {
      if (!a->hit_zeof_once) {
        // This is the first time we hit eof, insert 16 extra padding bits
        // to allow us to keep going; if we actually consume any of them
        // though, that is invalid data. This is caught later.
        a->hit_zeof_once = 1;
        a->num_bits += 16;  // add 16 implicit zero bits
      } else {
        // We already inserted our extra 16 padding bits and are again
        // out, this stream is actually prematurely terminated.
        return -1;
      }
    } else {
      stbi__fill_bits(a);
    }
  }
  b = z->fast[a->code_buffer & STBI__ZFAST_MASK];
  if (b) {
    s = b >> 9;
    a->code_buffer >>= s;
    a->num_bits -= s;
    return b & 511;
  }
  return stbi__zhuffman_decode_slowpath(a, z);
}

static int stbi__zexpand(stbi__zbuf *z, char *zout, int n) {
  char *q;
  unsigned int cur, limit;
  z->zout = zout;
  if (!z->z_expandable) return stbi__err("output buffer limit", "Corrupt PNG");
  cur = (unsigned int)(z->zout - z->zout_start);
  limit = (unsigned)(z->zout_end - z->zout_start);
  if (UINT_MAX - cur < (unsigned)n) {
    return stbi__err("outofmem", "Out of memory");
  }
  while (cur + n > limit) {
    if (limit > UINT_MAX / 2) return stbi__err("outofmem", "Out of memory");
    limit *= 2;
  }
  q = (char *)realloc(z->zout_start, limit);
  if (q == NULL) return stbi__err("outofmem", "Out of memory");
  z->zout_start = q;
  z->zout = q + cur;
  z->zout_end = q + limit;
  return 1;
}

static const int stbi__zlength_base[31] = {
    3,  4,  5,  6,  7,  8,  9,  10,  11,  13,  15,  17,  19,  23, 27, 31,
    35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0,  0,
};

static const int stbi__zlength_extra[31] = {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
    3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 0, 0,
};

static const int stbi__zdist_base[32] = {
    1,    2,    3,    4,    5,    7,     9,     13,    17,  25,   33,
    49,   65,   97,   129,  193,  257,   385,   513,   769, 1025, 1537,
    2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577, 0,   0,
};

static const int stbi__zdist_extra[32] = {
    0, 0, 0, 0, 1, 1, 2, 2,  3,  3,  4,  4,  5,  5,  6,
    6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13,
};

static int stbi__parse_huffman_block(stbi__zbuf *a) {
  char *zout = a->zout;
  for (;;) {
    int z = stbi__zhuffman_decode(a, &a->z_length);
    if (z < 256) {
      if (z < 0) return stbi__err("bad huffman code", "Corrupt PNG");
      if (zout >= a->zout_end) {
        if (!stbi__zexpand(a, zout, 1)) return 0;
        zout = a->zout;
      }
      *zout++ = (char)z;
    } else {
      unsigned char *p;
      int len, dist;
      if (z == 256) {
        a->zout = zout;
        if (a->hit_zeof_once && a->num_bits < 16) {
          // The first time we hit zeof, we inserted 16 extra zero bits into our
          // bit buffer so the decoder can just do its speculative decoding. But
          // if we actually consumed any of those bits (which is the case when
          // num_bits < 16), the stream actually read past the end so it is
          // malformed.
          return stbi__err("unexpected end", "Corrupt PNG");
        }
        return 1;
      }
      if (z >= 286) {
        // per DEFLATE, length codes 286 and 287
        // must not appear in compressed data
        return stbi__err("bad huffman code", "Corrupt PNG");
      }
      z -= 257;
      len = stbi__zlength_base[z];
      if (stbi__zlength_extra[z])
        len += stbi__zreceive(a, stbi__zlength_extra[z]);
      z = stbi__zhuffman_decode(a, &a->z_distance);
      if (z < 0 || z >= 30) {
        // per DEFLATE, distance codes 30 and 31
        // must not appear in compressed data
        return stbi__err("bad huffman code", "Corrupt PNG");
      }
      dist = stbi__zdist_base[z];
      if (stbi__zdist_extra[z]) dist += stbi__zreceive(a, stbi__zdist_extra[z]);
      if (zout - a->zout_start < dist)
        return stbi__err("bad dist", "Corrupt PNG");
      if (len > a->zout_end - zout) {
        if (!stbi__zexpand(a, zout, len)) return 0;
        zout = a->zout;
      }
      p = (unsigned char *)(zout - dist);
      if (dist == 1) {  // run of one byte; common in images.
        unsigned char v = *p;
        if (len) {
          do *zout++ = v;
          while (--len);
        }
      } else {
        if (len) {
          do *zout++ = *p++;
          while (--len);
        }
      }
    }
  }
}

static int stbi__compute_huffman_codes(stbi__zbuf *a) {
  static const unsigned char length_dezigzag[19] = {
      16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
  stbi__zhuffman z_codelength;
  unsigned char lencodes[286 + 32 + 137];  // padding for maximum single op
  unsigned char codelength_sizes[19];
  int i, n;
  int hlit = stbi__zreceive(a, 5) + 257;
  int hdist = stbi__zreceive(a, 5) + 1;
  int hclen = stbi__zreceive(a, 4) + 4;
  int ntot = hlit + hdist;
  bzero(codelength_sizes, sizeof(codelength_sizes));
  for (i = 0; i < hclen; ++i) {
    int s = stbi__zreceive(a, 3);
    codelength_sizes[length_dezigzag[i]] = (unsigned char)s;
  }
  if (!stbi__zbuild_huffman(&z_codelength, codelength_sizes, 19)) return 0;
  n = 0;
  while (n < ntot) {
    int c = stbi__zhuffman_decode(a, &z_codelength);
    if (c < 0 || c >= 19) return stbi__err("bad codelengths", "Corrupt PNG");
    if (c < 16)
      lencodes[n++] = (unsigned char)c;
    else {
      unsigned char fill = 0;
      if (c == 16) {
        c = stbi__zreceive(a, 2) + 3;
        if (n == 0) return stbi__err("bad codelengths", "Corrupt PNG");
        fill = lencodes[n - 1];
      } else if (c == 17) {
        c = stbi__zreceive(a, 3) + 3;
      } else if (c == 18) {
        c = stbi__zreceive(a, 7) + 11;
      } else {
        return stbi__err("bad codelengths", "Corrupt PNG");
      }
      if (ntot - n < c) return stbi__err("bad codelengths", "Corrupt PNG");
      memset(lencodes + n, fill, c);
      n += c;
    }
  }
  if (n != ntot) return stbi__err("bad codelengths", "Corrupt PNG");
  if (!stbi__zbuild_huffman(&a->z_length, lencodes, hlit)) return 0;
  if (!stbi__zbuild_huffman(&a->z_distance, lencodes + hlit, hdist)) return 0;
  return 1;
}

static int stbi__parse_uncompressed_block(stbi__zbuf *a) {
  unsigned char header[4];
  int len, nlen, k;
  if (a->num_bits & 7) stbi__zreceive(a, a->num_bits & 7);  // discard
  // drain the bit-packed data into header
  k = 0;
  while (a->num_bits > 0) {
    header[k++] =
        (unsigned char)(a->code_buffer & 255);  // suppress MSVC run-time check
    a->code_buffer >>= 8;
    a->num_bits -= 8;
  }
  if (a->num_bits < 0) return stbi__err("zlib corrupt", "Corrupt PNG");
  // now fill header the normal way
  while (k < 4) header[k++] = stbi__zget8(a);
  len = header[1] * 256 + header[0];
  nlen = header[3] * 256 + header[2];
  if (nlen != (len ^ 0xffff)) return stbi__err("zlib corrupt", "Corrupt PNG");
  if (a->zbuffer + len > a->zbuffer_end)
    return stbi__err("read past buffer", "Corrupt PNG");
  if (a->zout + len > a->zout_end)
    if (!stbi__zexpand(a, a->zout, len)) return 0;
  memcpy(a->zout, a->zbuffer, len);
  a->zbuffer += len;
  a->zout += len;
  return 1;
}

static int stbi__parse_zlib_header(stbi__zbuf *a) {
  int cmf = stbi__zget8(a);
  int cm = cmf & 15;
  /* int cinfo = cmf >> 4; */
  int flg = stbi__zget8(a);
  if (stbi__zeof(a))
    return stbi__err("bad zlib header", "Corrupt PNG");  // zlib spec
  if ((cmf * 256 + flg) % 31 != 0)
    return stbi__err("bad zlib header", "Corrupt PNG");  // zlib spec
  if (flg & 32)
    return stbi__err("no preset dict",
                     "Corrupt PNG");  // preset dictionary not allowed in png
  if (cm != 8)
    return stbi__err("bad compression",
                     "Corrupt PNG");  // DEFLATE required for png
  // window = 1 << (8 + cinfo)... but who cares, we fully buffer output
  return 1;
}

static const unsigned char stbi__zdefault_length[STBI__ZNSYMS] = {
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
    9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
    9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
    9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
    9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8};

static const unsigned char stbi__zdefault_distance[32] = {
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
};

/*
  Init algorithm:{
  int i;   // use <= to match clearly with spec
  for (i=0; i <= 143; ++i)     stbi__zdefault_length[i]   = 8;
  for (   ; i <= 255; ++i)     stbi__zdefault_length[i]   = 9;
  for (   ; i <= 279; ++i)     stbi__zdefault_length[i]   = 7;
  for (   ; i <= 287; ++i)     stbi__zdefault_length[i]   = 8;
  for (i=0; i <=  31; ++i)     stbi__zdefault_distance[i] = 5;
  }
*/

static int stbi__parse_zlib(stbi__zbuf *a, int parse_header) {
  int final, type;
  if (parse_header)
    if (!stbi__parse_zlib_header(a)) return 0;
  a->num_bits = 0;
  a->code_buffer = 0;
  a->hit_zeof_once = 0;
  do {
    final = stbi__zreceive(a, 1);
    type = stbi__zreceive(a, 2);
    if (type == 0) {
      if (!stbi__parse_uncompressed_block(a)) return 0;
    } else if (type == 3) {
      return 0;
    } else {
      if (type == 1) {
        // use fixed code lengths
        if (!stbi__zbuild_huffman(&a->z_length, stbi__zdefault_length,
                                  STBI__ZNSYMS))
          return 0;
        if (!stbi__zbuild_huffman(&a->z_distance, stbi__zdefault_distance, 32))
          return 0;
      } else {
        if (!stbi__compute_huffman_codes(a)) return 0;
      }
      if (!stbi__parse_huffman_block(a)) return 0;
    }
  } while (!final);
  return 1;
}

static int stbi__do_zlib(stbi__zbuf *a, char *obuf, int olen, int exp,
                         int parse_header) {
  a->zout_start = obuf;
  a->zout = obuf;
  a->zout_end = obuf + olen;
  a->z_expandable = exp;

  return stbi__parse_zlib(a, parse_header);
}

char *stbi_zlib_decode_malloc_guesssize(const char *buffer, int len,
                                        int initial_size, int *outlen) {
  char *res, *p;
  stbi__zbuf *a;
  a = NULL;
  res = NULL;
  if ((p = (char *)malloc(initial_size)) &&
      (a = (stbi__zbuf *)malloc(sizeof(stbi__zbuf)))) {
    a->zbuffer = (unsigned char *)buffer;
    a->zbuffer_end = (unsigned char *)buffer + len;
    if (stbi__do_zlib(a, p, initial_size, 1, 1)) {
      if (outlen) *outlen = (int)(a->zout - a->zout_start);
      res = a->zout_start;
      a->zout_start = NULL;
    }
  }
  free(a->zout_start);
  free(a);
  return res;
}

char *stbi_zlib_decode_malloc(char const *buffer, int len, int *outlen) {
  return stbi_zlib_decode_malloc_guesssize(buffer, len, 16384, outlen);
}

char *stbi_zlib_decode_malloc_guesssize_headerflag(const char *buffer, int len,
                                                   int initial_size,
                                                   int *outlen,
                                                   int parse_header) {
  char *res;
  stbi__zbuf *a = malloc(sizeof(stbi__zbuf));
  char *p = (char *)malloc(initial_size);
  if (!p) return NULL;
  a->zbuffer = (unsigned char *)buffer;
  a->zbuffer_end = (unsigned char *)buffer + len;
  if (stbi__do_zlib(a, p, initial_size, 1, parse_header)) {
    if (outlen) *outlen = (int)(a->zout - a->zout_start);
    res = a->zout_start;
  } else {
    free(a->zout_start);
    res = NULL;
  }
  free(a);
  return res;
}

int stbi_zlib_decode_buffer(char *obuffer, int olen, char const *ibuffer,
                            int ilen) {
  stbi__zbuf a;
  a.zbuffer = (unsigned char *)ibuffer;
  a.zbuffer_end = (unsigned char *)ibuffer + ilen;
  if (stbi__do_zlib(&a, obuffer, olen, 0, 1))
    return (int)(a.zout - a.zout_start);
  else
    return -1;
}

char *stbi_zlib_decode_noheader_malloc(char const *buffer, int len,
                                       int *outlen) {
  stbi__zbuf a;
  char *p = (char *)malloc(16384);
  if (p == NULL) return NULL;
  a.zbuffer = (unsigned char *)buffer;
  a.zbuffer_end = (unsigned char *)buffer + len;
  if (stbi__do_zlib(&a, p, 16384, 1, 0)) {
    if (outlen) *outlen = (int)(a.zout - a.zout_start);
    return a.zout_start;
  } else {
    free(a.zout_start);
    return NULL;
  }
}

int stbi_zlib_decode_noheader_buffer(char *obuffer, int olen,
                                     const char *ibuffer, int ilen) {
  stbi__zbuf a;
  a.zbuffer = (unsigned char *)ibuffer;
  a.zbuffer_end = (unsigned char *)ibuffer + ilen;
  if (stbi__do_zlib(&a, obuffer, olen, 0, 0))
    return (int)(a.zout - a.zout_start);
  else
    return -1;
}

// public domain "baseline" PNG decoder   v0.10  Sean Barrett 2006-11-18
//    simple implementation
//      - only 8-bit samples
//      - no CRC checking
//      - allocates lots of intermediate memory
//        - avoids problem of streaming data between subsystems
//        - avoids explicit window management
//    performance
//      - uses stb_zlib, a PD zlib implementation with fast huffman decoding

typedef struct {
  uint32_t length;
  uint32_t type;
} stbi__pngchunk;

static stbi__pngchunk stbi__get_chunk_header(stbi__context *s) {
  stbi__pngchunk c;
  c.length = stbi__get32be(s);
  c.type = stbi__get32be(s);
  return c;
}

static int stbi__check_png_header(stbi__context *s) {
  int i;
  for (i = 0; i < 8; ++i) {
    if (stbi__get8(s) != kPngSig[i]) {
      return stbi__err("bad png sig", "Not a PNG");
    }
  }
  return 1;
}

typedef struct {
  stbi__context *s;
  unsigned char *idata, *expanded, *out;
  int depth;
} stbi__png;

enum {
  STBI__F_none = 0,
  STBI__F_sub = 1,
  STBI__F_up = 2,
  STBI__F_avg = 3,
  STBI__F_paeth = 4,
  // synthetic filter used for first scanline to avoid needing a dummy row of 0s
  STBI__F_avg_first
};

static int stbi__de_iphone_flag = 0;
static int stbi__unpremultiply_on_load = 0;
static unsigned char first_row_filter[5] = {
    STBI__F_none, STBI__F_sub, STBI__F_none, STBI__F_avg_first,
    STBI__F_sub  // Paeth with b = c = 0 turns out to be equivalent to sub
};

static int stbi__paeth(int a, int b, int c) {
  // This formulation looks very different from the reference in the PNG spec,
  // but is actually equivalent and has favorable data dependencies and admits
  // straightforward generation of branch-free code, which helps performance
  // significantly.
  int thresh = c * 3 - (a + b);
  int lo = a < b ? a : b;
  int hi = a < b ? b : a;
  int t0 = (hi <= thresh) ? lo : c;
  int t1 = (thresh <= lo) ? hi : t0;
  return t1;
}

static const unsigned char stbi__depth_scale_table[9] = {
    0, 0xff, 0x55, 0, 0x11, 0, 0, 0, 0x01};

// adds an extra all-255 alpha channel
// dest == src is legal
// img_n must be 1 or 3
static void stbi__create_png_alpha_expand8(unsigned char *dest,
                                           unsigned char *src, uint32_t x,
                                           int img_n) {
  int i;
  // must process data backwards since we allow dest==src
  if (img_n == 1) {
    for (i = x - 1; i >= 0; --i) {
      dest[i * 2 + 1] = 255;
      dest[i * 2 + 0] = src[i];
    }
  } else {
    assert(img_n == 3);
    for (i = x - 1; i >= 0; --i) {
      dest[i * 4 + 3] = 255;
      dest[i * 4 + 2] = src[i * 3 + 2];
      dest[i * 4 + 1] = src[i * 3 + 1];
      dest[i * 4 + 0] = src[i * 3 + 0];
    }
  }
}

// create the png data from post-deflated data
static int stbi__create_png_image_raw(stbi__png *a, unsigned char *raw,
                                      uint32_t raw_len, int out_n, uint32_t x,
                                      uint32_t y, int depth, int color) {
  int bytes = (depth == 16 ? 2 : 1);
  stbi__context *s = a->s;
  uint32_t i, j, stride = x * out_n * bytes;
  uint32_t img_len, img_width_bytes;
  unsigned char *filler_buf;
  int all_ok = 1;
  int k;
  int img_n = s->img_n;  // copy it into a local for later

  int output_bytes = out_n * bytes;
  int filter_bytes = img_n * bytes;
  int width = x;

  assert(out_n == s->img_n || out_n == s->img_n + 1);
  a->out = stbi__malloc_mad3(x, y, output_bytes,
                             0);  // extra bytes to write off the end into

  // note: error exits here don't need to clean up a->out individually,
  // stbi__do_png always does on error.
  if (!stbi__mad3sizes_valid(img_n, x, depth, 7))
    return stbi__err("too large", "Corrupt PNG");
  img_width_bytes = (((img_n * x * depth) + 7) >> 3);
  if (!stbi__mad2sizes_valid(img_width_bytes, y, img_width_bytes))
    return stbi__err("too large", "Corrupt PNG");
  img_len = (img_width_bytes + 1) * y;

  // we used to check for exact match between raw_len and img_len on
  // non-interlaced PNGs, but issue #276 reported a PNG in the wild that had
  // extra data at the end (all zeros), so just check for raw_len < img_len
  // always.
  if (raw_len < img_len) return stbi__err("not enough pixels", "Corrupt PNG");

  // Allocate two scan lines worth of filter workspace buffer.
  filler_buf = stbi__malloc_mad2(img_width_bytes, 2, 0);
  if (!filler_buf) return stbi__err("outofmem", "Out of memory");

  // Filtering for low-bit-depth images
  if (depth < 8) {
    filter_bytes = 1;
    width = img_width_bytes;
  }

  for (j = 0; j < y; ++j) {
    // cur/prior filter buffers alternate
    unsigned char *cur = filler_buf + (j & 1) * img_width_bytes;
    unsigned char *prior = filler_buf + (~j & 1) * img_width_bytes;
    unsigned char *dest = a->out + stride * j;
    int nk = width * filter_bytes;
    int filter = *raw++;

    // check filter type
    if (filter > 4) {
      all_ok = stbi__err("invalid filter", "Corrupt PNG");
      break;
    }

    // if first row, use special filter that doesn't sample previous row
    if (j == 0) filter = first_row_filter[filter];

    // perform actual filtering
    switch (filter) {
      case STBI__F_none:
        memcpy(cur, raw, nk);
        break;
      case STBI__F_sub:
        memcpy(cur, raw, filter_bytes);
        for (k = filter_bytes; k < nk; ++k)
          cur[k] = STBI__BYTECAST(raw[k] + cur[k - filter_bytes]);
        break;
      case STBI__F_up:
        for (k = 0; k < nk; ++k) cur[k] = STBI__BYTECAST(raw[k] + prior[k]);
        break;
      case STBI__F_avg:
        for (k = 0; k < filter_bytes; ++k)
          cur[k] = STBI__BYTECAST(raw[k] + (prior[k] >> 1));
        for (k = filter_bytes; k < nk; ++k)
          cur[k] = STBI__BYTECAST(raw[k] +
                                  ((prior[k] + cur[k - filter_bytes]) >> 1));
        break;
      case STBI__F_paeth:
        for (k = 0; k < filter_bytes; ++k)
          // prior[k] == stbi__paeth(0, prior[k], 0)
          cur[k] = STBI__BYTECAST(raw[k] + prior[k]);
        for (k = filter_bytes; k < nk; ++k)
          cur[k] = STBI__BYTECAST(raw[k] +
                                  stbi__paeth(cur[k - filter_bytes], prior[k],
                                              prior[k - filter_bytes]));
        break;
      case STBI__F_avg_first:
        memcpy(cur, raw, filter_bytes);
        for (k = filter_bytes; k < nk; ++k)
          cur[k] = STBI__BYTECAST(raw[k] + (cur[k - filter_bytes] >> 1));
        break;
    }

    raw += nk;

    // expand decoded bits in cur to dest, also adding an extra alpha channel
    // if desired
    if (depth < 8) {
      unsigned char scale = (color == 0)
                                ? stbi__depth_scale_table[depth]
                                : 1;  // scale grayscale values to 0..255 range
      unsigned char *in = cur;
      unsigned char *out = dest;
      unsigned char inb = 0;
      uint32_t nsmp = x * img_n;

      // expand bits to bytes first
      if (depth == 4) {
        for (i = 0; i < nsmp; ++i) {
          if ((i & 1) == 0) inb = *in++;
          *out++ = scale * (inb >> 4);
          inb <<= 4;
        }
      } else if (depth == 2) {
        for (i = 0; i < nsmp; ++i) {
          if ((i & 3) == 0) inb = *in++;
          *out++ = scale * (inb >> 6);
          inb <<= 2;
        }
      } else {
        assert(depth == 1);
        for (i = 0; i < nsmp; ++i) {
          if ((i & 7) == 0) inb = *in++;
          *out++ = scale * (inb >> 7);
          inb <<= 1;
        }
      }

      // insert alpha=255 values if desired
      if (img_n != out_n) stbi__create_png_alpha_expand8(dest, dest, x, img_n);
    } else if (depth == 8) {
      if (img_n == out_n)
        memcpy(dest, cur, x * img_n);
      else
        stbi__create_png_alpha_expand8(dest, cur, x, img_n);
    } else if (depth == 16) {
      // convert the image data from big-endian to platform-native
      // TODO TYPES
      uint16_t *dest16 = (uint16_t *)dest;
      uint32_t nsmp = x * img_n;

      if (img_n == out_n) {
        for (i = 0; i < nsmp; ++i, ++dest16, cur += 2)
          *dest16 = (cur[0] << 8) | cur[1];
      } else {
        assert(img_n + 1 == out_n);
        if (img_n == 1) {
          for (i = 0; i < x; ++i, dest16 += 2, cur += 2) {
            dest16[0] = (cur[0] << 8) | cur[1];
            dest16[1] = 0xffff;
          }
        } else {
          assert(img_n == 3);
          for (i = 0; i < x; ++i, dest16 += 4, cur += 6) {
            dest16[0] = (cur[0] << 8) | cur[1];
            dest16[1] = (cur[2] << 8) | cur[3];
            dest16[2] = (cur[4] << 8) | cur[5];
            dest16[3] = 0xffff;
          }
        }
      }
    }
  }

  free(filler_buf);
  if (!all_ok) return 0;

  return 1;
}

static int stbi__create_png_image(stbi__png *a, unsigned char *image_data,
                                  uint32_t image_data_len, int out_n, int depth,
                                  int color, int interlaced) {
  int bytes = (depth == 16 ? 2 : 1);
  int out_bytes = out_n * bytes;
  unsigned char *final;
  int p;
  if (!interlaced)
    return stbi__create_png_image_raw(a, image_data, image_data_len, out_n,
                                      a->s->img_x, a->s->img_y, depth, color);
  // de-interlacing
  final = stbi__malloc_mad3(a->s->img_x, a->s->img_y, out_bytes, 0);
  if (!final) return stbi__err("outofmem", "Out of memory");
  for (p = 0; p < 7; ++p) {
    int xorig[] = {0, 4, 0, 2, 0, 1, 0};
    int yorig[] = {0, 0, 4, 0, 2, 0, 1};
    int xspc[] = {8, 8, 4, 4, 2, 2, 1};
    int yspc[] = {8, 8, 8, 4, 4, 2, 2};
    int i, j, x, y;
    // pass1_x[4] = 0, pass1_x[5] = 1, pass1_x[12] = 1
    x = (a->s->img_x - xorig[p] + xspc[p] - 1) / xspc[p];
    y = (a->s->img_y - yorig[p] + yspc[p] - 1) / yspc[p];
    if (x && y) {
      uint32_t img_len = ((((a->s->img_n * x * depth) + 7) >> 3) + 1) * y;
      if (!stbi__create_png_image_raw(a, image_data, image_data_len, out_n, x,
                                      y, depth, color)) {
        free(final);
        return 0;
      }
      for (j = 0; j < y; ++j) {
        for (i = 0; i < x; ++i) {
          int out_y = j * yspc[p] + yorig[p];
          int out_x = i * xspc[p] + xorig[p];
          memcpy(final + out_y * a->s->img_x * out_bytes + out_x * out_bytes,
                 a->out + (j * x + i) * out_bytes, out_bytes);
        }
      }
      free(a->out);
      image_data += img_len;
      image_data_len -= img_len;
    }
  }
  a->out = final;
  return 1;
}

static int stbi__compute_transparency(stbi__png *z, unsigned char tc[3],
                                      int out_n) {
  stbi__context *s = z->s;
  uint32_t i, pixel_count = s->img_x * s->img_y;
  unsigned char *p = z->out;
  // compute color-based transparency, assuming we've
  // already got 255 as the alpha value in the output
  assert(out_n == 2 || out_n == 4);
  if (out_n == 2) {
    for (i = 0; i < pixel_count; ++i) {
      p[1] = (p[0] == tc[0] ? 0 : 255);
      p += 2;
    }
  } else {
    for (i = 0; i < pixel_count; ++i) {
      if (p[0] == tc[0] && p[1] == tc[1] && p[2] == tc[2]) p[3] = 0;
      p += 4;
    }
  }
  return 1;
}

static int stbi__compute_transparency16(stbi__png *z, uint16_t tc[3],
                                        int out_n) {
  stbi__context *s = z->s;
  uint32_t i, pixel_count = s->img_x * s->img_y;
  uint16_t *p = (uint16_t *)z->out;
  // compute color-based transparency, assuming we've
  // already got 65535 as the alpha value in the output
  assert(out_n == 2 || out_n == 4);
  if (out_n == 2) {
    for (i = 0; i < pixel_count; ++i) {
      p[1] = (p[0] == tc[0] ? 0 : 65535);
      p += 2;
    }
  } else {
    for (i = 0; i < pixel_count; ++i) {
      if (p[0] == tc[0] && p[1] == tc[1] && p[2] == tc[2]) p[3] = 0;
      p += 4;
    }
  }
  return 1;
}

static int stbi__expand_png_palette(stbi__png *a, unsigned char *palette,
                                    int len, int pal_img_n) {
  uint32_t i, pixel_count = a->s->img_x * a->s->img_y;
  unsigned char *p, *temp_out, *orig = a->out;
  p = stbi__malloc_mad2(pixel_count, pal_img_n, 0);
  // between here and free(out) below, exitting would leak
  temp_out = p;
  if (pal_img_n == 3) {
    for (i = 0; i < pixel_count; ++i) {
      int n = orig[i] * 4;
      p[0] = palette[n];
      p[1] = palette[n + 1];
      p[2] = palette[n + 2];
      p += 3;
    }
  } else {
    for (i = 0; i < pixel_count; ++i) {
      int n = orig[i] * 4;
      p[0] = palette[n];
      p[1] = palette[n + 1];
      p[2] = palette[n + 2];
      p[3] = palette[n + 3];
      p += 4;
    }
  }
  free(a->out);
  a->out = temp_out;
  return 1;
}

void stbi_set_unpremultiply_on_load(int flag_true_if_should_unpremultiply) {
  stbi__unpremultiply_on_load = flag_true_if_should_unpremultiply;
}

void stbi_convert_iphone_png_to_rgb(int flag_true_if_should_convert) {
  stbi__de_iphone_flag = flag_true_if_should_convert;
}

static void stbi__de_iphone(stbi__png *z) {
  stbi__context *s = z->s;
  uint32_t i, pixel_count = s->img_x * s->img_y;
  unsigned char *p = z->out;
  if (s->img_out_n == 3) {  // convert bgr to rgb
    for (i = 0; i < pixel_count; ++i) {
      unsigned char t = p[0];
      p[0] = p[2];
      p[2] = t;
      p += 3;
    }
  } else {
    assert(s->img_out_n == 4);
    if (stbi__unpremultiply_on_load) {
      // convert bgr to rgb and unpremultiply
      for (i = 0; i < pixel_count; ++i) {
        unsigned char a = p[3];
        unsigned char t = p[0];
        if (a) {
          unsigned char half = a / 2;
          p[0] = (p[2] * 255 + half) / a;
          p[1] = (p[1] * 255 + half) / a;
          p[2] = (t * 255 + half) / a;
        } else {
          p[0] = p[2];
          p[2] = t;
        }
        p += 4;
      }
    } else {
      // convert bgr to rgb
      for (i = 0; i < pixel_count; ++i) {
        unsigned char t = p[0];
        p[0] = p[2];
        p[2] = t;
        p += 4;
      }
    }
  }
}

#define STBI__PNG_TYPE(a, b, c, d)                                        \
  (((unsigned)(a) << 24) + ((unsigned)(b) << 16) + ((unsigned)(c) << 8) + \
   (unsigned)(d))

static int stbi__parse_png_file(stbi__png *z, int scan, int req_comp) {
  unsigned char palette[1024], pal_img_n = 0;
  unsigned char has_trans = 0, tc[3] = {0};
  uint16_t tc16[3];
  uint32_t ioff = 0, idata_limit = 0, i, pal_len = 0;
  int first = 1, k, interlace = 0, color = 0, is_iphone = 0;
  stbi__context *s = z->s;
  z->expanded = NULL;
  z->idata = NULL;
  z->out = NULL;
  if (!stbi__check_png_header(s)) return 0;
  if (scan == STBI__SCAN_type) return 1;
  for (;;) {
    stbi__pngchunk c = stbi__get_chunk_header(s);
    switch (c.type) {

      case STBI__PNG_TYPE('C', 'g', 'B', 'I'):
        is_iphone = 1;
        stbi__skip(s, c.length);
        break;

      case STBI__PNG_TYPE('I', 'H', 'D', 'R'): {
        int comp, filter;
        if (!first) return stbi__err("multiple IHDR", "Corrupt PNG");
        first = 0;
        if (c.length != 13) return stbi__err("bad IHDR len", "Corrupt PNG");
        s->img_x = stbi__get32be(s);
        s->img_y = stbi__get32be(s);
        if (s->img_y > STBI_MAX_DIMENSIONS)
          return stbi__err("too large", "Very large image (corrupt?)");
        if (s->img_x > STBI_MAX_DIMENSIONS)
          return stbi__err("too large", "Very large image (corrupt?)");
        z->depth = stbi__get8(s);
        if (z->depth != 1 && z->depth != 2 && z->depth != 4 && z->depth != 8 &&
            z->depth != 16)
          return stbi__err("1/2/4/8/16-bit only",
                           "PNG not supported: 1/2/4/8/16-bit only");
        color = stbi__get8(s);
        if (color > 6) return stbi__err("bad ctype", "Corrupt PNG");
        if (color == 3 && z->depth == 16)
          return stbi__err("bad ctype", "Corrupt PNG");
        if (color == 3)
          pal_img_n = 3;
        else if (color & 1)
          return stbi__err("bad ctype", "Corrupt PNG");
        comp = stbi__get8(s);
        if (comp) return stbi__err("bad comp method", "Corrupt PNG");
        filter = stbi__get8(s);
        if (filter) return stbi__err("bad filter method", "Corrupt PNG");
        interlace = stbi__get8(s);
        if (interlace > 1)
          return stbi__err("bad interlace method", "Corrupt PNG");
        if (!s->img_x || !s->img_y)
          return stbi__err("0-pixel image", "Corrupt PNG");
        if (!pal_img_n) {
          s->img_n = (color & 2 ? 3 : 1) + (color & 4 ? 1 : 0);
          if ((1 << 30) / s->img_x / s->img_n < s->img_y)
            return stbi__err("too large", "Image too large to decode");
        } else {
          // if paletted, then pal_n is our final components, and
          // img_n is # components to decompress/filter.
          s->img_n = 1;
          if ((1 << 30) / s->img_x / 4 < s->img_y)
            return stbi__err("too large", "Corrupt PNG");
        }
        // even with SCAN_header, have to scan to see if we have a tRNS
        break;
      }

      case STBI__PNG_TYPE('P', 'L', 'T', 'E'): {
        if (first) return stbi__err("first not IHDR", "Corrupt PNG");
        if (c.length > 256 * 3) return stbi__err("invalid PLTE", "Corrupt PNG");
        pal_len = c.length / 3;
        if (pal_len * 3 != c.length)
          return stbi__err("invalid PLTE", "Corrupt PNG");
        for (i = 0; i < pal_len; ++i) {
          palette[i * 4 + 0] = stbi__get8(s);
          palette[i * 4 + 1] = stbi__get8(s);
          palette[i * 4 + 2] = stbi__get8(s);
          palette[i * 4 + 3] = 255;
        }
        break;
      }

      case STBI__PNG_TYPE('t', 'R', 'N', 'S'): {
        if (first) return stbi__err("first not IHDR", "Corrupt PNG");
        if (z->idata) return stbi__err("tRNS after IDAT", "Corrupt PNG");
        if (pal_img_n) {
          if (scan == STBI__SCAN_header) {
            s->img_n = 4;
            return 1;
          }
          if (pal_len == 0) return stbi__err("tRNS before PLTE", "Corrupt PNG");
          if (c.length > pal_len)
            return stbi__err("bad tRNS len", "Corrupt PNG");
          pal_img_n = 4;
          for (i = 0; i < c.length; ++i) palette[i * 4 + 3] = stbi__get8(s);
        } else {
          if (!(s->img_n & 1))
            return stbi__err("tRNS with alpha", "Corrupt PNG");
          if (c.length != (uint32_t)s->img_n * 2)
            return stbi__err("bad tRNS len", "Corrupt PNG");
          has_trans = 1;
          // non-paletted with tRNS = constant alpha.
          // if header-scanning, we can stop now.
          if (scan == STBI__SCAN_header) {
            ++s->img_n;
            return 1;
          }
          if (z->depth == 16) {
            for (k = 0; k < s->img_n; ++k)
              tc16[k] = (uint16_t)stbi__get16be(s);  // copy the values as-is
          } else {
            for (k = 0; k < s->img_n; ++k)
              tc[k] = (unsigned char)(stbi__get16be(s) & 255) *
                      stbi__depth_scale_table[z->depth];  // non 8-bit images
                                                          // will be larger
          }
        }
        break;
      }

      case STBI__PNG_TYPE('I', 'D', 'A', 'T'): {
        if (first) return stbi__err("first not IHDR", "Corrupt PNG");
        if (pal_img_n && !pal_len) return stbi__err("no PLTE", "Corrupt PNG");
        if (scan == STBI__SCAN_header) {
          // header scan definitely stops at first IDAT
          if (pal_img_n) s->img_n = pal_img_n;
          return 1;
        }
        if (c.length > (1u << 30))
          return stbi__err("IDAT size limit",
                           "IDAT section larger than 2^30 bytes");
        if ((int)(ioff + c.length) < (int)ioff) return 0;
        if (ioff + c.length > idata_limit) {
          unsigned char *p;
          if (idata_limit == 0) idata_limit = c.length > 4096 ? c.length : 4096;
          while (ioff + c.length > idata_limit) idata_limit *= 2;
          p = realloc(z->idata, idata_limit);
          if (p == NULL) return stbi__err("outofmem", "Out of memory");
          z->idata = p;
        }
        if (!stbi__getn(s, z->idata + ioff, c.length))
          return stbi__err("outofdata", "Corrupt PNG");
        ioff += c.length;
        break;
      }

      case STBI__PNG_TYPE('I', 'E', 'N', 'D'): {
        uint32_t raw_len, bpl;
        if (first) return stbi__err("first not IHDR", "Corrupt PNG");
        if (scan != STBI__SCAN_load) return 1;
        if (z->idata == NULL) return stbi__err("no IDAT", "Corrupt PNG");
        // initial guess for decoded data size to avoid unnecessary reallocs
        bpl = (s->img_x * z->depth + 7) / 8;  // bytes per line, per component
        raw_len = bpl * s->img_y * s->img_n /* pixels */ +
                  s->img_y /* filter mode per row */;
        z->expanded =
            (unsigned char *)stbi_zlib_decode_malloc_guesssize_headerflag(
                (char *)z->idata, ioff, raw_len, (int *)&raw_len, !is_iphone);
        if (z->expanded == NULL) return 0;  // zlib should set error
        free(z->idata);
        z->idata = NULL;
        if ((req_comp == s->img_n + 1 && req_comp != 3 && !pal_img_n) ||
            has_trans) {
          s->img_out_n = s->img_n + 1;
        } else {
          s->img_out_n = s->img_n;
        }
        if (!stbi__create_png_image(z, z->expanded, raw_len, s->img_out_n,
                                    z->depth, color, interlace))
          return 0;
        if (has_trans) {
          if (z->depth == 16) {
            if (!stbi__compute_transparency16(z, tc16, s->img_out_n)) return 0;
          } else {
            if (!stbi__compute_transparency(z, tc, s->img_out_n)) return 0;
          }
        }
        if (is_iphone && stbi__de_iphone_flag && s->img_out_n > 2)
          stbi__de_iphone(z);
        if (pal_img_n) {
          // pal_img_n == 3 or 4
          s->img_n = pal_img_n;  // record the actual colors we had
          s->img_out_n = pal_img_n;
          if (req_comp >= 3) s->img_out_n = req_comp;
          if (!stbi__expand_png_palette(z, palette, pal_len, s->img_out_n))
            return 0;
        } else if (has_trans) {
          // non-paletted image with tRNS -> source image has (constant) alpha
          ++s->img_n;
        }
        free(z->expanded);
        z->expanded = NULL;
        // end of PNG chunk, read and skip CRC
        stbi__get32be(s);
        return 1;
      }

      default:
        // if critical, fail
        if (first) return stbi__err("first not IHDR", "Corrupt PNG");
        if ((c.type & (1 << 29)) == 0) {
#if !defined(STBI_NO_FAILURE_STRINGS) && !defined(STBI_FAILURE_USERMSG)
          // not threadsafe
          static char invalid_chunk[] = "XXXX PNG chunk not known";
          invalid_chunk[0] = STBI__BYTECAST(c.type >> 24);
          invalid_chunk[1] = STBI__BYTECAST(c.type >> 16);
          invalid_chunk[2] = STBI__BYTECAST(c.type >> 8);
          invalid_chunk[3] = STBI__BYTECAST(c.type >> 0);
#endif
          return stbi__err(invalid_chunk,
                           "PNG not supported: unknown PNG chunk type");
        }
        stbi__skip(s, c.length);
        break;
    }
    // end of PNG chunk, read and skip CRC
    stbi__get32be(s);
  }
}

static void *stbi__do_png(stbi__png *p, int *x, int *y, int *n, int req_comp,
                          stbi__result_info *ri) {
  void *result = NULL;
  if (req_comp < 0 || req_comp > 4)
    return stbi__errpuc("bad req_comp", "Internal error");
  if (stbi__parse_png_file(p, STBI__SCAN_load, req_comp)) {
    if (p->depth <= 8)
      ri->bits_per_channel = 8;
    else if (p->depth == 16)
      ri->bits_per_channel = 16;
    else
      return stbi__errpuc("bad bits_per_channel",
                          "PNG not supported: unsupported color depth");
    result = p->out;
    p->out = NULL;
    if (req_comp && req_comp != p->s->img_out_n) {
      if (ri->bits_per_channel == 8)
        result = stbi__convert_format((unsigned char *)result, p->s->img_out_n,
                                      req_comp, p->s->img_x, p->s->img_y);
      else
        result = stbi__convert_format16((uint16_t *)result, p->s->img_out_n,
                                        req_comp, p->s->img_x, p->s->img_y);
      p->s->img_out_n = req_comp;
      if (result == NULL) return result;
    }
    *x = p->s->img_x;
    *y = p->s->img_y;
    if (n) *n = p->s->img_n;
  }
  free(p->out);
  p->out = NULL;
  free(p->expanded);
  p->expanded = NULL;
  free(p->idata);
  p->idata = NULL;

  return result;
}

static dontinline void *stbi__png_load(stbi__context *s, int *x, int *y,
                                       int *comp, int req_comp,
                                       stbi__result_info *ri) {
  stbi__png p;
  p.s = s;
  return stbi__do_png(&p, x, y, comp, req_comp, ri);
}

static int stbi__png_test(stbi__context *s) {
  int r;
  r = stbi__check_png_header(s);
  stbi__rewind(s);
  return r;
}

static int stbi__png_info_raw(stbi__png *p, int *x, int *y, int *comp) {
  if (!stbi__parse_png_file(p, STBI__SCAN_header, 0)) {
    stbi__rewind(p->s);
    return 0;
  }
  if (x) *x = p->s->img_x;
  if (y) *y = p->s->img_y;
  if (comp) *comp = p->s->img_n;
  return 1;
}

static int stbi__png_info(stbi__context *s, int *x, int *y, int *comp) {
  stbi__png p;
  p.s = s;
  return stbi__png_info_raw(&p, x, y, comp);
}

static int stbi__png_is16(stbi__context *s) {
  stbi__png p;
  p.s = s;
  if (!stbi__png_info_raw(&p, NULL, NULL, NULL)) return 0;
  if (p.depth != 16) {
    stbi__rewind(p.s);
    return 0;
  }
  return 1;
}

// *****************************************************************************
// GIF loader -- public domain by Jean-Marc Lienher -- simplified/shrunk by
// stb

typedef struct {
  int16_t prefix;
  unsigned char first;
  unsigned char suffix;
} stbi__gif_lzw;

typedef struct {
  int w, h;
  unsigned char *out;  // output buffer (always 4 components)
  unsigned char
      *background;  // The current "background" as far as a gif is concerned
  unsigned char *history;
  int flags, bgindex, ratio, transparent, eflags;
  unsigned char pal[256][4];
  unsigned char lpal[256][4];
  stbi__gif_lzw codes[8192];
  unsigned char *color_table;
  int parse, step;
  int lflags;
  int start_x, start_y;
  int max_x, max_y;
  int cur_x, cur_y;
  int line_size;
  int delay;
} stbi__gif;

static int stbi__gif_test_raw(stbi__context *s) {
  int sz;
  if (stbi__get8(s) != 'G' || stbi__get8(s) != 'I' || stbi__get8(s) != 'F' ||
      stbi__get8(s) != '8')
    return 0;
  sz = stbi__get8(s);
  if (sz != '9' && sz != '7') return 0;
  if (stbi__get8(s) != 'a') return 0;
  return 1;
}

static int stbi__gif_test(stbi__context *s) {
  int r = stbi__gif_test_raw(s);
  stbi__rewind(s);
  return r;
}

static void stbi__gif_parse_colortable(stbi__context *s,
                                       unsigned char pal[256][4],
                                       int num_entries, int transp) {
  int i;
  for (i = 0; i < num_entries; ++i) {
    pal[i][2] = stbi__get8(s);
    pal[i][1] = stbi__get8(s);
    pal[i][0] = stbi__get8(s);
    pal[i][3] = transp == i ? 0 : 255;
  }
}

static int stbi__gif_header(stbi__context *s, stbi__gif *g, int *comp,
                            int is_info) {
  unsigned char version;
  if (stbi__get8(s) != 'G' || stbi__get8(s) != 'I' || stbi__get8(s) != 'F' ||
      stbi__get8(s) != '8')
    return stbi__err("not GIF", "Corrupt GIF");
  version = stbi__get8(s);
  if (version != '7' && version != '9') {
    return stbi__err("not GIF", "Corrupt GIF");
  }
  if (stbi__get8(s) != 'a') return stbi__err("not GIF", "Corrupt GIF");
  stbi__g_failure_reason = "";
  g->w = stbi__get16le(s);
  g->h = stbi__get16le(s);
  g->flags = stbi__get8(s);
  g->bgindex = stbi__get8(s);
  g->ratio = stbi__get8(s);
  g->transparent = -1;
  if (g->w > STBI_MAX_DIMENSIONS)
    return stbi__err("too large", "Very large image (corrupt?)");
  if (g->h > STBI_MAX_DIMENSIONS)
    return stbi__err("too large", "Very large image (corrupt?)");
  if (comp != 0) {
    *comp = 4;  // can't actually tell whether it's 3 or 4 until we parse the
                // comments
  }
  if (is_info) return 1;
  if (g->flags & 0x80) {
    stbi__gif_parse_colortable(s, g->pal, 2 << (g->flags & 7), -1);
  }
  return 1;
}

static int stbi__gif_info_raw(stbi__context *s, int *x, int *y, int *comp) {
  stbi__gif *g = (stbi__gif *)malloc(sizeof(stbi__gif));
  if (!g) return stbi__err("outofmem", "Out of memory");
  if (!stbi__gif_header(s, g, comp, 1)) {
    free(g);
    stbi__rewind(s);
    return 0;
  }
  if (x) *x = g->w;
  if (y) *y = g->h;
  free(g);
  return 1;
}

static void stbi__out_gif_code(stbi__gif *g, uint16_t code) {
  unsigned char *p, *c;
  int idx;
  // recurse to decode the prefixes, since the linked-list is backwards,
  // and working backwards through an interleaved image would be nasty
  if (g->codes[code].prefix >= 0) stbi__out_gif_code(g, g->codes[code].prefix);
  if (g->cur_y >= g->max_y) return;
  idx = g->cur_x + g->cur_y;
  p = &g->out[idx];
  g->history[idx / 4] = 1;
  c = &g->color_table[g->codes[code].suffix * 4];
  if (c[3] > 128) {  // don't render transparent pixels;
    p[0] = c[2];
    p[1] = c[1];
    p[2] = c[0];
    p[3] = c[3];
  }
  g->cur_x += 4;
  if (g->cur_x >= g->max_x) {
    g->cur_x = g->start_x;
    g->cur_y += g->step;
    while (g->cur_y >= g->max_y && g->parse > 0) {
      g->step = (1 << g->parse) * g->line_size;
      g->cur_y = g->start_y + (g->step >> 1);
      --g->parse;
    }
  }
}

static unsigned char *stbi__process_gif_raster(stbi__context *s, stbi__gif *g) {
  unsigned char lzw_cs;
  int32_t len, init_code;
  uint32_t first;
  int32_t codesize, codemask, avail, oldcode, bits, valid_bits, clear;
  stbi__gif_lzw *p;

  lzw_cs = stbi__get8(s);
  if (lzw_cs > 12) return NULL;
  clear = 1 << lzw_cs;
  first = 1;
  codesize = lzw_cs + 1;
  codemask = (1 << codesize) - 1;
  bits = 0;
  valid_bits = 0;
  for (init_code = 0; init_code < clear; init_code++) {
    g->codes[init_code].prefix = -1;
    g->codes[init_code].first = (unsigned char)init_code;
    g->codes[init_code].suffix = (unsigned char)init_code;
  }

  // support no starting clear code
  avail = clear + 2;
  oldcode = -1;

  len = 0;
  for (;;) {
    if (valid_bits < codesize) {
      if (len == 0) {
        len = stbi__get8(s);  // start new block
        if (len == 0) return g->out;
      }
      --len;
      bits |= (int32_t)stbi__get8(s) << valid_bits;
      valid_bits += 8;
    } else {
      int32_t code = bits & codemask;
      bits >>= codesize;
      valid_bits -= codesize;
      // @OPTIMIZE: is there some way we can accelerate the non-clear path?
      if (code == clear) {  // clear code
        codesize = lzw_cs + 1;
        codemask = (1 << codesize) - 1;
        avail = clear + 2;
        oldcode = -1;
        first = 0;
      } else if (code == clear + 1) {  // end of stream code
        stbi__skip(s, len);
        while ((len = stbi__get8(s)) > 0) stbi__skip(s, len);
        return g->out;
      } else if (code <= avail) {
        if (first) {
          return stbi__errpuc("no clear code", "Corrupt GIF");
        }

        if (oldcode >= 0) {
          p = &g->codes[avail++];
          if (avail > 8192) {
            return stbi__errpuc("too many codes", "Corrupt GIF");
          }

          p->prefix = (int16_t)oldcode;
          p->first = g->codes[oldcode].first;
          p->suffix = (code == avail) ? p->first : g->codes[code].first;
        } else if (code == avail)
          return stbi__errpuc("illegal code in raster", "Corrupt GIF");

        stbi__out_gif_code(g, (uint16_t)code);

        if ((avail & codemask) == 0 && avail <= 0x0FFF) {
          codesize++;
          codemask = (1 << codesize) - 1;
        }

        oldcode = code;
      } else {
        return stbi__errpuc("illegal code in raster", "Corrupt GIF");
      }
    }
  }
}

// this function is designed to support animated gifs, although stb_image
// doesn't support it two back is the image from two frames ago, used for a
// very specific disposal format
static unsigned char *stbi__gif_load_next(stbi__context *s, stbi__gif *g,
                                          int *comp, int req_comp,
                                          unsigned char *two_back) {
  int dispose;
  int first_frame;
  int pi;
  int pcount;

  // on first frame, any non-written pixels get the background colour
  // (non-transparent)
  first_frame = 0;
  if (g->out == 0) {
    if (!stbi__gif_header(s, g, comp, 0))
      return 0;  // stbi__g_failure_reason set by stbi__gif_header
    if (!stbi__mad3sizes_valid(4, g->w, g->h, 0))
      return stbi__errpuc("too large", "GIF image is too large");
    pcount = g->w * g->h;
    g->out = malloc(4 * pcount);
    g->background = malloc(4 * pcount);
    g->history = malloc(pcount);
    if (!g->out || !g->background || !g->history)
      return stbi__errpuc("outofmem", "Out of memory");

    // image is treated as "transparent" at the start - i.e. nothing overwrites
    // the current background; background colour is only used for pixels that
    // are not rendered first frame, after that "background" color refers to
    // the color that was there the previous frame.
    bzero(g->out, 4 * pcount);
    bzero(g->background,
          4 * pcount);  // state of the background (starts transparent)
    bzero(g->history,
          pcount);  // pixels that were affected previous frame
    first_frame = 1;
  } else {
    // second frame - how do we dispose of the previous one?
    dispose = (g->eflags & 0x1C) >> 2;
    pcount = g->w * g->h;

    if ((dispose == 3) && (two_back == 0)) {
      dispose = 2;  // if I don't have an image to revert back to, default to
                    // the old background
    }

    if (dispose == 3) {  // use previous graphic
      for (pi = 0; pi < pcount; ++pi) {
        if (g->history[pi]) {
          memcpy(&g->out[pi * 4], &two_back[pi * 4], 4);
        }
      }
    } else if (dispose == 2) {
      // restore what was changed last frame to background before that frame;
      for (pi = 0; pi < pcount; ++pi) {
        if (g->history[pi]) {
          memcpy(&g->out[pi * 4], &g->background[pi * 4], 4);
        }
      }
    } else {
      // This is a non-disposal case either way, so just
      // leave the pixels as is, and they will become the new background
      // 1: do not dispose
      // 0: not specified.
    }

    // background is what out is after the undoing of the previou frame;
    memcpy(g->background, g->out, 4 * g->w * g->h);
  }

  // clear my history;
  bzero(g->history,
        g->w * g->h);  // pixels that were affected previous frame

  for (;;) {
    int tag = stbi__get8(s);
    switch (tag) {
      case 0x2C: /* Image Descriptor */
      {
        int32_t x, y, w, h;
        unsigned char *o;

        x = stbi__get16le(s);
        y = stbi__get16le(s);
        w = stbi__get16le(s);
        h = stbi__get16le(s);
        if (((x + w) > (g->w)) || ((y + h) > (g->h)))
          return stbi__errpuc("bad Image Descriptor", "Corrupt GIF");

        g->line_size = g->w * 4;
        g->start_x = x * 4;
        g->start_y = y * g->line_size;
        g->max_x = g->start_x + w * 4;
        g->max_y = g->start_y + h * g->line_size;
        g->cur_x = g->start_x;
        g->cur_y = g->start_y;

        // if the width of the specified rectangle is 0, that means
        // we may not see *any* pixels or the image is malformed;
        // to make sure this is caught, move the current y down to
        // max_y (which is what out_gif_code checks).
        if (w == 0) g->cur_y = g->max_y;

        g->lflags = stbi__get8(s);

        if (g->lflags & 0x40) {
          g->step = 8 * g->line_size;  // first interlaced spacing
          g->parse = 3;
        } else {
          g->step = g->line_size;
          g->parse = 0;
        }

        if (g->lflags & 0x80) {
          stbi__gif_parse_colortable(s, g->lpal, 2 << (g->lflags & 7),
                                     g->eflags & 0x01 ? g->transparent : -1);
          g->color_table = (unsigned char *)g->lpal;
        } else if (g->flags & 0x80) {
          g->color_table = (unsigned char *)g->pal;
        } else
          return stbi__errpuc("missing color table", "Corrupt GIF");

        o = stbi__process_gif_raster(s, g);
        if (!o) return NULL;

        // if this was the first frame,
        pcount = g->w * g->h;
        if (first_frame && (g->bgindex > 0)) {
          // if first frame, any pixel not drawn to gets the background color
          for (pi = 0; pi < pcount; ++pi) {
            if (g->history[pi] == 0) {
              g->pal[g->bgindex][3] =
                  255;  // just in case it was made transparent, undo that; It
                        // will be reset next frame if need be;
              memcpy(&g->out[pi * 4], &g->pal[g->bgindex], 4);
            }
          }
        }

        return o;
      }

      case 0x21:  // Comment Extension.
      {
        int len;
        int ext = stbi__get8(s);
        if (ext == 0xF9) {  // Graphic Control Extension.
          len = stbi__get8(s);
          if (len == 4) {
            g->eflags = stbi__get8(s);
            g->delay =
                10 *
                stbi__get16le(
                    s);  // delay - 1/100th of a second, saving as 1/1000ths.

            // unset old transparent
            if (g->transparent >= 0) {
              g->pal[g->transparent][3] = 255;
            }
            if (g->eflags & 0x01) {
              g->transparent = stbi__get8(s);
              if (g->transparent >= 0) {
                g->pal[g->transparent][3] = 0;
              }
            } else {
              // don't need transparent
              stbi__skip(s, 1);
              g->transparent = -1;
            }
          } else {
            stbi__skip(s, len);
            break;
          }
        }
        while ((len = stbi__get8(s)) != 0) {
          stbi__skip(s, len);
        }
        break;
      }

      case 0x3B:  // gif stream termination code
        return (
            unsigned char *)s;  // using '1' causes warning on some compilers

      default:
        return stbi__errpuc("unknown code", "Corrupt GIF");
    }
  }
}

static void *stbi__load_gif_main_outofmem(stbi__gif *g, unsigned char *out,
                                          int **delays) {
  free(g->out);
  free(g->history);
  free(g->background);
  if (out) free(out);
  if (delays && *delays) free(*delays);
  return stbi__errpuc("outofmem", "Out of memory");
}

static void *stbi__load_gif_main(stbi__context *s, int **delays, int *x, int *y,
                                 int *z, int *comp, int req_comp) {
  if (stbi__gif_test(s)) {
    int layers = 0;
    unsigned char *u = 0;
    unsigned char *out = 0;
    unsigned char *two_back = 0;
    stbi__gif *g;
    int stride;
    g = calloc(1, sizeof(stbi__gif));
    if (delays) {
      *delays = 0;
    }
    do {
      u = stbi__gif_load_next(s, g, comp, req_comp, two_back);
      if (u == (unsigned char *)s) u = 0;  // end of animated gif marker
      if (u) {
        *x = g->w;
        *y = g->h;
        ++layers;
        stride = g->w * g->h * 4;
        if (out) {
          void *tmp = (unsigned char *)realloc(out, layers * stride);
          if (!tmp)
            return stbi__load_gif_main_outofmem(g, out, delays);
          else {
            out = (unsigned char *)tmp;
          }
          if (delays) {
            int *new_delays = (int *)realloc(*delays, sizeof(int) * layers);
            if (!new_delays)
              return stbi__load_gif_main_outofmem(g, out, delays);
            *delays = new_delays;
          }
        } else {
          out = malloc(layers * stride);
          if (!out) return stbi__load_gif_main_outofmem(g, out, delays);
          if (delays) {
            *delays = malloc(layers * sizeof(int));
            if (!*delays) return stbi__load_gif_main_outofmem(g, out, delays);
          }
        }
        memcpy(out + ((layers - 1) * stride), u, stride);
        if (layers >= 2) {
          two_back = out + ((layers - 2) * stride);
        }
        if (delays) {
          (*delays)[layers - 1U] = g->delay;
        }
      }
    } while (u != 0);
    free(g->out);
    free(g->history);
    free(g->background);
    // do the final conversion after loading everything;
    if (req_comp && req_comp != 4)
      out = stbi__convert_format(out, 4, req_comp, layers * g->w, g->h);

    free(g);
    *z = layers;
    return out;
  } else {
    return stbi__errpuc("not GIF", "Image was not as a gif type.");
  }
}

static dontinline void *stbi__gif_load(stbi__context *s, int *x, int *y,
                                       int *comp, int req_comp,
                                       stbi__result_info *ri) {
  unsigned char *u = 0;
  stbi__gif *g;
  g = calloc(1, sizeof(stbi__gif));
  u = stbi__gif_load_next(s, g, comp, req_comp, 0);
  if (u == (unsigned char *)s) u = 0;  // end of animated gif marker
  if (u) {
    *x = g->w;
    *y = g->h;
    // moved conversion to after successful load so that the same
    // can be done for multiple frames.
    if (req_comp && req_comp != 4)
      u = stbi__convert_format(u, 4, req_comp, g->w, g->h);
  } else if (g->out) {
    // if there was an error and we allocated an image buffer, free it!
    free(g->out);
  }
  // free buffers needed for multiple frame loading;
  free(g->history);
  free(g->background);
  free(g);
  return u;
}

static int stbi__gif_info(stbi__context *s, int *x, int *y, int *comp) {
  return stbi__gif_info_raw(s, x, y, comp);
}

// ********************************************************************
// Portable Gray Map and Portable Pixel Map loader
// by Ken Miller
//
// PGM: http://netpbm.sourceforge.net/doc/pgm.html
// PPM: http://netpbm.sourceforge.net/doc/ppm.html
//
// Known limitations:
//    Does not support comments in the header section
//    Does not support ASCII image data (formats P2 and P3)

static int stbi__pnm_test(stbi__context *s) {
  char p, t;
  p = (char)stbi__get8(s);
  t = (char)stbi__get8(s);
  if (p != 'P' || (t != '5' && t != '6')) {
    stbi__rewind(s);
    return 0;
  }
  return 1;
}

static dontinline void *stbi__pnm_load(stbi__context *s, int *x, int *y,
                                       int *comp, int req_comp,
                                       stbi__result_info *ri) {
  unsigned char *out;
  ri->bits_per_channel =
      stbi__pnm_info(s, (int *)&s->img_x, (int *)&s->img_y, (int *)&s->img_n);
  if (ri->bits_per_channel == 0) return 0;
  if (s->img_y > STBI_MAX_DIMENSIONS) {
    return stbi__errpuc("too large", "Very large image (corrupt?)");
  }
  if (s->img_x > STBI_MAX_DIMENSIONS) {
    return stbi__errpuc("too large", "Very large image (corrupt?)");
  }
  *x = s->img_x;
  *y = s->img_y;
  if (comp) *comp = s->img_n;
  if (!stbi__mad4sizes_valid(s->img_n, s->img_x, s->img_y,
                             ri->bits_per_channel / 8, 0)) {
    return stbi__errpuc("too large", "PNM too large");
  }
  out = stbi__malloc_mad4(s->img_n, s->img_x, s->img_y,
                          ri->bits_per_channel / 8, 0);
  if (!stbi__getn(
          s, out,
          s->img_n * s->img_x * s->img_y * (ri->bits_per_channel / 8))) {
    free(out);
    return stbi__errpuc("bad PNM", "PNM file truncated");
  }
  if (req_comp && req_comp != s->img_n) {
    if (ri->bits_per_channel == 16) {
      out = (unsigned char *)stbi__convert_format16(
          (uint16_t *)out, s->img_n, req_comp, s->img_x, s->img_y);
    } else {
      out = stbi__convert_format(out, s->img_n, req_comp, s->img_x, s->img_y);
    }
    if (out == NULL) return out;  // stbi__convert_format frees input on failure
  }
  return out;
}

static int stbi__pnm_isspace(char c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' ||
         c == '\r';
}

static void stbi__pnm_skip_whitespace(stbi__context *s, char *c) {
  for (;;) {
    while (!stbi__at_eof(s) && stbi__pnm_isspace(*c)) *c = (char)stbi__get8(s);
    if (stbi__at_eof(s) || *c != '#') break;
    while (!stbi__at_eof(s) && *c != '\n' && *c != '\r')
      *c = (char)stbi__get8(s);
  }
}

static int stbi__pnm_isdigit(char c) {
  return c >= '0' && c <= '9';
}

static int stbi__pnm_getinteger(stbi__context *s, char *c) {
  int value = 0;
  while (!stbi__at_eof(s) && stbi__pnm_isdigit(*c)) {
    value = value * 10 + (*c - '0');
    *c = (char)stbi__get8(s);
    // TODO INT_MAX
    if ((value > 214748364) || (value == 214748364 && *c > '7')) {
      return stbi__err(
          "integer parse overflow",
          "Parsing an integer in the PPM header overflowed a 32-bit int");
    }
  }
  return value;
}

static int stbi__pnm_info(stbi__context *s, int *x, int *y, int *comp) {
  int maxv, dummy;
  char c, p, t;
  if (!x) x = &dummy;
  if (!y) y = &dummy;
  if (!comp) comp = &dummy;
  stbi__rewind(s);
  // Get identifier
  p = (char)stbi__get8(s);
  t = (char)stbi__get8(s);
  if (p != 'P' || (t != '5' && t != '6')) {
    stbi__rewind(s);
    return 0;
  }
  *comp =
      (t == '6') ? 3 : 1;  // '5' is 1-component .pgm; '6' is 3-component .ppm
  c = (char)stbi__get8(s);
  stbi__pnm_skip_whitespace(s, &c);
  *x = stbi__pnm_getinteger(s, &c);  // read width
  if (*x == 0) {
    return stbi__err("invalid_width",
                     "PPM image header had zero or overflowing width");
  }
  stbi__pnm_skip_whitespace(s, &c);
  *y = stbi__pnm_getinteger(s, &c);  // read height
  if (*y == 0) {
    return stbi__err("invalid height",
                     "PPM image header had zero or overflowing height");
  }
  stbi__pnm_skip_whitespace(s, &c);
  maxv = stbi__pnm_getinteger(s, &c);  // read max value
  if (maxv > 65535)
    return stbi__err("max value > 65535",
                     "PPM image supports only 8-bit and 16-bit images");
  else if (maxv > 255)
    return 16;
  else
    return 8;
}

static int stbi__pnm_is16(stbi__context *s) {
  if (stbi__pnm_info(s, NULL, NULL, NULL) == 16) return 1;
  return 0;
}

static int stbi__info_main(stbi__context *s, int *x, int *y, int *comp) {
#ifndef STBI_NO_JPEG
  if (stbi__jpeg_info(s, x, y, comp)) return 1;
#endif
#ifndef STBI_NO_PNG
  if (stbi__png_info(s, x, y, comp)) return 1;
#endif
#ifndef STBI_NO_GIF
  if (stbi__gif_info(s, x, y, comp)) return 1;
#endif
#ifndef STBI_NO_PNM
  if (stbi__pnm_info(s, x, y, comp)) return 1;
#endif
  return stbi__err("unknown image type",
                   "Image not of any known type, or corrupt");
}

static int stbi__is_16_main(stbi__context *s) {
#ifndef STBI_NO_PNG
  if (stbi__png_is16(s)) return 1;
#endif
#ifndef STBI_NO_PNM
  if (stbi__pnm_is16(s)) return 1;
#endif
  return 0;
}

int stbi_info(char const *filename, int *x, int *y, int *comp) {
  FILE *f = stbi__fopen(filename, "rb");
  int result;
  if (!f) return stbi__err("can't fopen", "Unable to open file");
  result = stbi_info_from_file(f, x, y, comp);
  fclose(f);
  return result;
}

int stbi_info_from_file(FILE *f, int *x, int *y, int *comp) {
  int r;
  stbi__context s;
  long pos = ftell(f);
  if (pos < 0) return stbi__err("bad file", "ftell() failed");
  stbi__start_file(&s, f);
  r = stbi__info_main(&s, x, y, comp);
  if (fseek(f, pos, SEEK_SET)) return stbi__err("bad file", "fseek() failed");
  return r;
}

int stbi_is_16_bit(char const *filename) {
  FILE *f = stbi__fopen(filename, "rb");
  int result;
  if (!f) return stbi__err("can't fopen", "Unable to open file");
  result = stbi_is_16_bit_from_file(f);
  fclose(f);
  return result;
}

int stbi_is_16_bit_from_file(FILE *f) {
  int r;
  stbi__context s;
  long pos = ftell(f);
  if (pos < 0) return stbi__err("bad file", "ftell() failed");
  stbi__start_file(&s, f);
  r = stbi__is_16_main(&s);
  if (fseek(f, pos, SEEK_SET)) return stbi__err("bad file", "fseek() failed");
  return r;
}

int stbi_info_from_memory(unsigned char const *buffer, int len, int *x, int *y,
                          int *comp) {
  stbi__context s;
  stbi__start_mem(&s, buffer, len);
  return stbi__info_main(&s, x, y, comp);
}

int stbi_info_from_callbacks(stbi_io_callbacks const *c, void *user, int *x,
                             int *y, int *comp) {
  stbi__context s;
  stbi__start_callbacks(&s, (stbi_io_callbacks *)c, user);
  return stbi__info_main(&s, x, y, comp);
}

int stbi_is_16_bit_from_memory(unsigned char const *buffer, int len) {
  stbi__context s;
  stbi__start_mem(&s, buffer, len);
  return stbi__is_16_main(&s);
}

int stbi_is_16_bit_from_callbacks(stbi_io_callbacks const *c, void *user) {
  stbi__context s;
  stbi__start_callbacks(&s, (stbi_io_callbacks *)c, user);
  return stbi__is_16_main(&s);
}
