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
#include "third_party/stb/stb_image_write.h"
#include "dsp/core/core.h"
#include "libc/assert.h"
#include "libc/fmt/conv.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/math.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "third_party/zlib/zlib.h"

asm(".ident\t\"\\n\\n\
stb_image_write (Public Domain)\\n\
Credit: Sean Barrett, et al.\\n\
http://nothings.org/stb\"");

#define STBIW_UCHAR(x)                       (unsigned char)((x)&0xff)

#define stbiw__wpng4(o, a, b, c, d)                                           \
  ((o)[0] = STBIW_UCHAR(a), (o)[1] = STBIW_UCHAR(b), (o)[2] = STBIW_UCHAR(c), \
   (o)[3] = STBIW_UCHAR(d), (o) += 4)
#define stbiw__wp32(data, v) \
  stbiw__wpng4(data, (v) >> 24, (v) >> 16, (v) >> 8, (v));
#define stbiw__wptag(data, s) stbiw__wpng4(data, s[0], s[1], s[2], s[3])

typedef struct {
  stbi_write_func *func;
  void *context;
  unsigned char buffer[64];
  int buf_used;
} stbi__write_context;

int stbi_write_png_compression_level = 4;
int stbi_write_tga_with_rle = 1;
int stbi_write_force_png_filter = -1;

static int stbi__flip_vertically_on_write = 0;

void stbi_flip_vertically_on_write(int flag) {
  stbi__flip_vertically_on_write = flag;
}

// initialize a callback-based context
static void stbi__start_write_callbacks(stbi__write_context *s,
                                        stbi_write_func *c, void *context) {
  s->func = c;
  s->context = context;
}

static void stbi__stdio_write(void *context, void *data, int size) {
  fwrite(data, 1, size, (FILE *)context);
}

static int stbi__start_write_file(stbi__write_context *s,
                                  const char *filename) {
  FILE *f = fopen(filename, "wb");
  stbi__start_write_callbacks(s, stbi__stdio_write, (void *)f);
  return f != NULL;
}

static void stbi__end_write_file(stbi__write_context *s) {
  fclose((FILE *)s->context);
}

static void stbiw__writefv(stbi__write_context *s, const char *fmt, va_list v) {
  while (*fmt) {
    switch (*fmt++) {
      case ' ':
        break;
      case '1': {
        unsigned char x = STBIW_UCHAR(va_arg(v, int));
        s->func(s->context, &x, 1);
        break;
      }
      case '2': {
        int x = va_arg(v, int);
        unsigned char b[2];
        b[0] = STBIW_UCHAR(x);
        b[1] = STBIW_UCHAR(x >> 8);
        s->func(s->context, b, 2);
        break;
      }
      case '4': {
        unsigned int x = va_arg(v, int);
        unsigned char b[4];
        b[0] = STBIW_UCHAR(x);
        b[1] = STBIW_UCHAR(x >> 8);
        b[2] = STBIW_UCHAR(x >> 16);
        b[3] = STBIW_UCHAR(x >> 24);
        s->func(s->context, b, 4);
        break;
      }
      default:
        __builtin_unreachable();
    }
  }
}

static void stbiw__writef(stbi__write_context *s, const char *fmt, ...) {
  va_list v;
  va_start(v, fmt);
  stbiw__writefv(s, fmt, v);
  va_end(v);
}

static void stbiw__write_flush(stbi__write_context *s) {
  if (s->buf_used) {
    s->func(s->context, &s->buffer, s->buf_used);
    s->buf_used = 0;
  }
}

static void stbiw__putc(stbi__write_context *s, unsigned char c) {
  s->func(s->context, &c, 1);
}

static void stbiw__write1(stbi__write_context *s, unsigned char a) {
  if ((size_t)s->buf_used + 1 > sizeof(s->buffer)) stbiw__write_flush(s);
  s->buffer[s->buf_used++] = a;
}

static void stbiw__write3(stbi__write_context *s, unsigned char a,
                          unsigned char b, unsigned char c) {
  int n;
  if ((size_t)s->buf_used + 3 > sizeof(s->buffer)) stbiw__write_flush(s);
  n = s->buf_used;
  s->buf_used = n + 3;
  s->buffer[n + 0] = a;
  s->buffer[n + 1] = b;
  s->buffer[n + 2] = c;
}

static void stbiw__write_pixel(stbi__write_context *s, int rgb_dir, int comp,
                               int write_alpha, int expand_mono,
                               unsigned char *d) {
  unsigned char bg[3] = {255, 0, 255}, px[3];
  int k;

  if (write_alpha < 0) stbiw__write1(s, d[comp - 1]);

  switch (comp) {
    case 2:  // 2 pixels = mono + alpha, alpha is written separately, so same as
             // 1-channel case
    case 1:
      if (expand_mono)
        stbiw__write3(s, d[0], d[0], d[0]);  // monochrome bmp
      else
        stbiw__write1(s, d[0]);  // monochrome TGA
      break;
    case 4:
      if (!write_alpha) {
        // composite against pink background
        for (k = 0; k < 3; ++k) px[k] = bg[k] + ((d[k] - bg[k]) * d[3]) / 255;
        stbiw__write3(s, px[1 - rgb_dir], px[1], px[1 + rgb_dir]);
        break;
      }
      /* FALLTHROUGH */
    case 3:
      stbiw__write3(s, d[1 - rgb_dir], d[1], d[1 + rgb_dir]);
      break;
  }
  if (write_alpha > 0) stbiw__write1(s, d[comp - 1]);
}

static void stbiw__write_pixels(stbi__write_context *s, int rgb_dir, int vdir,
                                int x, int y, int comp, void *data,
                                int write_alpha, int scanline_pad,
                                int expand_mono) {
  unsigned int zero = 0;
  int i, j, j_end;
  if (y <= 0) return;
  if (stbi__flip_vertically_on_write) vdir *= -1;
  if (vdir < 0) {
    j_end = -1;
    j = y - 1;
  } else {
    j_end = y;
    j = 0;
  }
  for (; j != j_end; j += vdir) {
    for (i = 0; i < x; ++i) {
      unsigned char *d = (unsigned char *)data + (j * x + i) * comp;
      stbiw__write_pixel(s, rgb_dir, comp, write_alpha, expand_mono, d);
    }
    stbiw__write_flush(s);
    s->func(s->context, &zero, scanline_pad);
  }
}

static int stbiw__outfile(stbi__write_context *s, int rgb_dir, int vdir, int x,
                          int y, int comp, int expand_mono, void *data,
                          int alpha, int pad, const char *fmt, ...) {
  if (y < 0 || x < 0) {
    return 0;
  } else {
    va_list v;
    va_start(v, fmt);
    stbiw__writefv(s, fmt, v);
    va_end(v);
    stbiw__write_pixels(s, rgb_dir, vdir, x, y, comp, data, alpha, pad,
                        expand_mono);
    return 1;
  }
}

