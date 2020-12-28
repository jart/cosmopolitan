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
#include "libc/bits/bits.h"
#include "libc/bits/safemacros.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/fmt/conv.h"
#include "libc/log/check.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/nt/struct/filetime.h"
#include "libc/runtime/gc.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/x/x.h"
#include "libc/zip.h"
#include "tool/decode/lib/asmcodegen.h"
#include "tool/decode/lib/disassemblehex.h"
#include "tool/decode/lib/flagger.h"
#include "tool/decode/lib/idname.h"
#include "tool/decode/lib/ntfileflagnames.h"
#include "tool/decode/lib/titlegen.h"
#include "tool/decode/lib/zipnames.h"

/**
 * @fileoverview Zip File Disassembler.
 */

nodiscard char *formatdosdate(uint16_t dosdate) {
  return xasprintf("%04u-%02u-%02u", ((dosdate >> 9) & 0b1111111) + 1980,
                   (dosdate >> 5) & 0b1111, dosdate & 0b11111);
}

nodiscard char *formatdostime(uint16_t dostime) {
  return xasprintf("%02u:%02u:%02u", (dostime >> 11) & 0b11111,
                   (dostime >> 5) & 0b111111, (dostime << 1) & 0b111110);
}

void advancepos(uint8_t *map, size_t *pos, size_t off) {
  CHECK_GE(off, *pos);
  if (off > *pos) {
    printf("\n/\t<%s>\n", "LIMBO");
    disassemblehex(&map[*pos], off - *pos, stdout);
    printf("/\t</%s>\n", "LIMBO");
  }
  *pos = off;
}

void showgeneralflag(uint16_t generalflag) {
  puts("\
/	              ┌─utf8\n\
/	              │    ┌─strong encryption\n\
/	              │    │┌─compressed patch data\n\
/	              │    ││ ┌─crc and size go after file content\n\
/	              │    ││ │┌─{normal,max,fast,superfast}\n\
/	              │    ││ ││ ┌─encrypted\n\
/	          rrrr│uuuu││r│├┐│");
  show(".short", format(b1, "0b%016b", generalflag), "generalflag");
}

void showtimestamp(uint16_t time, uint16_t date) {
  show(".short", format(b1, "%#04hx", time),
       gc(xasprintf("%s (%s)", "lastmodifiedtime", gc(formatdostime(time)))));
  show(".short", format(b1, "%#04hx", date),
       gc(xasprintf("%s (%s)", "lastmodifieddate", gc(formatdosdate(date)))));
}

void showcompressmethod(uint16_t compressmethod) {
  show(".short",
       firstnonnull(findnamebyid(kZipCompressionNames, compressmethod),
                    format(b1, "%hu", compressmethod)),
       "compressionmethod");
}

void showextrantfs(uint8_t *ntfs) {
  struct timespec mtime, atime, ctime;
  mtime = FileTimeToTimeSpec(
      (struct NtFileTime){read32le(ntfs + 8), read32le(ntfs + 12)});
  atime = FileTimeToTimeSpec(
      (struct NtFileTime){read32le(ntfs + 16), read32le(ntfs + 20)});
  ctime = FileTimeToTimeSpec(
      (struct NtFileTime){read32le(ntfs + 24), read32le(ntfs + 28)});
  show(".long", gc(xasprintf("%d", read32le(ntfs))), "ntfs reserved");
  show(".short", gc(xasprintf("0x%04x", read16le(ntfs + 4))),
       "ntfs attribute tag value #1");
  show(".short", gc(xasprintf("%hu", read16le(ntfs + 6))),
       "ntfs attribute tag size");
  show(".quad", gc(xasprintf("%lu", read64le(ntfs + 8))),
       gc(xasprintf("%s (%s)", "ntfs last modified time",
                    gc(xiso8601(&mtime)))));
  show(".quad", gc(xasprintf("%lu", read64le(ntfs + 16))),
       gc(xasprintf("%s (%s)", "ntfs last access time", gc(xiso8601(&atime)))));
  show(".quad", gc(xasprintf("%lu", read64le(ntfs + 24))),
       gc(xasprintf("%s (%s)", "ntfs creation time", gc(xiso8601(&ctime)))));
}

