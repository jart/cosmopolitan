/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/alg/arraylist.internal.h"
#include "libc/bits/bits.h"
#include "libc/bits/safemacros.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/timespec.h"
#include "libc/dos.h"
#include "libc/elf/def.h"
#include "libc/fmt/conv.h"
#include "libc/limits.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/alloca.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/struct/imageauxsymbolex.internal.h"
#include "libc/runtime/gc.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/ok.h"
#include "libc/sysv/consts/prot.h"
#include "libc/time/struct/tm.h"
#include "libc/time/time.h"
#include "libc/x/x.h"
#include "libc/zip.h"
#include "third_party/getopt/getopt.h"
#include "third_party/zlib/zlib.h"
#include "tool/build/lib/elfwriter.h"

#define ZIP_LOCALFILE_SECTION ".zip.2."
#define ZIP_DIRECTORY_SECTION ".zip.4."

char *symbol_;
char *outpath_;
char *yoink_;
int64_t image_base_;

const size_t kMinCompressSize = 32;
const char kNoCompressExts[][8] = {".gz",  ".xz",  ".jpg",  ".png",
                                   ".gif", ".zip", ".bz2",  ".mpg",
                                   ".mp4", ".lz4", ".webp", ".mpeg"};

wontreturn void PrintUsage(int rc, FILE *f) {
  fprintf(f, "%s%s%s\n", "Usage: ", program_invocation_name,
          " [-o FILE] [-s SYMBOL] [-y YOINK] [FILE...]\n");
  exit(rc);
}

void GetOpts(int *argc, char ***argv) {
  int opt;
  yoink_ = "__zip_start";
  image_base_ = IMAGE_BASE_VIRTUAL;
  while ((opt = getopt(*argc, *argv, "?ho:s:y:b:")) != -1) {
    switch (opt) {
      case 'o':
        outpath_ = optarg;
        break;
      case 's':
        symbol_ = optarg;
        break;
      case 'y':
        yoink_ = optarg;
        break;
      case 'b':
        image_base_ = strtol(optarg, NULL, 0);
        break;
      case '?':
      case 'h':
        PrintUsage(EXIT_SUCCESS, stdout);
      default:
        PrintUsage(EX_USAGE, stderr);
    }
  }
  *argc -= optind;
  *argv += optind;
  CHECK_NOTNULL(outpath_);
}

bool IsUtf8(const void *data, size_t size) {
  const unsigned char *p, *pe;
  for (p = data, pe = p + size; p + 2 <= pe; ++p) {
    if (*p >= 0300) {
      if (*p >= 0200 && *p < 0300) {
        return true;
      } else {
        return false;
      }
    }
  }
  return false;
}

bool IsText(const void *data, size_t size) {
  const unsigned char *p, *pe;
  for (p = data, pe = p + size; p < pe; ++p) {
    if (*p <= 3) {
      return false;
    }
  }
  return true;
}

bool ShouldCompress(const char *name, size_t size) {
  size_t i;
  char key[8];
  const char *p;
  if (!(p = memrchr(name, '.', size))) return true;
  strncpy(key, p, sizeof(key));
  for (i = 0; i < ARRAYLEN(kNoCompressExts); ++i) {
    if (memcmp(key, kNoCompressExts[i], sizeof(key)) == 0) return false;
  }
  return true;
}

void GetDosLocalTime(int64_t utcunixts, uint16_t *out_time,
                     uint16_t *out_date) {
  struct tm tm;
  CHECK_NOTNULL(localtime_r(&utcunixts, &tm));
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
                           size_t commentsize, struct stat *st) {
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
#define CFILE_HDR_SIZE (kZipCfileHdrMinSize + 36)
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
#define NTTIME(t) \
  (t.tv_sec + MODERNITYSECONDS) * HECTONANOSECONDS + t.tv_nsec / 100
  mt = NTTIME(st->st_mtim);
  at = NTTIME(st->st_atim);
  ct = NTTIME(st->st_ctim);
  p = WRITE64LE(p, mt);
  p = WRITE64LE(p, at);
  p = WRITE64LE(p, ct);
}