static int stbi_write_bmp_core(stbi__write_context *s, int x, int y, int comp,
                               const void *data) {
  if (comp != 4) {
    // write RGB bitmap
    int pad;
    pad = (-x * 3) & 3;
    return stbiw__outfile(s, -1, -1, x, y, comp, 1, (void *)data, 0, pad,
                          "11 4 22 4"
                          "4 44 22 444444",
                          'B', 'M', 14 + 40 + (x * 3 + pad) * y, 0, 0,
                          14 + 40,                             // file header
                          40, x, y, 1, 24, 0, 0, 0, 0, 0, 0);  // bitmap header
  } else {
    // RGBA bitmaps need a v4 header
    // use BI_BITFIELDS mode with 32bpp and alpha mask
    // (straight BI_RGB with alpha mask doesn't work in most readers)
    return stbiw__outfile(s, -1, -1, x, y, comp, 1, (void *)data, 1, 0,
                          "11 4 22 4"
                          "4 44 22 444444 4444 4 444 444 444 444",
                          'B', 'M', 14 + 108 + x * y * 4, 0, 0,
                          14 + 108,  // file header
                          108, x, y, 1, 32, 3, 0, 0, 0, 0, 0, 0xff0000, 0xff00,
                          0xff, 0xff000000u, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0);  // bitmap V4 header
  }
}

int stbi_write_bmp_to_func(stbi_write_func *func, void *context, int x, int y,
                           int comp, const void *data) {
  stbi__write_context s = {0};
  stbi__start_write_callbacks(&s, func, context);
  return stbi_write_bmp_core(&s, x, y, comp, data);
}

int stbi_write_bmp(char const *filename, int x, int y, int comp,
                   const void *data) {
  stbi__write_context s = {0};
  if (stbi__start_write_file(&s, filename)) {
    int r = stbi_write_bmp_core(&s, x, y, comp, data);
    stbi__end_write_file(&s);
    return r;
  } else
    return 0;
}

static int stbi_write_tga_core(stbi__write_context *s, int x, int y, int comp,
                               void *data) {
  int has_alpha = (comp == 2 || comp == 4);
  int colorbytes = has_alpha ? comp - 1 : comp;
  int format =
      colorbytes < 2
          ? 3
          : 2;  // 3 color channels (RGB/RGBA) = 2, 1 color channel (Y/YA) = 3

  if (y < 0 || x < 0) return 0;

  if (!stbi_write_tga_with_rle) {
    return stbiw__outfile(s, -1, -1, x, y, comp, 0, (void *)data, has_alpha, 0,
                          "111 221 2222 11", 0, 0, format, 0, 0, 0, 0, 0, x, y,
                          (colorbytes + has_alpha) * 8, has_alpha * 8);
  } else {
    int i, j, k;
    int jend, jdir;

    stbiw__writef(s, "111 221 2222 11", 0, 0, format + 8, 0, 0, 0, 0, 0, x, y,
                  (colorbytes + has_alpha) * 8, has_alpha * 8);

    if (stbi__flip_vertically_on_write) {
      j = 0;
      jend = y;
      jdir = 1;
    } else {
      j = y - 1;
      jend = -1;
      jdir = -1;
    }
    for (; j != jend; j += jdir) {
      unsigned char *row = (unsigned char *)data + j * x * comp;
      int len;

      for (i = 0; i < x; i += len) {
        unsigned char *begin = row + i * comp;
        int diff = 1;
        len = 1;

        if (i < x - 1) {
          ++len;
          diff = memcmp(begin, row + (i + 1) * comp, comp);
          if (diff) {
            const unsigned char *prev = begin;
            for (k = i + 2; k < x && len < 128; ++k) {
              if (memcmp(prev, row + k * comp, comp)) {
                prev += comp;
                ++len;
              } else {
                --len;
                break;
              }
            }
          } else {
            for (k = i + 2; k < x && len < 128; ++k) {
              if (!memcmp(begin, row + k * comp, comp)) {
                ++len;
              } else {
                break;
              }
            }
          }
        }

        if (diff) {
          unsigned char header = STBIW_UCHAR(len - 1);
          stbiw__write1(s, header);
          for (k = 0; k < len; ++k) {
            stbiw__write_pixel(s, -1, comp, has_alpha, 0, begin + k * comp);
          }
        } else {
          unsigned char header = STBIW_UCHAR(len - 129);
          stbiw__write1(s, header);
          stbiw__write_pixel(s, -1, comp, has_alpha, 0, begin);
        }
      }
    }
    stbiw__write_flush(s);
  }
  return 1;
}

int stbi_write_tga_to_func(stbi_write_func *func, void *context, int x, int y,
                           int comp, const void *data) {
  stbi__write_context s = {0};
  stbi__start_write_callbacks(&s, func, context);
  return stbi_write_tga_core(&s, x, y, comp, (void *)data);
}

int stbi_write_tga(char const *filename, int x, int y, int comp,
                   const void *data) {
  stbi__write_context s = {0};
  if (stbi__start_write_file(&s, filename)) {
    int r = stbi_write_tga_core(&s, x, y, comp, (void *)data);
    stbi__end_write_file(&s);
    return r;
  } else
    return 0;
}

/*
 * PNG writer
 */

static unsigned char *stbi_zlib_compress(unsigned char *data, int size,
                                         int *out_len, int quality) {
  unsigned long newsize;
  unsigned char *newdata, *trimdata;
  assert(0 <= size && size <= INT_MAX);
  if ((newdata = malloc((newsize = compressBound(size)))) &&
      compress2(newdata, &newsize, data, size,
                stbi_write_png_compression_level) == Z_OK) {
    *out_len = newsize;
    if ((trimdata = realloc(newdata, newsize))) {
      return trimdata;
    } else {
      return newdata;
    }
  }
  free(newdata);
  return NULL;
}

static void stbiw__wpcrc(unsigned char **data, int len) {
  unsigned int crc = crc32(0, *data - len - 4, len + 4);
  stbiw__wp32(*data, crc);
}

forceinline unsigned char stbiw__paeth(int a, int b, int c) {
  int p = a + b - c, pa = abs(p - a), pb = abs(p - b), pc = abs(p - c);
  if (pa <= pb && pa <= pc) return STBIW_UCHAR(a);
  if (pb <= pc) return STBIW_UCHAR(b);
  return STBIW_UCHAR(c);
}

