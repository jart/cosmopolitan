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
#include "libc/bits/safemacros.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/timespec.h"
#include "libc/elf/def.h"
#include "libc/limits.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/mem/alloca.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/struct/imageauxsymbolex.internal.h"
#include "libc/runtime/gc.h"
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

#define ZIP_LOCALFILE_SECTION ".piro.data.sort.zip.2."
#define ZIP_DIRECTORY_SECTION ".piro.data.sort.zip.4."

#define PUT8(P, V)  *P++ = V
#define PUT16(P, V) P[0] = V & 0xff, P[1] = V >> 010 & 0xff, P += 2
#define PUT32(P, V)                                                \
  P[0] = V & 0xff, P[1] = V >> 010 & 0xff, P[2] = V >> 020 & 0xff, \
  P[3] = V >> 030 & 0xff, P += 4

#define DOS_DATE(YEAR, MONTH_IDX1, DAY_IDX1) \
  (((YEAR)-1980) << 9 | (MONTH_IDX1) << 5 | (DAY_IDX1))
#define DOS_TIME(HOUR, MINUTE, SECOND) \
  ((HOUR) << 11 | (MINUTE) << 5 | (SECOND) >> 1)

char *symbol_;
char *outpath_;
char *yoink_;

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
  while ((opt = getopt(*argc, *argv, "?ho:s:y:")) != -1) {
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

bool IsPureAscii(const void *data, size_t size) {
  const unsigned char *p, *pe;
  for (p = data, pe = p + size; p < pe; ++p) {
    if (!*p || *p >= 0x80) {
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

static unsigned char *EmitZipLfileHdr(unsigned char *op, const void *name,
                                      size_t namesize, uint32_t crc,
                                      uint8_t era, uint16_t gflags,
                                      uint16_t method, uint16_t mtime,
                                      uint16_t mdate, size_t compsize,
                                      size_t uncompsize) {
  PUT32(op, kZipLfileHdrMagic);
  PUT8(op, era);
  PUT8(op, kZipOsDos);
  PUT16(op, gflags);
  PUT16(op, method);
  PUT16(op, mtime);
  PUT16(op, mdate);
  PUT32(op, crc);
  PUT32(op, compsize);
  PUT32(op, uncompsize);
  PUT16(op, namesize);
  PUT16(op, 0); /* extra */
  return mempcpy(op, name, namesize);
}

static void EmitZipCdirHdr(unsigned char *op, const void *name, size_t namesize,
                           uint32_t crc, uint8_t era, uint16_t gflags,
                           uint16_t method, uint16_t mtime, uint16_t mdate,
                           uint16_t iattrs, uint16_t dosmode, uint16_t unixmode,
                           size_t compsize, size_t uncompsize,
                           size_t commentsize) {
  PUT32(op, kZipCfileHdrMagic);
  PUT8(op, kZipCosmopolitanVersion);
  PUT8(op, kZipOsUnix);
  PUT8(op, era);
  PUT8(op, kZipOsDos);
  PUT16(op, gflags);
  PUT16(op, method);
  PUT16(op, mtime);
  PUT16(op, mdate);
  PUT32(op, crc);
  PUT32(op, compsize);
  PUT32(op, uncompsize);
  PUT16(op, namesize);
  PUT16(op, 0); /* extra size */
  PUT16(op, commentsize);
  PUT16(op, 0); /* disk */
  PUT16(op, iattrs);
  PUT16(op, dosmode);
  PUT16(op, unixmode);
  PUT32(op, 0); /* RELOCATE ME (kZipCfileOffsetOffset) */
  memcpy(op, name, namesize);
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

  compsize = st->st_size;
  uncompsize = st->st_size;
  CHECK_LE(uncompsize, UINT32_MAX);
  lfilehdrsize = kZipLfileHdrMinSize + namesize;
  crc = crc32_z(0, data, uncompsize);
  GetDosLocalTime(st->st_mtim.tv_sec, &mtime, &mdate);
  gflags = IsPureAscii(name, namesize) ? 0 : kZipGflagUtf8;
  commentsize = kZipCdirHdrLinkableSize - (kZipCfileHdrMinSize + namesize);
  iattrs = IsPureAscii(data, st->st_size) ? kZipIattrAscii : kZipIattrBinary;
  dosmode = !(st->st_mode & 0200) ? kNtFileAttributeReadonly : 0;
  method = (st->st_size >= kMinCompressSize && ShouldCompress(name, namesize))
               ? kZipCompressionDeflate
               : kZipCompressionNone;

  /* emit embedded file content w/ pkzip local file header */
  elfwriter_align(elf, kZipCdirAlign, 0);
  elfwriter_startsection(elf,
                         gc(xasprintf("%s%s", ZIP_LOCALFILE_SECTION, name)),
                         SHT_PROGBITS, SHF_ALLOC | SHF_WRITE);
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
  era = (gflags || method) ? kZipEra1993 : kZipEra1989;
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
  elfwriter_align(elf, kZipCdirAlign, 0);
  elfwriter_startsection(elf,
                         gc(xasprintf("%s%s", ZIP_DIRECTORY_SECTION, name)),
                         SHT_PROGBITS, SHF_ALLOC | SHF_WRITE);
  EmitZipCdirHdr((cfile = elfwriter_reserve(elf, kZipCdirHdrLinkableSize)),
                 name, namesize, crc, era, gflags, method, mtime, mdate, iattrs,
                 dosmode, st->st_mode, compsize, uncompsize, commentsize);
  elfwriter_appendsym(elf, gc(xasprintf("%s%s", "zip+cdir:", name)),
                      ELF64_ST_INFO(STB_LOCAL, STT_OBJECT), STV_DEFAULT, 0,
                      kZipCdirHdrLinkableSize);
  elfwriter_appendrela(elf, kZipCfileOffsetOffset, lfilesym, R_X86_64_32,
                       -IMAGE_BASE_VIRTUAL);
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