void ShowExtendedTimestamp(uint8_t *p, size_t n, bool islocal) {
  int flag;
  int64_t x;
  struct timespec ts;
  flag = *p++;
  show(".byte", gc(xasprintf("0b%03hhb", flag)), "fields present in local");
  if (!islocal) {
    show(".quad", gc(xasprintf("%u", READ32LE(p))),
         gc(xasprintf("%s (%s)", "last modified", gc(xiso8601(&ts)))));
  } else {
    if (flag & 1) {
      ts = (struct timespec){READ32LE(p)};
      show(".quad", gc(xasprintf("%u", READ32LE(p))),
           gc(xasprintf("%s (%s)", "last modified", gc(xiso8601(&ts)))));
      p += 4;
    }
    flag >>= 1;
    if (flag & 1) {
      ts = (struct timespec){READ32LE(p)};
      show(".quad", gc(xasprintf("%u", READ32LE(p))),
           gc(xasprintf("%s (%s)", "access time", gc(xiso8601(&ts)))));
      p += 4;
    }
    flag >>= 1;
    if (flag & 1) {
      ts = (struct timespec){READ32LE(p)};
      show(".quad", gc(xasprintf("%u", READ32LE(p))),
           gc(xasprintf("%s (%s)", "creation time", gc(xiso8601(&ts)))));
    }
  }
}

void showextra(uint8_t *extra, bool islocal) {
  switch (ZIP_EXTRA_HEADERID(extra)) {
    case kZipExtraNtfs:
      showextrantfs(ZIP_EXTRA_CONTENT(extra));
      break;
    case kZipExtraExtendedTimestamp:
      ShowExtendedTimestamp(ZIP_EXTRA_CONTENT(extra),
                            ZIP_EXTRA_CONTENTSIZE(extra), islocal);
      break;
    case kZipExtraZip64:
      /* TODO */
    default:
      disassemblehex(ZIP_EXTRA_CONTENT(extra), ZIP_EXTRA_CONTENTSIZE(extra),
                     stdout);
      break;
  }
}

void showexternalattributes(uint8_t *cf) {
  uint32_t ea;
  ea = ZIP_CFILE_EXTERNALATTRIBUTES(cf);
  if (ZIP_CFILE_FILEATTRCOMPAT(cf) == kZipOsUnix) {
    show(".short", RecreateFlags(kNtFileFlagNames, ea & 0xffff),
         "dos file flags");
    show(".short", format(b1, "%#o", ea >> 16), "st_mode");
  } else {
    show(".long", RecreateFlags(kNtFileFlagNames, ea), "externalattributes");
  }
}

void showextras(uint8_t *extras, uint16_t extrassize, bool islocal) {
  int i;
  bool first;
  uint8_t *p, *pe;
  if (extrassize) {
    first = true;
    for (p = extras, pe = extras + extrassize, i = 0; p < pe;
         p += ZIP_EXTRA_SIZE(p), ++i) {
      show(".short",
           firstnonnull(findnamebyid(kZipExtraNames, ZIP_EXTRA_HEADERID(p)),
                        gc(xasprintf("0x%04hx", ZIP_EXTRA_HEADERID(p)))),
           gc(xasprintf("%s[%d].%s", "extras", i, "headerid")));
      show(".short", gc(xasprintf("%df-%df", (i + 2) * 10, (i + 1) * 10)),
           gc(xasprintf("%s[%d].%s (%hd %s)", "extras", i, "contentsize",
                        ZIP_EXTRA_CONTENTSIZE(p), "bytes")));
      if (first) {
        first = false;
        printf("%d:", (i + 1) * 10);
      }
      showextra(p, islocal);
      printf("%d:", (i + 2) * 10);
    }
  }
  putchar('\n');
}