// @OPTIMIZE: provide an option that always forces left-predict or paeth predict
static void stbiw__encode_png_line(const unsigned char *pixels,
                                   int stride_bytes, int width, int height,
                                   int y, int n, int filter_type,
                                   signed char *line_buffer) {
  int mapping[] = {0, 1, 2, 3, 4};
  int firstmap[] = {0, 1, 0, 5, 6};
  const unsigned char *z;
  int *mymap, i, type, signed_stride;

  mymap = (y != 0) ? mapping : firstmap;
  type = mymap[filter_type];
  z = pixels +
      stride_bytes * (stbi__flip_vertically_on_write ? height - 1 - y : y);
  signed_stride = stbi__flip_vertically_on_write ? -stride_bytes : stride_bytes;

  if (type == 0) {
    memcpy(line_buffer, z, width * n);
    return;
  }

  for (i = 0; i < n; ++i) {
    switch (type) {
      case 1:
        line_buffer[i] = z[i];
        break;
      case 2:
        line_buffer[i] = z[i] - z[i - signed_stride];
        break;
      case 3:
        line_buffer[i] = z[i] - (z[i - signed_stride] >> 1);
        break;
      case 4:
        line_buffer[i] =
            (signed char)(z[i] - stbiw__paeth(0, z[i - signed_stride], 0));
        break;
      case 5:
        line_buffer[i] = z[i];
        break;
      case 6:
        line_buffer[i] = z[i];
        break;
    }
  }

  switch (type) {
    case 1:
      for (i = n; i < width * n; ++i) {
        line_buffer[i] = z[i] - z[i - n];
      }
      break;
    case 2:
      for (i = n; i < width * n; ++i) {
        line_buffer[i] = z[i] - z[i - signed_stride];
      }
      break;
    case 3:
      for (i = n; i < width * n; ++i) {
        line_buffer[i] = z[i] - ((z[i - n] + z[i - signed_stride]) >> 1);
      }
      break;
    case 4:
      for (i = n; i < width * n; ++i) {
        line_buffer[i] = z[i] - stbiw__paeth(z[i - n], z[i - signed_stride],
                                             z[i - signed_stride - n]);
      }
      break;
    case 5:
      for (i = n; i < width * n; ++i) {
        line_buffer[i] = z[i] - (z[i - n] >> 1);
      }
      break;
    case 6:
      for (i = n; i < width * n; ++i) {
        line_buffer[i] = z[i] - stbiw__paeth(z[i - n], 0, 0);
      }
      break;
  }
}

unsigned char *stbi_write_png_to_mem(const unsigned char *pixels,
                                     int stride_bytes, int x, int y, int n,
                                     int *out_len) {
  int force_filter = stbi_write_force_png_filter;
  int ctype[5] = {-1, 0, 4, 2, 6};
  unsigned char sig[8] = {137, 80, 78, 71, 13, 10, 26, 10};
  unsigned char *out, *o, *filt, *zlib;
  signed char *line_buffer;
  int j, zlen;

  if (stride_bytes == 0) stride_bytes = x * n;

  if (force_filter >= 5) {
    force_filter = -1;
  }

  filt = malloc((x * n + 1) * y);
  if (!filt) return 0;
  line_buffer = malloc(x * n);
  if (!line_buffer) {
    free(filt);
    return 0;
  }
  for (j = 0; j < y; ++j) {
    int filter_type;
    if (force_filter > -1) {
      filter_type = force_filter;
      stbiw__encode_png_line(pixels, stride_bytes, x, y, j, n, force_filter,
                             line_buffer);
    } else {  // Estimate the best filter by running through all of them:
      int best_filter = 0, best_filter_val = 0x7fffffff, est, i;
      for (filter_type = 0; filter_type < 5; filter_type++) {
        stbiw__encode_png_line(pixels, stride_bytes, x, y, j, n, filter_type,
                               line_buffer);

        // Estimate the entropy of the line using this filter; the less, the
        // better.
        est = 0;
        for (i = 0; i < x * n; ++i) {
          est += abs((signed char)line_buffer[i]);
        }
        if (est < best_filter_val) {
          best_filter_val = est;
          best_filter = filter_type;
        }
      }
      if (filter_type != best_filter) {  // If the last iteration already got us
                                         // the best filter, don't redo it
        stbiw__encode_png_line(pixels, stride_bytes, x, y, j, n, best_filter,
                               line_buffer);
        filter_type = best_filter;
      }
    }
    // when we get here, filter_type contains the filter type, and line_buffer
    // contains the data
    filt[j * (x * n + 1)] = (unsigned char)filter_type;
    memmove(filt + j * (x * n + 1) + 1, line_buffer, x * n);
  }
  free(line_buffer);
  zlib = stbi_zlib_compress(filt, y * (x * n + 1), &zlen,
                            stbi_write_png_compression_level);
  free(filt);
  if (!zlib) return 0;

  // each tag requires 12 bytes of overhead
  out = malloc(8 + 12 + 13 + 12 + zlen + 12);
  if (!out) return 0;
  *out_len = 8 + 12 + 13 + 12 + zlen + 12;

  o = out;
  memmove(o, sig, 8);
  o += 8;
  stbiw__wp32(o, 13);  // header length
  stbiw__wptag(o, "IHDR");
  stbiw__wp32(o, x);
  stbiw__wp32(o, y);
  *o++ = 8;
  *o++ = STBIW_UCHAR(ctype[n]);
  *o++ = 0;
  *o++ = 0;
  *o++ = 0;
  stbiw__wpcrc(&o, 13);

  stbiw__wp32(o, zlen);
  stbiw__wptag(o, "IDAT");
  memmove(o, zlib, zlen);
  o += zlen;
  free(zlib);
  stbiw__wpcrc(&o, zlen);

  stbiw__wp32(o, 0);
  stbiw__wptag(o, "IEND");
  stbiw__wpcrc(&o, 0);

  assert(o == out + *out_len);

  return out;
}

int stbi_write_png(const char *filename, int x, int y, int comp,
                   const void *data, int stride_bytes) {
  int len;
  FILE *f;
  unsigned char *png;
  png = stbi_write_png_to_mem(data, stride_bytes, x, y, comp, &len);
  if (png == NULL) return 0;
  f = fopen(filename, "wb");
  if (!f) {
    free(png);
    return 0;
  }
  fwrite(png, 1, len, f);
  fclose(f);
  free(png);
  return 1;
}

int stbi_write_png_to_func(stbi_write_func *func, void *context, int x, int y,
                           int comp, const void *data, int stride_bytes) {
  int len;
  unsigned char *png;
  png = stbi_write_png_to_mem((const unsigned char *)data, stride_bytes, x, y,
                              comp, &len);
  if (png == NULL) return 0;
  func(context, png, len);
  free(png);
  return 1;
}

/* JPEG writer
 *
 * This is based on Jon Olick's jo_jpeg.cpp:
 * public domain Simple, Minimalistic JPEG writer -
 * http://www.jonolick.com/code.html
 */

