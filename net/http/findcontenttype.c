/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/assert.h"
#include "libc/serialize.h"
#include "libc/intrin/bswap.h"
#include "libc/macros.internal.h"
#include "libc/str/str.h"
#include "libc/str/tab.internal.h"
#include "net/http/http.h"

static const struct ContentTypeExtension {
  unsigned char ext[8];
  const char *mime;
} kContentTypeExtension[] = {
    {"7z", "application/x-7z-compressed"},     //
    {"aac", "audio/aac"},                      //
    {"apng", "image/apng"},                    //
    {"atom", "application/atom+xml"},          //
    {"avi", "video/x-msvideo"},                //
    {"avif", "image/avif"},                    //
    {"azw", "application/vnd.amazon.ebook"},   //
    {"bmp", "image/bmp"},                      //
    {"bz2", "application/x-bzip2"},            //
    {"c", "text/plain"},                       //
    {"cc", "text/plain"},                      //
    {"css", "text/css"},                       //
    {"csv", "text/csv"},                       //
    {"diff", "text/plain"},                    //
    {"diff", "text/plain"},                    //
    {"doc", "application/msword"},             //
    {"epub", "application/epub+zip"},          //
    {"gif", "image/gif"},                      //
    {"gz", "application/gzip"},                //
    {"h", "text/plain"},                       //
    {"htm", "text/html"},                      //
    {"html", "text/html"},                     //
    {"i", "text/plain"},                       //
    {"ico", "image/vnd.microsoft.icon"},       //
    {"jar", "application/java-archive"},       //
    {"jpeg", "image/jpeg"},                    //
    {"jpg", "image/jpeg"},                     //
    {"js", "text/javascript"},                 //
    {"json", "application/json"},              //
    {"m4a", "audio/mpeg"},                     //
    {"markdown", "text/plain"},                //
    {"md", "text/plain"},                      //
    {"mid", "audio/midi"},                     //
    {"midi", "audio/midi"},                    //
    {"mjs", "text/javascript"},                //
    {"mp2", "audio/mpeg"},                     //
    {"mp3", "audio/mpeg"},                     //
    {"mp4", "video/mp4"},                      //
    {"mpeg", "video/mpeg"},                    //
    {"mpg", "video/mpeg"},                     //
    {"oga", "audio/ogg"},                      //
    {"ogg", "application/ogg"},                //
    {"ogv", "video/ogg"},                      //
    {"ogx", "application/ogg"},                //
    {"otf", "font/otf"},                       //
    {"patch", "text/plain"},                   //
    {"pdf", "application/pdf"},                //
    {"png", "image/png"},                      //
    {"rar", "application/vnd.rar"},            //
    {"rtf", "application/rtf"},                //
    {"s", "text/plain"},                       //
    {"sh", "application/x-sh"},                //
    {"sqlite", "application/vnd.sqlite3"},     //
    {"sqlite3", "application/vnd.sqlite3"},    //
    {"svg", "image/svg+xml"},                  //
    {"swf", "application/x-shockwave-flash"},  //
    {"t38", "image/t38"},                      //
    {"tar", "application/x-tar"},              //
    {"tiff", "image/tiff"},                    //
    {"ttf", "font/ttf"},                       //
    {"txt", "text/plain"},                     //
    {"ul", "audio/basic"},                     //
    {"ulaw", "audio/basic"},                   //
    {"wasm", "application/wasm"},              //
    {"wav", "audio/x-wav"},                    //
    {"weba", "audio/webm"},                    //
    {"webm", "video/webm"},                    //
    {"webp", "image/webp"},                    //
    {"woff", "font/woff"},                     //
    {"woff2", "font/woff2"},                   //
    {"wsdl", "application/wsdl+xml"},          //
    {"xhtml", "application/xhtml+xml"},        //
    {"xls", "application/vnd.ms-excel"},       //
    {"xml", "application/xml"},                //
    {"xsl", "application/xslt+xml"},           //
    {"xslt", "application/xslt+xml"},          //
    {"xz", "application/x-xz"},                //
    {"z", "application/zlib"},                 //
    {"zip", "application/zip"},                //
    {"zst", "application/zstd"},               //
};

static inline int CompareInts(const uint64_t x, uint64_t y) {
  return x > y ? 1 : x < y ? -1 : 0;
}

static const char *BisectContentType(uint64_t ext) {
  int c, m, l, r;
  l = 0;
  r = ARRAYLEN(kContentTypeExtension) - 1;
  while (l <= r) {
    m = (l & r) + ((l ^ r) >> 1);  // floor((a+b)/2)
    c = CompareInts(READ64BE(kContentTypeExtension[m].ext), ext);
    if (c < 0) {
      l = m + 1;
    } else if (c > 0) {
      r = m - 1;
    } else {
      return kContentTypeExtension[m].mime;
    }
  }
  return 0;
}

/**
 * Returns Content-Type for file extension.
 */
const char *FindContentType(const char *p, size_t n) {
  int c;
  uint64_t w;
  if (n == -1) n = p ? strlen(p) : 0;
  for (w = 0; n--;) {
    c = p[n] & 255;
    if (c == '.') {
      return BisectContentType(bswap_64(w));
    }
    w <<= 8;
    w |= kToLower[c];
  }
  return 0;
}