void showlocalfileheader(uint8_t *lf, uint16_t idx) {
  printf("\n/\t%s #%hu (%zu %s)\n", "local file", idx + 1,
         ZIP_LFILE_HDRSIZE(lf), "bytes");
  show(".ascii", format(b1, "%`'.*s", 4, lf), "magic");
  show(".byte",
       firstnonnull(findnamebyid(kZipEraNames, ZIP_LFILE_VERSIONNEED(lf)),
                    gc(xasprintf("%d", ZIP_LFILE_VERSIONNEED(lf)))),
       "pkzip version need");
  show(".byte",
       firstnonnull(findnamebyid(kZipOsNames, ZIP_LFILE_OSNEED(lf)),
                    gc(xasprintf("%d", ZIP_LFILE_OSNEED(lf)))),
       "os need");
  showgeneralflag(ZIP_LFILE_GENERALFLAG(lf));
  showcompressmethod(ZIP_LFILE_COMPRESSIONMETHOD(lf));
  showtimestamp(ZIP_LFILE_LASTMODIFIEDTIME(lf), ZIP_LFILE_LASTMODIFIEDDATE(lf));
  show(".long", format(b1, "%#x", ZIP_LFILE_CRC32(lf)),
       gc(xasprintf(
           "%s (%#x)", "crc32z",
           crc32_z(0, ZIP_LFILE_CONTENT(lf), ZIP_LFILE_COMPRESSEDSIZE(lf)))));
  show(".long", "3f-2f",
       format(b1, "%s (%u %s)", "compressedsize", ZIP_LFILE_COMPRESSEDSIZE(lf),
              "bytes"));
  show(".long", format(b1, "%u", ZIP_LFILE_UNCOMPRESSEDSIZE(lf)),
       "uncompressedsize");
  show(".short", "1f-0f",
       format(b1, "%s (%hu %s)", "namesize", ZIP_LFILE_NAMESIZE(lf), "bytes"));
  show(
      ".short", "2f-1f",
      format(b1, "%s (%hu %s)", "extrasize", ZIP_LFILE_EXTRASIZE(lf), "bytes"));
  printf("0:");
  show(".ascii",
       format(b1, "%`'s",
              gc(strndup(ZIP_LFILE_NAME(lf), ZIP_LFILE_NAMESIZE(lf)))),
       "name");
  printf("1:");
  showextras(ZIP_LFILE_EXTRA(lf), ZIP_LFILE_EXTRASIZE(lf), true);
  printf("2:");
  disassemblehex(ZIP_LFILE_CONTENT(lf), ZIP_LFILE_COMPRESSEDSIZE(lf), stdout);
  printf("3:\n");
}

void showcentralfileheader(uint8_t *cf) {
  printf("\n/\t%s (%zu %s)\n", "central directory file header",
         ZIP_CFILE_HDRSIZE(cf), "bytes");
  show(".ascii", format(b1, "%`'.*s", 4, cf), "magic");
  show(".byte", gc(xasprintf("%d", ZIP_CFILE_VERSIONMADE(cf))), "version made");
  show(".byte",
       firstnonnull(findnamebyid(kZipOsNames, ZIP_CFILE_FILEATTRCOMPAT(cf)),
                    gc(xasprintf("%d", ZIP_CFILE_FILEATTRCOMPAT(cf)))),
       "file attr compat");
  show(".byte",
       firstnonnull(findnamebyid(kZipEraNames, ZIP_CFILE_VERSIONNEED(cf)),
                    gc(xasprintf("%d", ZIP_CFILE_VERSIONNEED(cf)))),
       "pkzip version need");
  show(".byte",
       firstnonnull(findnamebyid(kZipOsNames, ZIP_CFILE_OSNEED(cf)),
                    gc(xasprintf("%d", ZIP_CFILE_OSNEED(cf)))),
       "os need");
  showgeneralflag(ZIP_CFILE_GENERALFLAG(cf));
  showcompressmethod(ZIP_CFILE_COMPRESSIONMETHOD(cf));
  showtimestamp(ZIP_CFILE_LASTMODIFIEDTIME(cf), ZIP_CFILE_LASTMODIFIEDDATE(cf));
  show(".long", format(b1, "%#x", ZIP_CFILE_CRC32(cf)), "crc32z");
  show(".long", format(b1, "%u", ZIP_CFILE_COMPRESSEDSIZE(cf)),
       "compressedsize");
  show(".long", format(b1, "%u", ZIP_CFILE_UNCOMPRESSEDSIZE(cf)),
       "uncompressedsize");
  show(".short", "1f-0f",
       format(b1, "%s (%hu %s)", "namesize", ZIP_CFILE_NAMESIZE(cf), "bytes"));
  show(
      ".short", "2f-1f",
      format(b1, "%s (%hu %s)", "extrasize", ZIP_CFILE_EXTRASIZE(cf), "bytes"));
  show(".short", "3f-2f",
       format(b1, "%s (%hu %s)", "commentsize", ZIP_CFILE_COMMENTSIZE(cf),
              "bytes"));
  show(".short", format(b1, "%hu", ZIP_CFILE_DISK(cf)), "disk");
  show(".short",
       RecreateFlags(kZipIattrNames, ZIP_CFILE_INTERNALATTRIBUTES(cf)),
       "internalattributes");
  showexternalattributes(cf);
  show(".long", format(b1, "%u", ZIP_CFILE_OFFSET(cf)), "lfile hdr offset");
  printf("0:");
  show(".ascii",
       format(b1, "%`'s",
              gc(strndup(ZIP_CFILE_NAME(cf), ZIP_CFILE_NAMESIZE(cf)))),
       "name");
  printf("1:");
  showextras(ZIP_CFILE_EXTRA(cf), ZIP_CFILE_EXTRASIZE(cf), false);
  printf("2:");
  disassemblehex(ZIP_CFILE_COMMENT(cf), ZIP_CFILE_COMMENTSIZE(cf), stdout);
  printf("3:\n");
}