static const unsigned char stbiw__jpg_ZigZag[] = {
    0,  1,  5,  6,  14, 15, 27, 28, 2,  4,  7,  13, 16, 26, 29, 42,
    3,  8,  12, 17, 25, 30, 41, 43, 9,  11, 18, 24, 31, 40, 44, 53,
    10, 19, 23, 32, 39, 45, 52, 54, 20, 22, 33, 38, 46, 51, 55, 60,
    21, 34, 37, 47, 50, 56, 59, 61, 35, 36, 48, 49, 57, 58, 62, 63,
};

static void stbiw__jpg_writeBits(stbi__write_context *s, int *bitBufP,
                                 int *bitCntP, const unsigned short *bs) {
  unsigned char c;
  unsigned bitBuf, bitCnt;
  bitBuf = *bitBufP;
  bitCnt = *bitCntP;
  bitCnt += bs[1];
  bitBuf |= bs[0] << (24 - bitCnt);
  while (bitCnt >= 8) {
    c = (bitBuf >> 16) & 255;
    stbiw__putc(s, c);
    if (c == 255) {
      stbiw__putc(s, 0);
    }
    bitBuf <<= 8;
    bitCnt -= 8;
  }
  *bitBufP = bitBuf;
  *bitCntP = bitCnt;
}

static void stbiw__jpg_calcBits(int val, unsigned short bits[2]) {
  int tmp1 = val < 0 ? -val : val;
  val = val < 0 ? val - 1 : val;
  bits[1] = 1;
  while (tmp1 >>= 1) {
    ++bits[1];
  }
  bits[0] = val & ((1u << bits[1]) - 1);
}

static int stbiw__jpg_processDU(stbi__write_context *s, int *bitBuf,
                                int *bitCnt, float *CDU, unsigned du_stride,
                                float *fdtbl, int DC,
                                const unsigned short HTDC[256][2],
                                const unsigned short HTAC[256][2]) {
  const unsigned short EOB[2] = {HTAC[0x00][0], HTAC[0x00][1]};
  const unsigned short M16zeroes[2] = {HTAC[0xF0][0], HTAC[0xF0][1]};
  unsigned i, j, diff, end0pos, x, y;
  int DU[64];

  dctjpeg((void *)CDU, du_stride / 8);

  // Quantize/descale/zigzag the coefficients
  for (j = 0, y = 0; y < 8; ++y) {
    for (x = 0; x < 8; ++x, ++j) {
      float v;
      i = y * du_stride + x;
      v = CDU[i] * fdtbl[j];
      DU[stbiw__jpg_ZigZag[j]] = v < 0 ? ceilf(v - 0.5f) : floorf(v + 0.5f);
    }
  }

  // Encode DC
  diff = DU[0] - DC;
  if (diff == 0) {
    stbiw__jpg_writeBits(s, bitBuf, bitCnt, HTDC[0]);
  } else {
    unsigned short bits[2];
    stbiw__jpg_calcBits(diff, bits);
    stbiw__jpg_writeBits(s, bitBuf, bitCnt, HTDC[bits[1]]);
    stbiw__jpg_writeBits(s, bitBuf, bitCnt, bits);
  }
  // Encode ACs
  end0pos = 63;
  for (; (end0pos > 0) && (DU[end0pos] == 0); --end0pos) {
  }
  // end0pos = first element in reverse order !=0
  if (end0pos == 0) {
    stbiw__jpg_writeBits(s, bitBuf, bitCnt, EOB);
    return DU[0];
  }
  for (i = 1; i <= end0pos; ++i) {
    unsigned startpos = i;
    unsigned nrzeroes;
    unsigned short bits[2];
    for (; DU[i] == 0 && i <= end0pos; ++i) {
    }
    nrzeroes = i - startpos;
    if (nrzeroes >= 16) {
      unsigned lng = nrzeroes >> 4;
      unsigned nrmarker;
      for (nrmarker = 1; nrmarker <= lng; ++nrmarker) {
        stbiw__jpg_writeBits(s, bitBuf, bitCnt, M16zeroes);
      }
      nrzeroes &= 15;
    }
    stbiw__jpg_calcBits(DU[i], bits);
    stbiw__jpg_writeBits(s, bitBuf, bitCnt, HTAC[(nrzeroes << 4) + bits[1]]);
    stbiw__jpg_writeBits(s, bitBuf, bitCnt, bits);
  }
  if (end0pos != 63) {
    stbiw__jpg_writeBits(s, bitBuf, bitCnt, EOB);
  }
  return DU[0];
}

