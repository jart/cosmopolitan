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
#include "libc/assert.h"
#include "libc/fmt/conv.h"
#include "libc/limits.h"
#include "libc/mem/mem.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "third_party/stb/stb_image_write.h"
#include "third_party/zlib/zlib.h"

#define STBIW_UCHAR(x) (unsigned char)((x)&0xff)
#define stbiw__wpng4(o, a, b, c, d)                                           \
  ((o)[0] = STBIW_UCHAR(a), (o)[1] = STBIW_UCHAR(b), (o)[2] = STBIW_UCHAR(c), \
   (o)[3] = STBIW_UCHAR(d), (o) += 4)
#define stbiw__wp32(data, v) \
  stbiw__wpng4(data, (v) >> 24, (v) >> 16, (v) >> 8, (v));
#define stbiw__wptag(data, s) stbiw__wpng4(data, s[0], s[1], s[2], s[3])

int stbi_write_png_compression_level = 4;
int stbi_write_force_png_filter = -1;

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
static void stbiw__encode_png_line(unsigned char *pixels, int stride_bytes,
                                   int width, int height, int y, int n,
                                   int filter_type, signed char *line_buffer) {
  const int mapping[] = {0, 1, 2, 3, 4};
  const int firstmap[] = {0, 1, 0, 5, 6};
  unsigned char *z;
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