void EmitZip(struct ElfWriter *elf, const char *name, size_t namesize,
             const unsigned char *data, struct stat *st) {
  z_stream zs;
  uint8_t era;
  uint32_t crc;
  unsigned char *lfile, *cfile;
  struct ElfWriterSymRef lfilesym;
  size_t lfilehdrsize, uncompsize, compsize, commentsize;
  uint16_t method, gflags, mtime, mdate, iattrs, dosmode;

  gflags = 0;
  iattrs = 0;
  compsize = st->st_size;
  uncompsize = st->st_size;
  CHECK_LE(uncompsize, UINT32_MAX);
  lfilehdrsize = kZipLfileHdrMinSize + namesize;
  crc = crc32_z(0, data, uncompsize);
  GetDosLocalTime(st->st_mtim.tv_sec, &mtime, &mdate);
  if (IsUtf8(name, namesize)) gflags |= kZipGflagUtf8;
  if (IsText(data, st->st_size)) iattrs |= kZipIattrText;
  commentsize = kZipCdirHdrLinkableSize - (CFILE_HDR_SIZE + namesize);
  dosmode = !(st->st_mode & 0200) ? kNtFileAttributeReadonly : 0;
  method = (st->st_size >= kMinCompressSize && ShouldCompress(name, namesize))
               ? kZipCompressionDeflate
               : kZipCompressionNone;

  /* emit embedded file content w/ pkzip local file header */
  elfwriter_align(elf, 1, 0);
  elfwriter_startsection(elf,
                         gc(xasprintf("%s%s", ZIP_LOCALFILE_SECTION, name)),
                         SHT_PROGBITS, SHF_ALLOC);
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
  elfwriter_appendsym(elf, name, ELF64_ST_INFO(STB_GLOBAL, STT_OBJECT),
                      STV_DEFAULT, lfilehdrsize, compsize);
  elfwriter_finishsection(elf);

  /* emit central directory record */
  elfwriter_align(elf, 1, 0);
  elfwriter_startsection(elf,
                         gc(xasprintf("%s%s", ZIP_DIRECTORY_SECTION, name)),
                         SHT_PROGBITS, SHF_ALLOC);
  EmitZipCdirHdr((cfile = elfwriter_reserve(elf, kZipCdirHdrLinkableSize)),
                 name, namesize, crc, era, gflags, method, mtime, mdate, iattrs,
                 dosmode, st->st_mode, compsize, uncompsize, commentsize, st);
  elfwriter_appendsym(elf, gc(xasprintf("%s%s", "zip+cdir:", name)),
                      ELF64_ST_INFO(STB_LOCAL, STT_OBJECT), STV_DEFAULT, 0,
                      kZipCdirHdrLinkableSize);
  elfwriter_appendrela(elf, kZipCfileOffsetOffset, lfilesym, R_X86_64_32,
                       -image_base_);
  elfwriter_commit(elf, kZipCdirHdrLinkableSize);
  elfwriter_finishsection(elf);
}

void ProcessFile(struct ElfWriter *elf, const char *path) {
  int fd;
  void *map;
  size_t pathlen;
  struct stat st;
  pathlen = strlen(path);
  CHECK_NE(-1, (fd = open(path, O_RDONLY)));
  CHECK_NE(-1, fstat(fd, &st));
  if (st.st_size) {
    CHECK_NE(MAP_FAILED,
             (map = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0)));
    CHECK_NE(-1, close(fd));
  } else {
    map = NULL;
  }
  EmitZip(elf, path, pathlen, map, &st);
  if (st.st_size) {
    CHECK_NE(-1, munmap(map, st.st_size));
  }
}

void PullEndOfCentralDirectoryIntoLinkage(struct ElfWriter *elf) {
  elfwriter_align(elf, 1, 0);
  elfwriter_startsection(elf, ".yoink", SHT_PROGBITS,
                         SHF_ALLOC | SHF_EXECINSTR);
  elfwriter_yoink(elf, yoink_);
  elfwriter_finishsection(elf);
}

void CheckFilenameKosher(const char *path) {
  CHECK_LE(strlen(path), PATH_MAX);
  CHECK(!startswith(path, "/"));
  CHECK(!strstr(path, ".."));
}

void zipobj(int argc, char **argv) {
  size_t i;
  struct ElfWriter *elf;
  CHECK_LT(argc, UINT16_MAX / 3 - 64); /* ELF 64k section limit */
  GetOpts(&argc, &argv);
  for (i = 0; i < argc; ++i) CheckFilenameKosher(argv[i]);
  elf = elfwriter_open(outpath_, 0644);
  elfwriter_cargoculting(elf);
  for (i = 0; i < argc; ++i) ProcessFile(elf, argv[i]);
  PullEndOfCentralDirectoryIntoLinkage(elf);
  elfwriter_close(elf);
}

int main(int argc, char **argv) {
  showcrashreports();
  zipobj(argc, argv);
  return 0;
}