static int stbi_write_jpg_core(stbi__write_context *s, int width, int height,
                               int comp, const void *data, int quality) {
  static const unsigned char std_dc_luminance_nrcodes[] = {
      0, 0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0};
  static const unsigned char std_dc_luminance_values[] = {0, 1, 2, 3, 4,  5,
                                                          6, 7, 8, 9, 10, 11};
  static const unsigned char std_ac_luminance_nrcodes[] = {
      0, 0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7d};
  static const unsigned char std_ac_luminance_values[] = {
      0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06,
      0x13, 0x51, 0x61, 0x07, 0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08,
      0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0, 0x24, 0x33, 0x62, 0x72,
      0x82, 0x09, 0x0a, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
      0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45,
      0x46, 0x47, 0x48, 0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
      0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75,
      0x76, 0x77, 0x78, 0x79, 0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
      0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3,
      0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
      0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9,
      0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
      0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf1, 0xf2, 0xf3, 0xf4,
      0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa};
  static const unsigned char std_dc_chrominance_nrcodes[] = {
      0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0};
  static const unsigned char std_dc_chrominance_values[] = {0, 1, 2, 3, 4,  5,
                                                            6, 7, 8, 9, 10, 11};
  static const unsigned char std_ac_chrominance_nrcodes[] = {
      0, 0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 0x77};
  static const unsigned char std_ac_chrominance_values[] = {
      0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 0x06, 0x12, 0x41,
      0x51, 0x07, 0x61, 0x71, 0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91,
      0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0, 0x15, 0x62, 0x72, 0xd1,
      0x0a, 0x16, 0x24, 0x34, 0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
      0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44,
      0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
      0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x73, 0x74,
      0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
      0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a,
      0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4,
      0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
      0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
      0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf2, 0xf3, 0xf4,
      0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa};
  // Huffman tables
  static const unsigned short YDC_HT[256][2] = {
      {0, 2},  {2, 3},  {3, 3},  {4, 3},   {5, 3},   {6, 3},
      {14, 4}, {30, 5}, {62, 6}, {126, 7}, {254, 8}, {510, 9}};
  static const unsigned short UVDC_HT[256][2] = {
      {0, 2},  {1, 2},   {2, 2},   {6, 3},   {14, 4},    {30, 5},
      {62, 6}, {126, 7}, {254, 8}, {510, 9}, {1022, 10}, {2046, 11}};
  static const unsigned short YAC_HT[256][2] = {
      {10, 4},     {0, 2},      {1, 2},      {4, 3},      {11, 4},
      {26, 5},     {120, 7},    {248, 8},    {1014, 10},  {65410, 16},
      {65411, 16}, {0, 0},      {0, 0},      {0, 0},      {0, 0},
      {0, 0},      {0, 0},      {12, 4},     {27, 5},     {121, 7},
      {502, 9},    {2038, 11},  {65412, 16}, {65413, 16}, {65414, 16},
      {65415, 16}, {65416, 16}, {0, 0},      {0, 0},      {0, 0},
      {0, 0},      {0, 0},      {0, 0},      {28, 5},     {249, 8},
      {1015, 10},  {4084, 12},  {65417, 16}, {65418, 16}, {65419, 16},
      {65420, 16}, {65421, 16}, {65422, 16}, {0, 0},      {0, 0},
      {0, 0},      {0, 0},      {0, 0},      {0, 0},      {58, 6},
      {503, 9},    {4085, 12},  {65423, 16}, {65424, 16}, {65425, 16},
      {65426, 16}, {65427, 16}, {65428, 16}, {65429, 16}, {0, 0},
      {0, 0},      {0, 0},      {0, 0},      {0, 0},      {0, 0},
      {59, 6},     {1016, 10},  {65430, 16}, {65431, 16}, {65432, 16},
      {65433, 16}, {65434, 16}, {65435, 16}, {65436, 16}, {65437, 16},
      {0, 0},      {0, 0},      {0, 0},      {0, 0},      {0, 0},
      {0, 0},      {122, 7},    {2039, 11},  {65438, 16}, {65439, 16},
      {65440, 16}, {65441, 16}, {65442, 16}, {65443, 16}, {65444, 16},
      {65445, 16}, {0, 0},      {0, 0},      {0, 0},      {0, 0},
      {0, 0},      {0, 0},      {123, 7},    {4086, 12},  {65446, 16},
      {65447, 16}, {65448, 16}, {65449, 16}, {65450, 16}, {65451, 16},
      {65452, 16}, {65453, 16}, {0, 0},      {0, 0},      {0, 0},
      {0, 0},      {0, 0},      {0, 0},      {250, 8},    {4087, 12},
      {65454, 16}, {65455, 16}, {65456, 16}, {65457, 16}, {65458, 16},
      {65459, 16}, {65460, 16}, {65461, 16}, {0, 0},      {0, 0},
      {0, 0},      {0, 0},      {0, 0},      {0, 0},      {504, 9},
      {32704, 15}, {65462, 16}, {65463, 16}, {65464, 16}, {65465, 16},
      {65466, 16}, {65467, 16}, {65468, 16}, {65469, 16}, {0, 0},
      {0, 0},      {0, 0},      {0, 0},      {0, 0},      {0, 0},
      {505, 9},    {65470, 16}, {65471, 16}, {65472, 16}, {65473, 16},
      {65474, 16}, {65475, 16}, {65476, 16}, {65477, 16}, {65478, 16},
      {0, 0},      {0, 0},      {0, 0},      {0, 0},      {0, 0},
      {0, 0},      {506, 9},    {65479, 16}, {65480, 16}, {65481, 16},
      {65482, 16}, {65483, 16}, {65484, 16}, {65485, 16}, {65486, 16},
      {65487, 16}, {0, 0},      {0, 0},      {0, 0},      {0, 0},
      {0, 0},      {0, 0},      {1017, 10},  {65488, 16}, {65489, 16},
      {65490, 16}, {65491, 16}, {65492, 16}, {65493, 16}, {65494, 16},
      {65495, 16}, {65496, 16}, {0, 0},      {0, 0},      {0, 0},
      {0, 0},      {0, 0},      {0, 0},      {1018, 10},  {65497, 16},
      {65498, 16}, {65499, 16}, {65500, 16}, {65501, 16}, {65502, 16},
      {65503, 16}, {65504, 16}, {65505, 16}, {0, 0},      {0, 0},
      {0, 0},      {0, 0},      {0, 0},      {0, 0},      {2040, 11},
      {65506, 16}, {65507, 16}, {65508, 16}, {65509, 16}, {65510, 16},
      {65511, 16}, {65512, 16}, {65513, 16}, {65514, 16}, {0, 0},
      {0, 0},      {0, 0},      {0, 0},      {0, 0},      {0, 0},
      {65515, 16}, {65516, 16}, {65517, 16}, {65518, 16}, {65519, 16},
      {65520, 16}, {65521, 16}, {65522, 16}, {65523, 16}, {65524, 16},
      {0, 0},      {0, 0},      {0, 0},      {0, 0},      {0, 0},
      {2041, 11},  {65525, 16}, {65526, 16}, {65527, 16}, {65528, 16},
      {65529, 16}, {65530, 16}, {65531, 16}, {65532, 16}, {65533, 16},
      {65534, 16}, {0, 0},      {0, 0},      {0, 0},      {0, 0},
      {0, 0}};
  static const unsigned short UVAC_HT[256][2] = {
      {0, 2},      {1, 2},      {4, 3},      {10, 4},     {24, 5},
      {25, 5},     {56, 6},     {120, 7},    {500, 9},    {1014, 10},
      {4084, 12},  {0, 0},      {0, 0},      {0, 0},      {0, 0},
      {0, 0},      {0, 0},      {11, 4},     {57, 6},     {246, 8},
      {501, 9},    {2038, 11},  {4085, 12},  {65416, 16}, {65417, 16},
      {65418, 16}, {65419, 16}, {0, 0},      {0, 0},      {0, 0},
      {0, 0},      {0, 0},      {0, 0},      {26, 5},     {247, 8},
      {1015, 10},  {4086, 12},  {32706, 15}, {65420, 16}, {65421, 16},
      {65422, 16}, {65423, 16}, {65424, 16}, {0, 0},      {0, 0},
      {0, 0},      {0, 0},      {0, 0},      {0, 0},      {27, 5},
      {248, 8},    {1016, 10},  {4087, 12},  {65425, 16}, {65426, 16},
      {65427, 16}, {65428, 16}, {65429, 16}, {65430, 16}, {0, 0},
      {0, 0},      {0, 0},      {0, 0},      {0, 0},      {0, 0},
      {58, 6},     {502, 9},    {65431, 16}, {65432, 16}, {65433, 16},
      {65434, 16}, {65435, 16}, {65436, 16}, {65437, 16}, {65438, 16},
      {0, 0},      {0, 0},      {0, 0},      {0, 0},      {0, 0},
      {0, 0},      {59, 6},     {1017, 10},  {65439, 16}, {65440, 16},
      {65441, 16}, {65442, 16}, {65443, 16}, {65444, 16}, {65445, 16},
      {65446, 16}, {0, 0},      {0, 0},      {0, 0},      {0, 0},
      {0, 0},      {0, 0},      {121, 7},    {2039, 11},  {65447, 16},
      {65448, 16}, {65449, 16}, {65450, 16}, {65451, 16}, {65452, 16},
      {65453, 16}, {65454, 16}, {0, 0},      {0, 0},      {0, 0},
      {0, 0},      {0, 0},      {0, 0},      {122, 7},    {2040, 11},
      {65455, 16}, {65456, 16}, {65457, 16}, {65458, 16}, {65459, 16},
      {65460, 16}, {65461, 16}, {65462, 16}, {0, 0},      {0, 0},
      {0, 0},      {0, 0},      {0, 0},      {0, 0},      {249, 8},
      {65463, 16}, {65464, 16}, {65465, 16}, {65466, 16}, {65467, 16},
      {65468, 16}, {65469, 16}, {65470, 16}, {65471, 16}, {0, 0},
      {0, 0},      {0, 0},      {0, 0},      {0, 0},      {0, 0},
      {503, 9},    {65472, 16}, {65473, 16}, {65474, 16}, {65475, 16},
      {65476, 16}, {65477, 16}, {65478, 16}, {65479, 16}, {65480, 16},
      {0, 0},      {0, 0},      {0, 0},      {0, 0},      {0, 0},
      {0, 0},      {504, 9},    {65481, 16}, {65482, 16}, {65483, 16},
      {65484, 16}, {65485, 16}, {65486, 16}, {65487, 16}, {65488, 16},
      {65489, 16}, {0, 0},      {0, 0},      {0, 0},      {0, 0},
      {0, 0},      {0, 0},      {505, 9},    {65490, 16}, {65491, 16},
      {65492, 16}, {65493, 16}, {65494, 16}, {65495, 16}, {65496, 16},
      {65497, 16}, {65498, 16}, {0, 0},      {0, 0},      {0, 0},
      {0, 0},      {0, 0},      {0, 0},      {506, 9},    {65499, 16},
      {65500, 16}, {65501, 16}, {65502, 16}, {65503, 16}, {65504, 16},
      {65505, 16}, {65506, 16}, {65507, 16}, {0, 0},      {0, 0},
      {0, 0},      {0, 0},      {0, 0},      {0, 0},      {2041, 11},
      {65508, 16}, {65509, 16}, {65510, 16}, {65511, 16}, {65512, 16},
      {65513, 16}, {65514, 16}, {65515, 16}, {65516, 16}, {0, 0},
      {0, 0},      {0, 0},      {0, 0},      {0, 0},      {0, 0},
      {16352, 14}, {65517, 16}, {65518, 16}, {65519, 16}, {65520, 16},
      {65521, 16}, {65522, 16}, {65523, 16}, {65524, 16}, {65525, 16},
      {0, 0},      {0, 0},      {0, 0},      {0, 0},      {0, 0},
      {1018, 10},  {32707, 15}, {65526, 16}, {65527, 16}, {65528, 16},
      {65529, 16}, {65530, 16}, {65531, 16}, {65532, 16}, {65533, 16},
      {65534, 16}, {0, 0},      {0, 0},      {0, 0},      {0, 0},
      {0, 0}};
  static const int YQT[] = {
      16, 11, 10, 16, 24,  40,  51,  61,  12, 12, 14, 19, 26,  58,  60,  55,
      14, 13, 16, 24, 40,  57,  69,  56,  14, 17, 22, 29, 51,  87,  80,  62,
      18, 22, 37, 56, 68,  109, 103, 77,  24, 35, 55, 64, 81,  104, 113, 92,
      49, 64, 78, 87, 103, 121, 120, 101, 72, 92, 95, 98, 112, 100, 103, 99};
  static const int UVQT[] = {17, 18, 24, 47, 99, 99, 99, 99, 18, 21, 26, 66, 99,
                             99, 99, 99, 24, 26, 56, 99, 99, 99, 99, 99, 47, 66,
                             99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
                             99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
                             99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99};
  static const float aasf[] = {
      1.0f * 2.828427125f,         1.387039845f * 2.828427125f,
      1.306562965f * 2.828427125f, 1.175875602f * 2.828427125f,
      1.0f * 2.828427125f,         0.785694958f * 2.828427125f,
      0.541196100f * 2.828427125f, 0.275899379f * 2.828427125f};

  int row, col, i, k, subsample;
  float fdtbl_Y[64], fdtbl_UV[64];
  unsigned char YTable[64], UVTable[64];

  if (!data || !width || !height || comp > 4 || comp < 1) {
    return 0;
  }

  quality = quality ? quality : 97;
  subsample = quality <= 97 ? 1 : 0;
  quality = quality < 1 ? 1 : quality > 100 ? 100 : quality;
  quality = quality < 50 ? 5000 / quality : 200 - quality * 2;

  for (i = 0; i < 64; ++i) {
    int uvti, yti = (YQT[i] * quality + 50) / 100;
    YTable[stbiw__jpg_ZigZag[i]] = (unsigned char)(yti < 1     ? 1
                                                   : yti > 255 ? 255
                                                               : yti);
    uvti = (UVQT[i] * quality + 50) / 100;
    UVTable[stbiw__jpg_ZigZag[i]] = (unsigned char)(uvti < 1     ? 1
                                                    : uvti > 255 ? 255
                                                                 : uvti);
  }

  for (row = 0, k = 0; row < 8; ++row) {
    for (col = 0; col < 8; ++col, ++k) {
      fdtbl_Y[k] = 1 / (YTable[stbiw__jpg_ZigZag[k]] * aasf[row] * aasf[col]);
      fdtbl_UV[k] = 1 / (UVTable[stbiw__jpg_ZigZag[k]] * aasf[row] * aasf[col]);
    }
  }

  // Write Headers
  {
    static const unsigned char head0[] = {
        0xFF, 0xD8, 0xFF, 0xE0, 0, 0x10, 'J', 'F',  'I',  'F', 0,    1, 1,
        0,    0,    1,    0,    1, 0,    0,   0xFF, 0xDB, 0,   0x84, 0};
    static const unsigned char head2[] = {0xFF, 0xDA, 0, 0xC,  3, 1,    0,
                                          2,    0x11, 3, 0x11, 0, 0x3F, 0};
    const unsigned char head1[] = {
        0xFF,
        0xC0,
        0,
        0x11,
        8,
        (unsigned char)(height >> 8),
        STBIW_UCHAR(height),
        (unsigned char)(width >> 8),
        STBIW_UCHAR(width),
        3,
        1,
        (unsigned char)(subsample ? 0x22 : 0x11),
        0,
        2,
        0x11,
        1,
        3,
        0x11,
        1,
        0xFF,
        0xC4,
        0x01,
        0xA2,
        0,
    };
    s->func(s->context, (void *)head0, sizeof(head0));
    s->func(s->context, (void *)YTable, sizeof(YTable));
    stbiw__putc(s, 1);
    s->func(s->context, UVTable, sizeof(UVTable));
    s->func(s->context, (void *)head1, sizeof(head1));
    s->func(s->context, (void *)(std_dc_luminance_nrcodes + 1),
            sizeof(std_dc_luminance_nrcodes) - 1);
    s->func(s->context, (void *)std_dc_luminance_values,
            sizeof(std_dc_luminance_values));
    stbiw__putc(s, 0x10);  // HTYACinfo
    s->func(s->context, (void *)(std_ac_luminance_nrcodes + 1),
            sizeof(std_ac_luminance_nrcodes) - 1);
    s->func(s->context, (void *)std_ac_luminance_values,
            sizeof(std_ac_luminance_values));
    stbiw__putc(s, 1);  // HTUDCinfo
    s->func(s->context, (void *)(std_dc_chrominance_nrcodes + 1),
            sizeof(std_dc_chrominance_nrcodes) - 1);
    s->func(s->context, (void *)std_dc_chrominance_values,
            sizeof(std_dc_chrominance_values));
    stbiw__putc(s, 0x11);  // HTUACinfo
    s->func(s->context, (void *)(std_ac_chrominance_nrcodes + 1),
            sizeof(std_ac_chrominance_nrcodes) - 1);
    s->func(s->context, (void *)std_ac_chrominance_values,
            sizeof(std_ac_chrominance_values));
    s->func(s->context, (void *)head2, sizeof(head2));
  }

  // Encode 8x8 macroblocks
  {
    static const unsigned short fillBits[] = {0x7F, 7};
    int DCY = 0, DCU = 0, DCV = 0;
    int bitBuf = 0, bitCnt = 0;
    // comp == 2 is grey+alpha (alpha is ignored)
    int ofsG = comp > 2 ? 1 : 0, ofsB = comp > 2 ? 2 : 0;
    const unsigned char *dataR = (const unsigned char *)data;
    const unsigned char *dataG = dataR + ofsG;
    const unsigned char *dataB = dataR + ofsB;
    int x, y, pos;
    if (subsample) {
      for (y = 0; y < height; y += 16) {
        for (x = 0; x < width; x += 16) {
          float Y[256], U[256], V[256];
          for (row = y, pos = 0; row < y + 16; ++row) {
            // row >= height => use last input row
            int clamped_row = (row < height) ? row : height - 1;
            int base_p =
                (stbi__flip_vertically_on_write ? (height - 1 - clamped_row)
                                                : clamped_row) *
                width * comp;
            for (col = x; col < x + 16; ++col, ++pos) {
              // if col >= width => use pixel from last input column
              int p = base_p + ((col < width) ? col : (width - 1)) * comp;
              float r = dataR[p], g = dataG[p], b = dataB[p];
              Y[pos] = +0.29900f * r + 0.58700f * g + 0.11400f * b - 128;
              U[pos] = -0.16874f * r - 0.33126f * g + 0.50000f * b;
              V[pos] = +0.50000f * r - 0.41869f * g - 0.08131f * b;
            }
          }

          DCY = stbiw__jpg_processDU(s, &bitBuf, &bitCnt, Y + 0, 16, fdtbl_Y,
                                     DCY, YDC_HT, YAC_HT);
          DCY = stbiw__jpg_processDU(s, &bitBuf, &bitCnt, Y + 8, 16, fdtbl_Y,
                                     DCY, YDC_HT, YAC_HT);
          DCY = stbiw__jpg_processDU(s, &bitBuf, &bitCnt, Y + 128, 16, fdtbl_Y,
                                     DCY, YDC_HT, YAC_HT);
          DCY = stbiw__jpg_processDU(s, &bitBuf, &bitCnt, Y + 136, 16, fdtbl_Y,
                                     DCY, YDC_HT, YAC_HT);

          // subsample U,V
          {
            float subU[64], subV[64];
            int yy, xx;
            for (yy = 0, pos = 0; yy < 8; ++yy) {
              for (xx = 0; xx < 8; ++xx, ++pos) {
                int j = yy * 32 + xx * 2;
                subU[pos] =
                    (U[j + 0] + U[j + 1] + U[j + 16] + U[j + 17]) * 0.25f;
                subV[pos] =
                    (V[j + 0] + V[j + 1] + V[j + 16] + V[j + 17]) * 0.25f;
              }
            }
            DCU = stbiw__jpg_processDU(s, &bitBuf, &bitCnt, subU, 8, fdtbl_UV,
                                       DCU, UVDC_HT, UVAC_HT);
            DCV = stbiw__jpg_processDU(s, &bitBuf, &bitCnt, subV, 8, fdtbl_UV,
                                       DCV, UVDC_HT, UVAC_HT);
          }
        }
      }
    } else {
      for (y = 0; y < height; y += 8) {
        for (x = 0; x < width; x += 8) {
          float Y[64], U[64], V[64];
          for (row = y, pos = 0; row < y + 8; ++row) {
            // row >= height => use last input row
            int clamped_row = (row < height) ? row : height - 1;
            int base_p =
                (stbi__flip_vertically_on_write ? (height - 1 - clamped_row)
                                                : clamped_row) *
                width * comp;
            for (col = x; col < x + 8; ++col, ++pos) {
              // if col >= width => use pixel from last input column
              int p = base_p + ((col < width) ? col : (width - 1)) * comp;
              float r = dataR[p], g = dataG[p], b = dataB[p];
              Y[pos] = +0.29900f * r + 0.58700f * g + 0.11400f * b - 128;
              U[pos] = -0.16874f * r - 0.33126f * g + 0.50000f * b;
              V[pos] = +0.50000f * r - 0.41869f * g - 0.08131f * b;
            }
          }

          DCY = stbiw__jpg_processDU(s, &bitBuf, &bitCnt, Y, 8, fdtbl_Y, DCY,
                                     YDC_HT, YAC_HT);
          DCU = stbiw__jpg_processDU(s, &bitBuf, &bitCnt, U, 8, fdtbl_UV, DCU,
                                     UVDC_HT, UVAC_HT);
          DCV = stbiw__jpg_processDU(s, &bitBuf, &bitCnt, V, 8, fdtbl_UV, DCV,
                                     UVDC_HT, UVAC_HT);
        }
      }
    }

    // Do the bit alignment of the EOI marker
    stbiw__jpg_writeBits(s, &bitBuf, &bitCnt, fillBits);
  }

  // EOI
  stbiw__putc(s, 0xFF);
  stbiw__putc(s, 0xD9);

  return 1;
}

