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
#include "libc/dos.internal.h"
#include "libc/elf/def.h"
#include "libc/fmt/wintime.internal.h"
#include "libc/limits.h"
#include "libc/log/check.h"
#include "libc/mem/gc.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/serialize.h"
#include "libc/stdio/rand.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/s.h"
#include "libc/time/struct/tm.h"
#include "libc/x/x.h"
#include "libc/x/xasprintf.h"
#include "libc/zip.internal.h"
#include "net/http/http.h"
#include "third_party/zlib/zlib.h"
#include "tool/build/lib/elfwriter.h"

#define ZIP_CFILE_HDR_SIZE (kZipCfileHdrMinSize + 36)

static bool ShouldCompress(const char *name, size_t namesize,
                           const unsigned char *data, size_t datasize,
                           bool nocompress) {
  return !nocompress && datasize >= 64 && !IsNoCompressExt(name, namesize) &&
         (datasize < 1000 || MeasureEntropy((void *)data, 1000) < 7);
}

static void GetDosLocalTime(int64_t utcunixts, uint16_t *out_time,
                            uint16_t *out_date) {
  struct tm tm;
  CHECK_NOTNULL(gmtime_r(&utcunixts, &tm));
  *out_time = DOS_TIME(tm.tm_hour, tm.tm_min, tm.tm_sec);
  *out_date = DOS_DATE(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday + 1);
}

static int DetermineVersionNeededToExtract(int method) {
  if (method == kZipCompressionDeflate) {
    return kZipEra1993;
  } else {
    return kZipEra1989;
  }
}

static unsigned char *EmitZipLfileHdr(unsigned char *p, const void *name,
                                      size_t namesize, uint32_t crc,
                                      uint8_t era, uint16_t gflags,
                                      uint16_t method, uint16_t mtime,
                                      uint16_t mdate, size_t compsize,
                                      size_t uncompsize) {
  p = WRITE32LE(p, kZipLfileHdrMagic);
  *p++ = era;
  *p++ = kZipOsDos;
  p = WRITE16LE(p, gflags);
  p = WRITE16LE(p, method);
  p = WRITE16LE(p, mtime);
  p = WRITE16LE(p, mdate);
  p = WRITE32LE(p, crc);
  p = WRITE32LE(p, compsize);
  p = WRITE32LE(p, uncompsize);
  p = WRITE16LE(p, namesize);
  p = WRITE16LE(p, 0); /* extra */
  return mempcpy(p, name, namesize);
}

static void EmitZipCdirHdr(unsigned char *p, const void *name, size_t namesize,
                           uint32_t crc, uint8_t era, uint16_t gflags,
                           uint16_t method, uint16_t mtime, uint16_t mdate,
                           uint16_t iattrs, uint16_t dosmode, uint16_t unixmode,
                           size_t compsize, size_t uncompsize,
                           size_t commentsize, struct timespec mtim,
                           struct timespec atim, struct timespec ctim) {
  uint64_t mt, at, ct;
  p = WRITE32LE(p, kZipCfileHdrMagic);
  *p++ = kZipCosmopolitanVersion;
  *p++ = kZipOsUnix;
  *p++ = era;
  *p++ = kZipOsDos;
  p = WRITE16LE(p, gflags);
  p = WRITE16LE(p, method);
  p = WRITE16LE(p, mtime);
  p = WRITE16LE(p, mdate);
  /* 16 */
  p = WRITE32LE(p, crc);
  p = WRITE32LE(p, compsize);
  p = WRITE32LE(p, uncompsize);
  p = WRITE16LE(p, namesize);
  p = WRITE16LE(p, 36); /* extra size */
  /* 32 */
  p = WRITE16LE(p, commentsize);
  p = WRITE16LE(p, 0); /* disk */
  p = WRITE16LE(p, iattrs);
  p = WRITE16LE(p, dosmode);
  p = WRITE16LE(p, unixmode);
  p = WRITE32LE(p, 0); /* RELOCATE ME (kZipCfileOffsetOffset) */
  /* 46 */
  memcpy(p, name, namesize);
  p += namesize;
  p = WRITE16LE(p, kZipExtraNtfs);
  p = WRITE16LE(p, 32);
  p = WRITE32LE(p, 0);
  p = WRITE16LE(p, 1);
  p = WRITE16LE(p, 24);
  mt = TimeSpecToWindowsTime(mtim);
  at = TimeSpecToWindowsTime(atim);
  ct = TimeSpecToWindowsTime(ctim);
  p = WRITE64LE(p, mt);
  p = WRITE64LE(p, at);
  p = WRITE64LE(p, ct);
}

/**
 * Embeds zip file in elf object.
 */
