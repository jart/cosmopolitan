/* stb_image_write - v1.13 - public domain - http://nothings.org/stb
 * writes out PNG/BMP/TGA/JPEG/HDR images to C stdio - Sean Barrett 2010-2015
 *                                  no warranty implied; use at your own risk
 *
 * ABOUT:
 *
 *    This file is a library for writing images to stdio or a callback.
 *
 *    The PNG output is not optimal; it is 20-50% larger than the file
 *    written by a decent optimizing implementation; though providing a
 *    custom zlib compress function (see STBIW_ZLIB_COMPRESS) can
 *    mitigate that. This library is designed for source code
 *    compactness and simplicity, not optimal image file size or
 *    run-time performance.
 *
 * USAGE:
 *
 *    There are five functions, one for each image file format:
 *
 *      stbi_write_png
 *      stbi_write_bmp
 *      stbi_write_tga
 *      stbi_write_jpg
 *      stbi_write_hdr
 *
 *      stbi_flip_vertically_on_write
 *
 *    There are also five equivalent functions that use an arbitrary
 *    write function. You are expected to open/close your
 *    file-equivalent before and after calling these:
 *
 *      stbi_write_png_to_func
 *      stbi_write_bmp_to_func
 *      stbi_write_tga_to_func
 *      stbi_write_hdr_to_func
 *      stbi_write_jpg_to_func
 *
 *    where the callback is:
 *       void stbi_write_func(void *context, void *data, int size);
 *
 *    You can configure it with these:
 *       stbi_write_tga_with_rle
 *       stbi_write_png_compression_level
 *       stbi_write_force_png_filter
 *
 *    Each function returns 0 on failure and non-0 on success.
 *
 *    The functions create an image file defined by the parameters. The
 *    image is a rectangle of pixels stored from left-to-right,
 *    top-to-bottom. Each pixel contains 'comp' channels of data stored
 *    interleaved with 8-bits per channel, in the following order: 1=Y,
 *    2=YA, 3=RGB, 4=RGBA. (Y is monochrome color.) The rectangle is 'w'
 *    pixels wide and 'h' pixels tall. The *data pointer points to the
 *    first byte of the top-left-most pixel. For PNG, "stride_in_bytes"
 *    is the distance in bytes from the first byte of a row of pixels to
 *    the first byte of the next row of pixels.
 *
 *    PNG creates output files with the same number of components as the
 *    input. The BMP format expands Y to RGB in the file format and does
 *    not output alpha.
 *
 *    PNG supports writing rectangles of data even when the bytes
 *    storing rows of data are not consecutive in memory (e.g.
 *    sub-rectangles of a larger image), by supplying the stride between
 *    the beginning of adjacent rows. The other formats do not. (Thus
 *    you cannot write a native-format BMP through the BMP writer, both
 *    because it is in BGR order and because it may have padding at the
 *    end of the line.)
 *
 *    PNG allows you to set the deflate compression level by setting the
 *    global variable 'stbi_write_png_compression_level' (it defaults to
 *    8).
 *
 *    HDR expects linear float data. Since the format is always 32-bit
 *    rgb(e) data, alpha (if provided) is discarded, and for monochrome
 *    data it is replicated across all three channels.
 *
 *    TGA supports RLE or non-RLE compressed data. To use
 *    non-RLE-compressed data, set the global variable
 *    'stbi_write_tga_with_rle' to 0.
 *
 *    JPEG does ignore alpha channels in input data; quality is between
 *    1 and 100. Higher quality looks better but results in a bigger
 *    image. JPEG baseline (no JPEG progressive).
 *
 * CREDITS:
 *
 *
 *    Sean Barrett           -    PNG/BMP/TGA
 *    Baldur Karlsson        -    HDR
 *    Jean-Sebastien Guay    -    TGA monochrome
 *    Tim Kelsey             -    misc enhancements
 *    Alan Hickman           -    TGA RLE
 *    Emmanuel Julien        -    initial file IO callback implementation
 *    Jon Olick              -    original jo_jpeg.cpp code
 *    Daniel Gibson          -    integrate JPEG, allow external zlib
 *    Aarni Koskela          -    allow choosing PNG filter
 *
 *    bugfixes:
 *       github:Chribba
 *       Guillaume Chereau
 *       github:jry2
 *       github:romigrou
 *       Sergio Gonzalez
 *       Jonas Karlsson
 *       Filip Wasil
 *       Thatcher Ulrich
 *       github:poppolopoppo
 *       Patrick Boettcher
 *       github:xeekworx
 *       Cap Petschulat
 *       Simon Rodriguez
 *       Ivan Tikhonov
 *       github:ignotion
 *       Adam Schackart
 *
 * LICENSE
 *
 *   Public Domain (www.unlicense.org)
 */