int stbi_write_jpg_to_func(stbi_write_func *func, void *context, int x, int y,
                           int comp, const void *data, int quality) {
  stbi__write_context s = {0};
  stbi__start_write_callbacks(&s, func, context);
  return stbi_write_jpg_core(&s, x, y, comp, (void *)data, quality);
}

int stbi_write_jpg(char const *filename, int x, int y, int comp,
                   const void *data, int quality) {
  stbi__write_context s = {0};
  if (stbi__start_write_file(&s, filename)) {
    int r = stbi_write_jpg_core(&s, x, y, comp, data, quality);
    stbi__end_write_file(&s);
    return r;
  } else
    return 0;
}

/*
********************************************************************************
Radiance RGBE HDR writer
by Baldur Karlsson */

static void stbiw__linear_to_rgbe(unsigned char *rgbe, float *linear) {
  int exponent;
  float normalize, maxcomp;
  maxcomp = MAX(linear[0], MAX(linear[1], linear[2]));
  if (maxcomp < 1e-32f) {
    rgbe[0] = rgbe[1] = rgbe[2] = rgbe[3] = 0;
  } else {
    /* no idea what HDR is but this could probably use clamping */
    normalize = (float)frexp(maxcomp, &exponent) * 256.0f / maxcomp;
    rgbe[0] = (unsigned char)(linear[0] * normalize);
    rgbe[1] = (unsigned char)(linear[1] * normalize);
    rgbe[2] = (unsigned char)(linear[2] * normalize);
    rgbe[3] = (unsigned char)(exponent + 128);
  }
}