void showcentraldirheader(uint8_t *cd) {
  printf("\n/\t%s (%zu %s)\n", "end of central directory header",
         ZIP_CDIR_HDRSIZE(cd), "bytes");
  show(".ascii", format(b1, "%`'.*s", 4, cd), "magic");
  show(".short", format(b1, "%hd", ZIP_CDIR_DISK(cd)), "disk");
  show(".short", format(b1, "%hd", ZIP_CDIR_STARTINGDISK(cd)), "startingdisk");
  show(".short", format(b1, "%hu", ZIP_CDIR_RECORDSONDISK(cd)),
       "recordsondisk");
  show(".short", format(b1, "%hu", ZIP_CDIR_RECORDS(cd)), "records");
  show(".long", format(b1, "%u", ZIP_CDIR_SIZE(cd)), "size");
  show(".long", format(b1, "%u", ZIP_CDIR_OFFSET(cd)), "cfile hdrs offset");
  show(".short", "1f-0f",
       format(b1, "%s (%hu %s)", "commentsize", ZIP_CDIR_COMMENTSIZE(cd),
              "bytes"));
  printf("0:");
  disassemblehex(ZIP_CDIR_COMMENT(cd), ZIP_CDIR_COMMENTSIZE(cd), stdout);
  printf("1:\n");
}

void disassemblezip(uint8_t *map, size_t mapsize) {
  size_t pos;
  uint16_t i;
  static int records;
  uint8_t *cd, *cf, *lf;
  CHECK_NOTNULL((cd = zipfindcentraldir(map, mapsize)));
  pos = 0;
  records = ZIP_CDIR_RECORDS(cd);
  for (i = 0, cf = map + ZIP_CDIR_OFFSET(cd); i < records;
       ++i, cf += ZIP_CFILE_HDRSIZE(cf)) {
    lf = map + ZIP_CFILE_OFFSET(cf);
    CHECK_EQ(kZipLfileHdrMagic, ZIP_LFILE_MAGIC(lf));
    advancepos(map, &pos, lf - map);
    showlocalfileheader(lf, i);
    pos = (lf - map) + ZIP_LFILE_SIZE(lf);
  }
  for (i = 0, cf = map + ZIP_CDIR_OFFSET(cd); i < records;
       ++i, cf += ZIP_CFILE_HDRSIZE(cf)) {
    CHECK_EQ(kZipCfileHdrMagic, ZIP_CFILE_MAGIC(cf));
    advancepos(map, &pos, cf - map);
    showcentralfileheader(cf);
    pos = (cf - map) + ZIP_CFILE_HDRSIZE(cf);
  }
  advancepos(map, &pos, cd - map);
  showcentraldirheader(cd);
  pos = (cd - map) + ZIP_CDIR_HDRSIZE(cd);
  advancepos(map, &pos, mapsize);
}

int main(int argc, char *argv[]) {
  int fd;
  uint8_t *map;
  struct stat st;
  CHECK_EQ(2, argc);
  CHECK_NE(-1, (fd = open(argv[1], O_RDONLY)));
  CHECK_NE(-1, fstat(fd, &st));
  CHECK_GE(st.st_size, kZipCdirHdrMinSize);
  CHECK_NE(MAP_FAILED,
           (map = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0)));
  showtitle("αcτµαlly pδrταblε εxεcµταblε", "tool/decode/zip",
            basename(argv[1]), NULL, &kModelineAsm);
  disassemblezip(map, st.st_size);
  CHECK_NE(-1, munmap(map, st.st_size));
  CHECK_NE(-1, close(fd));
  return 0;
}