#include "dsp/core/core.h"
#include "libc/assert.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/fmt.h"
#include "libc/limits.h"
#include "libc/macros.h"
#include "libc/math.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "third_party/stb/stb_image_write.h"
#include "third_party/zlib/zlib.h"

#define STBIW_UCHAR(x)                       (unsigned char)((x)&0xff)
#define STBIW_REALLOC_SIZED(p, oldsz, newsz) realloc(p, newsz)

typedef struct {
  stbi_write_func *func;
  void *context;
} stbi__write_context;

int stbi__flip_vertically_on_write = 0;
int stbi_write_tga_with_rle = 1;

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

typedef unsigned int stbiw_uint32;
typedef int stb_image_write_test[sizeof(stbiw_uint32) == 4 ? 1 : -1];

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
        stbiw_uint32 x = va_arg(v, int);
        unsigned char b[4];
        b[0] = STBIW_UCHAR(x);
        b[1] = STBIW_UCHAR(x >> 8);
        b[2] = STBIW_UCHAR(x >> 16);
        b[3] = STBIW_UCHAR(x >> 24);
        s->func(s->context, b, 4);
        break;
      }
      default:
        unreachable;
    }
  }
}

static void stbiw__writef(stbi__write_context *s, const char *fmt, ...) {
  va_list v;
  va_start(v, fmt);
  stbiw__writefv(s, fmt, v);
  va_end(v);
}

static void stbiw__putc(stbi__write_context *s, unsigned char c) {
  s->func(s->context, &c, 1);
}

static void stbiw__write3(stbi__write_context *s, unsigned char a,
                          unsigned char b, unsigned char c) {
  unsigned char arr[3];
  arr[0] = a;
  arr[1] = b;
  arr[2] = c;
  s->func(s->context, arr, 3);
}

static void stbiw__write_pixel(stbi__write_context *s, int rgb_dir, int comp,
                               int write_alpha, int expand_mono,
                               unsigned char *d) {
  unsigned char bg[3] = {255, 0, 255}, px[3];
  int k;

  if (write_alpha < 0) s->func(s->context, &d[comp - 1], 1);

  switch (comp) {
    case 2:  // 2 pixels = mono + alpha, alpha is written separately, so same as
             // 1-channel case
    case 1:
      if (expand_mono)
        stbiw__write3(s, d[0], d[0], d[0]);  // monochrome bmp
      else
        s->func(s->context, d, 1);  // monochrome TGA
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
  if (write_alpha > 0) s->func(s->context, &d[comp - 1], 1);
}

static void stbiw__write_pixels(stbi__write_context *s, int rgb_dir, int vdir,
                                int x, int y, int comp, void *data,
                                int write_alpha, int scanline_pad,
                                int expand_mono) {
  stbiw_uint32 zero = 0;
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
  int pad = (-x * 3) & 3;
  return stbiw__outfile(s, -1, -1, x, y, comp, 1, (void *)data, 0, pad,
                        "11 4 22 4"
                        "4 44 22 444444",
                        'B', 'M', 14 + 40 + (x * 3 + pad) * y, 0, 0,
                        14 + 40,                             // file header
                        40, x, y, 1, 24, 0, 0, 0, 0, 0, 0);  // bitmap header
}

int stbi_write_bmp_to_func(stbi_write_func *func, void *context, int x, int y,
                           int comp, const void *data) {
  stbi__write_context s;
  stbi__start_write_callbacks(&s, func, context);
  return stbi_write_bmp_core(&s, x, y, comp, data);
}

int stbi_write_bmp(char const *filename, int x, int y, int comp,
                   const void *data) {
  stbi__write_context s;
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
          s->func(s->context, &header, 1);
          for (k = 0; k < len; ++k) {
            stbiw__write_pixel(s, -1, comp, has_alpha, 0, begin + k * comp);
          }
        } else {
          unsigned char header = STBIW_UCHAR(len - 129);
          s->func(s->context, &header, 1);
          stbiw__write_pixel(s, -1, comp, has_alpha, 0, begin);
        }
      }
    }
  }
  return 1;
}

int stbi_write_tga_to_func(stbi_write_func *func, void *context, int x, int y,
                           int comp, const void *data) {
  stbi__write_context s;
  stbi__start_write_callbacks(&s, func, context);
  return stbi_write_tga_core(&s, x, y, comp, (void *)data);
}