static void stbiw__write_run_data(stbi__write_context *s, int length,
                                  unsigned char databyte) {
  unsigned char lengthbyte = STBIW_UCHAR(length + 128);
  assert(length + 128 <= 255);
  s->func(s->context, &lengthbyte, 1);
  s->func(s->context, &databyte, 1);
}

static void stbiw__write_dump_data(stbi__write_context *s, int length,
                                   unsigned char *data) {
  unsigned char lengthbyte = STBIW_UCHAR(length);
  assert(length <=
         128);  // inconsistent with spec but consistent with official code
  s->func(s->context, &lengthbyte, 1);
  s->func(s->context, data, length);
}

static void stbiw__write_hdr_scanline(stbi__write_context *s, int width,
                                      int ncomp, unsigned char *scratch,
                                      float *scanline) {
  unsigned char scanlineheader[4] = {2, 2, 0, 0};
  unsigned char rgbe[4];
  float linear[3];
  int x;

  scanlineheader[2] = (width & 0xff00) >> 8;
  scanlineheader[3] = (width & 0x00ff);

  /* skip RLE for images too small or large */
  if (width < 8 || width >= 32768) {
    for (x = 0; x < width; x++) {
      switch (ncomp) {
        case 4: /* fallthrough */
        case 3:
          linear[2] = scanline[x * ncomp + 2];
          linear[1] = scanline[x * ncomp + 1];
          linear[0] = scanline[x * ncomp + 0];
          break;
        default:
          linear[0] = linear[1] = linear[2] = scanline[x * ncomp + 0];
          break;
      }
      stbiw__linear_to_rgbe(rgbe, linear);
      s->func(s->context, rgbe, 4);
    }
  } else {
    int c, r;
    /* encode into scratch buffer */
    for (x = 0; x < width; x++) {
      switch (ncomp) {
        case 4: /* fallthrough */
        case 3:
          linear[2] = scanline[x * ncomp + 2];
          linear[1] = scanline[x * ncomp + 1];
          linear[0] = scanline[x * ncomp + 0];
          break;
        default:
          linear[0] = linear[1] = linear[2] = scanline[x * ncomp + 0];
          break;
      }
      stbiw__linear_to_rgbe(rgbe, linear);
      scratch[x + width * 0] = rgbe[0];
      scratch[x + width * 1] = rgbe[1];
      scratch[x + width * 2] = rgbe[2];
      scratch[x + width * 3] = rgbe[3];
    }

    s->func(s->context, scanlineheader, 4);

    /* RLE each component separately */
    for (c = 0; c < 4; c++) {
      unsigned char *comp = &scratch[width * c];

      x = 0;
      while (x < width) {
        // find first run
        r = x;
        while (r + 2 < width) {
          if (comp[r] == comp[r + 1] && comp[r] == comp[r + 2]) break;
          ++r;
        }
        if (r + 2 >= width) r = width;
        // dump up to first run
        while (x < r) {
          int len = r - x;
          if (len > 128) len = 128;
          stbiw__write_dump_data(s, len, &comp[x]);
          x += len;
        }
        // if there's a run, output it
        if (r + 2 < width) {  // same test as what we break out of in search
                              // loop, so only true if we break'd
          // find next byte after run
          while (r < width && comp[r] == comp[x]) ++r;
          // output run up to r
          while (x < r) {
            int len = r - x;
            if (len > 127) len = 127;
            stbiw__write_run_data(s, len, comp[x]);
            x += len;
          }
        }
      }
    }
  }
}