void elfwriter_zip(struct ElfWriter *elf, const char *symbol, const char *cname,
                   size_t namesize, const void *data, size_t size,
                   uint32_t mode, struct timespec mtim, struct timespec atim,
                   struct timespec ctim, bool nocompress) {
  z_stream zs;
  uint8_t era;
  uint32_t crc;
  unsigned char *lfile, *cfile;
  struct ElfWriterSymRef lfilesym;
  size_t lfilehdrsize, uncompsize, compsize, commentsize;
  uint16_t method, gflags, mtime, mdate, iattrs, dosmode;

  CHECK_NE(0, mtim.tv_sec);

  char *name = gc(strndup(cname, namesize));
  namesize = __zipos_normpath(name, name, strlen(name) + 1);
  if (S_ISDIR(mode) && namesize && name[namesize - 1] != '/') {
    name[namesize++] = '/';
    name[namesize] = 0;
  }

  gflags = 0;
  iattrs = 0;
  compsize = size;
  commentsize = 0;
  uncompsize = size;
  CHECK_LE(uncompsize, UINT32_MAX);
  lfilehdrsize = kZipLfileHdrMinSize + namesize;
  crc = crc32_z(0, data, uncompsize);
  GetDosLocalTime(mtim.tv_sec, &mtime, &mdate);
  if (isutf8(name, namesize)) gflags |= kZipGflagUtf8;
  if (S_ISREG(mode) && istext(data, size)) {
    iattrs |= kZipIattrText;
  }
  dosmode = !(mode & 0200) ? kNtFileAttributeReadonly : 0;
  method = ShouldCompress(name, namesize, data, size, nocompress)
               ? kZipCompressionDeflate
               : kZipCompressionNone;

  /* emit embedded file content w/ pkzip local file header */
  elfwriter_align(elf, 1, 0);
  elfwriter_startsection(elf, ".zip.file", SHT_PROGBITS, 0);
  if (method == kZipCompressionDeflate) {
    CHECK_EQ(Z_OK, deflateInit2(memset(&zs, 0, sizeof(zs)),
                                Z_DEFAULT_COMPRESSION, Z_DEFLATED, -MAX_WBITS,
                                MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY));
    zs.next_in = data;
    zs.avail_in = uncompsize;
    zs.next_out = ((lfile = elfwriter_reserve(
                        elf, (lfilehdrsize +
                              (zs.avail_out = compressBound(uncompsize))))) +
                   lfilehdrsize);
    CHECK_EQ(Z_STREAM_END, deflate(&zs, Z_FINISH));
    CHECK_EQ(Z_OK, deflateEnd(&zs));
    if (zs.total_out < uncompsize) {
      compsize = zs.total_out;
    } else {
      method = kZipCompressionNone;
    }
  } else {
    lfile = elfwriter_reserve(elf, lfilehdrsize + uncompsize);
  }
  if (method == kZipCompressionNone) {
    memcpy(lfile + lfilehdrsize, data, uncompsize);
  }
  era = method ? kZipEra1993 : kZipEra1989;
  EmitZipLfileHdr(lfile, name, namesize, crc, era, gflags, method, mtime, mdate,
                  compsize, uncompsize);
  elfwriter_commit(elf, lfilehdrsize + compsize);
  lfilesym = elfwriter_appendsym(elf, gc(xasprintf("%s%s", "zip+lfile:", name)),
                                 ELF64_ST_INFO(STB_LOCAL, STT_OBJECT),
                                 STV_DEFAULT, 0, lfilehdrsize);
  elfwriter_appendsym(elf, symbol, ELF64_ST_INFO(STB_GLOBAL, STT_OBJECT),
                      STV_DEFAULT, lfilehdrsize, compsize);
  elfwriter_finishsection(elf);

  /* emit central directory record */
  elfwriter_align(elf, 1, 0);
  elfwriter_startsection(elf, ".zip.cdir", SHT_PROGBITS, 0);
  EmitZipCdirHdr(
      (cfile = elfwriter_reserve(elf, ZIP_CFILE_HDR_SIZE + namesize)), name,
      namesize, crc, era, gflags, method, mtime, mdate, iattrs, dosmode, mode,
      compsize, uncompsize, commentsize, mtim, atim, ctim);
  elfwriter_appendsym(elf, gc(xasprintf("%s%s", "zip+cdir:", name)),
                      ELF64_ST_INFO(STB_LOCAL, STT_OBJECT), STV_DEFAULT, 0,
                      ZIP_CFILE_HDR_SIZE + namesize);
  elfwriter_appendrela(elf, kZipCfileOffsetOffset, lfilesym,
                       elfwriter_relatype_pc32(elf), 0);
  elfwriter_commit(elf, ZIP_CFILE_HDR_SIZE + namesize);
  elfwriter_finishsection(elf);
}