int stbi_write_tga(char const *filename, int x, int y, int comp,
                   const void *data) {
  stbi__write_context s;
  if (stbi__start_write_file(&s, filename)) {
    int r = stbi_write_tga_core(&s, x, y, comp, (void *)data);
    stbi__end_write_file(&s);
    return r;
  } else
    return 0;
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
                                int *bitCnt, float *CDU, float *fdtbl, int DC,
                                const unsigned short HTDC[256][2],
                                const unsigned short HTAC[256][2]) {
  const unsigned short EOB[2] = {HTAC[0x00][0], HTAC[0x00][1]};
  const unsigned short M16zeroes[2] = {HTAC[0xF0][0], HTAC[0xF0][1]};
  unsigned i, diff, end0pos;
  int DU[64];

  dctjpeg((void *)CDU);

  // Quantize/descale/zigzag the coefficients
  for (i = 0; i < 64; ++i) {
    float v = CDU[i] * fdtbl[i];
    DU[stbiw__jpg_ZigZag[i]] = v < 0 ? ceilf(v - 0.5f) : floorf(v + 0.5f);
    // DU[stbiw__jpg_ZigZag[i]] = (int)(v < 0 ? ceilf(v - 0.5f) : floorf(v +
    // 0.5f)); ceilf() and floorf() are C99, not C89, but I /think/ they're not
    // needed here anyway?
    /* DU[stbiw__jpg_ZigZag[i]] = (int)(v < 0 ? v - 0.5f : v + 0.5f); */
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

  int row, col, i, k;
  float fdtbl_Y[64], fdtbl_UV[64];
  unsigned char YTable[64], UVTable[64];

  if (!data || !width || !height || comp > 4 || comp < 1) {
    return 0;
  }

  quality = quality ? quality : 97;
  quality = quality < 1 ? 1 : quality > 100 ? 100 : quality;
  quality = quality < 50 ? 5000 / quality : 200 - quality * 2;

  for (i = 0; i < 64; ++i) {
    int uvti, yti = div100int64((YQT[i] * quality + 50));
    YTable[stbiw__jpg_ZigZag[i]] =
        (unsigned char)(yti < 1 ? 1 : yti > 255 ? 255 : yti);
    uvti = div100int64(UVQT[i] * quality + 50);
    UVTable[stbiw__jpg_ZigZag[i]] =
        (unsigned char)(uvti < 1 ? 1 : uvti > 255 ? 255 : uvti);
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
        0x11,
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
    const unsigned char *imageData = (const unsigned char *)data;
    int DCY = 0, DCU = 0, DCV = 0;
    int bitBuf = 0, bitCnt = 0;
    // comp == 2 is grey+alpha (alpha is ignored)
    int ofsG = comp > 2 ? 1 : 0, ofsB = comp > 2 ? 2 : 0;
    int x, y, pos;
    for (y = 0; y < height; y += 8) {
      for (x = 0; x < width; x += 8) {
        float YDU[64], UDU[64], VDU[64];
        for (row = y, pos = 0; row < y + 8; ++row) {
          // row >= height => use last input row
          int clamped_row = (row < height) ? row : height - 1;
          int base_p =
              (stbi__flip_vertically_on_write ? (height - 1 - clamped_row)
                                              : clamped_row) *
              width * comp;
          for (col = x; col < x + 8; ++col, ++pos) {
            float r, g, b;
            // if col >= width => use pixel from last input column
            int p = base_p + ((col < width) ? col : (width - 1)) * comp;

            r = imageData[p + 0];
            g = imageData[p + ofsG];
            b = imageData[p + ofsB];
            YDU[pos] = +0.29900f * r + 0.58700f * g + 0.11400f * b - 128;
            UDU[pos] = -0.16874f * r - 0.33126f * g + 0.50000f * b;
            VDU[pos] = +0.50000f * r - 0.41869f * g - 0.08131f * b;
          }
        }

        DCY = stbiw__jpg_processDU(s, &bitBuf, &bitCnt, YDU, fdtbl_Y, DCY,
                                   YDC_HT, YAC_HT);
        DCU = stbiw__jpg_processDU(s, &bitBuf, &bitCnt, UDU, fdtbl_UV, DCU,
                                   UVDC_HT, UVAC_HT);
        DCV = stbiw__jpg_processDU(s, &bitBuf, &bitCnt, VDU, fdtbl_UV, DCV,
                                   UVDC_HT, UVAC_HT);
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
  stbi__write_context s;
  stbi__start_write_callbacks(&s, func, context);
  return stbi_write_jpg_core(&s, x, y, comp, (void *)data, quality);
}

int stbi_write_jpg(char const *filename, int x, int y, int comp,
                   const void *data, int quality) {
  stbi__write_context s;
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
  stbi__write_context s;
  stbi__start_write_callbacks(&s, func, context);
  return stbi_write_hdr_core(&s, x, y, comp, (float *)data);
}

int stbi_write_hdr(char const *filename, int x, int y, int comp,
                   const float *data) {
  stbi__write_context s;
  if (stbi__start_write_file(&s, filename)) {
    int r = stbi_write_hdr_core(&s, x, y, comp, (float *)data);
    stbi__end_write_file(&s);
    return r;
  } else
    return 0;
}