static int stbi_write_hdr_core(stbi__write_context *s, int x, int y, int comp,
                               float *data) {
  if (y <= 0 || x <= 0 || data == NULL)
    return 0;
  else {
    // Each component is stored separately. Allocate scratch space for full
    // output scanline.
    unsigned char *scratch = malloc(x * 4);
    int i, len;
    char buffer[128];
    char header[] =
        "#?RADIANCE\n# Written by stb_image_write.h\nFORMAT=32-bit_rle_rgbe\n";
    s->func(s->context, header, sizeof(header) - 1);
    len = (snprintf)(buffer, sizeof(buffer), "%s\n\n%s%d%s%d\n",
                     "EXPOSURE=          1.0000000000000", "-Y ", y, " +X ", x);
    s->func(s->context, buffer, len);
    for (i = 0; i < y; i++) {
      stbiw__write_hdr_scanline(
          s, x, comp, scratch,
          data + comp * x * (stbi__flip_vertically_on_write ? y - 1 - i : i));
    }
    free(scratch);
    return 1;
  }
}

int stbi_write_hdr_to_func(stbi_write_func *func, void *context, int x, int y,
                           int comp, const float *data) {
  stbi__write_context s = {0};
  stbi__start_write_callbacks(&s, func, context);
  return stbi_write_hdr_core(&s, x, y, comp, (float *)data);
}

int stbi_write_hdr(char const *filename, int x, int y, int comp,
                   const float *data) {
  stbi__write_context s = {0};
  if (stbi__start_write_file(&s, filename)) {
    int r = stbi_write_hdr_core(&s, x, y, comp, (float *)data);
    stbi__end_write_file(&s);
    return r;
  } else
    return 0;
}
