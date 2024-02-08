/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/fmt/libgen.h"
#include "libc/fmt/wintime.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/nt/struct/filetime.h"
#include "libc/serialize.h"
#include "libc/stdckdint.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/sysparam.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/x/xasprintf.h"
#include "libc/x/xiso8601.h"
#include "libc/zip.internal.h"
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

uint8_t *map;

static __wur char *FormatDosDate(uint16_t dosdate) {
  return xasprintf("%04u-%02u-%02u", ((dosdate >> 9) & 0b1111111) + 1980,
                   (dosdate >> 5) & 0b1111, dosdate & 0b11111);
}

static __wur char *FormatDosTime(uint16_t dostime) {
  return xasprintf("%02u:%02u:%02u", (dostime >> 11) & 0b11111,
                   (dostime >> 5) & 0b111111, (dostime << 1) & 0b111110);
}

void AdvancePosition(uint8_t *map, size_t *pos, size_t off) {
  if (off > *pos) {
    /* printf("\n/\t<%s>\n", "LIMBO"); */
    /* disassemblehex(&map[*pos], off - *pos, stdout); */
    /* printf("/\t</%s>\n", "LIMBO"); */
  }
  *pos = off;
}

void ShowGeneralFlag(uint16_t generalflag) {
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

void ShowTimestamp(uint16_t time, uint16_t date) {
  show(".short", format(b1, "%#04hx", time),
       gc(xasprintf("%s (%s)", "lastmodifiedtime", gc(FormatDosTime(time)))));
  show(".short", format(b1, "%#04hx", date),
       gc(xasprintf("%s (%s)", "lastmodifieddate", gc(FormatDosDate(date)))));
}

void ShowCompressionMethod(uint16_t compressmethod) {
  show(".short",
       firstnonnull(findnamebyid(kZipCompressionNames, compressmethod),
                    format(b1, "%hu", compressmethod)),
       "compressionmethod");
}

void ShowNtfs(uint8_t *ntfs, size_t n) {
  struct timespec mtime, atime, ctime;
  mtime = WindowsTimeToTimeSpec(READ64LE(ntfs + 8));
  atime = WindowsTimeToTimeSpec(READ64LE(ntfs + 16));
  ctime = WindowsTimeToTimeSpec(READ64LE(ntfs + 24));
  show(".long", gc(xasprintf("%d", READ32LE(ntfs))), "ntfs reserved");
  show(".short", gc(xasprintf("0x%04x", READ16LE(ntfs + 4))),
       "ntfs attribute tag value #1");
  show(".short", gc(xasprintf("%hu", READ16LE(ntfs + 6))),
       "ntfs attribute tag size");
  show(".quad", gc(xasprintf("%lu", READ64LE(ntfs + 8))),
       gc(xasprintf("%s (%s)", "ntfs last modified time",
                    gc(xiso8601(&mtime)))));
  show(".quad", gc(xasprintf("%lu", READ64LE(ntfs + 16))),
       gc(xasprintf("%s (%s)", "ntfs last access time", gc(xiso8601(&atime)))));
  show(".quad", gc(xasprintf("%lu", READ64LE(ntfs + 24))),
       gc(xasprintf("%s (%s)", "ntfs creation time", gc(xiso8601(&ctime)))));
}

void ShowExtendedTimestamp(uint8_t *p, size_t n, bool islocal) {
  int flag;
  if (n) {
    --n;
    flag = *p++;
    show(".byte", gc(xasprintf("0b%03hhb", flag)), "fields present in local");
    if ((flag & 1) && n >= 4) {
      show(".long", gc(xasprintf("%u", READ32LE(p))),
           gc(xasprintf("%s (%s)", "last modified",
                        gc(xiso8601(&(struct timespec){READ32LE(p)})))));
      p += 4;
      n -= 4;
    }
    flag >>= 1;
    if (islocal) {
      if ((flag & 1) && n >= 4) {
        show(".long", gc(xasprintf("%u", READ32LE(p))),
             gc(xasprintf("%s (%s)", "access time",
                          gc(xiso8601(&(struct timespec){READ32LE(p)})))));
        p += 4;
        n -= 4;
      }
      flag >>= 1;
      if ((flag & 1) && n >= 4) {
        show(".long", gc(xasprintf("%u", READ32LE(p))),
             gc(xasprintf("%s (%s)", "creation time",
                          gc(xiso8601(&(struct timespec){READ32LE(p)})))));
        p += 4;
        n -= 4;
      }
    }
  }
}

void ShowZip64(uint8_t *lf, uint8_t *p, size_t n, bool islocal) {
  int i = 0;

  uint32_t uncompsize;
  if (islocal) {
    uncompsize = ZIP_LFILE_UNCOMPRESSEDSIZE(lf);
  } else {
    uncompsize = ZIP_CFILE_UNCOMPRESSEDSIZE(lf);
  }
  if (uncompsize == 0xffffffffu) {
    if (i + 8 <= n) {
      show(".quad", gc(xasprintf("0x%lx", READ64LE(p + i))),
           gc(xasprintf("uncompressed size (%,ld)", READ64LE(p + i))));
    } else {
      kprintf("/\tWARNING: ZIP64 EXTRA MISSING UNCOMPRESSED SIZE\n");
    }
    i += 8;
  }

  uint32_t compsize;
  if (islocal) {
    compsize = ZIP_LFILE_COMPRESSEDSIZE(lf);
  } else {
    compsize = ZIP_CFILE_COMPRESSEDSIZE(lf);
  }
  if (compsize == 0xffffffffu) {
    if (i + 8 <= n) {
      show(".quad", gc(xasprintf("0x%lx", READ64LE(p + i))),
           gc(xasprintf("compressed size (%,ld)", READ64LE(p + i))));
    } else {
      kprintf("/\tWARNING: ZIP64 EXTRA MISSING COMPRESSED SIZE\n");
    }
    i += 8;
  }

  if (!islocal) {
    uint32_t offset;
    offset = ZIP_CFILE_OFFSET(lf);
    if (offset == 0xffffffffu) {
      if (i + 8 <= n) {
        show(".quad", gc(xasprintf("0x%lx", READ64LE(p + i))),
             gc(xasprintf("lfile offset (%,ld)", READ64LE(p + i))));
      } else {
        kprintf("/\tWARNING: ZIP64 EXTRA MISSING OFFSET\n");
      }
      i += 8;
    }
  }

  if (!islocal) {
    uint16_t disk;
    disk = ZIP_CFILE_DISK(lf);
    if (disk == 0xffff) {
      if (i + 4 <= n) {
        show(".long", gc(xasprintf("0x%x", READ32LE(p + i))),
             gc(xasprintf("lfile disk (%,ld)", READ32LE(p + i))));
      } else {
        kprintf("/\tWARNING: ZIP64 EXTRA MISSING DISK\n");
      }
      i += 8;
    }
  }

  if (i != n) {
    kprintf("/\tWARNING: ZIP64 EXTRA HAS SUPERFLUOUS CONTENT\n");
  }
}

void ShowInfoZipNewUnixExtra(uint8_t *p, size_t n, bool islocal) {
  if (p[0] == 1 && p[1] == 4 && p[6] == 4) {
    show(".byte", "1", "version");
    show(".byte", "4", "uid length");
    show(".long", gc(xasprintf("%u", READ32LE(p + 2))), "uid");
    show(".byte", "4", "gid length");
    show(".long", gc(xasprintf("%u", READ32LE(p + 7))), "gid");
  } else {
    disassemblehex(p, n, stdout);
  }
}

void ShowExtra(uint8_t *lf, uint8_t *extra, bool islocal) {
  switch (ZIP_EXTRA_HEADERID(extra)) {
    case kZipExtraNtfs:
      ShowNtfs(ZIP_EXTRA_CONTENT(extra), ZIP_EXTRA_CONTENTSIZE(extra));
      break;
    case kZipExtraExtendedTimestamp:
      ShowExtendedTimestamp(ZIP_EXTRA_CONTENT(extra),
                            ZIP_EXTRA_CONTENTSIZE(extra), islocal);
      break;
    case kZipExtraZip64:
      ShowZip64(lf, ZIP_EXTRA_CONTENT(extra), ZIP_EXTRA_CONTENTSIZE(extra),
                islocal);
      break;
    case kZipExtraInfoZipNewUnixExtra:
      ShowInfoZipNewUnixExtra(ZIP_EXTRA_CONTENT(extra),
                              ZIP_EXTRA_CONTENTSIZE(extra), islocal);
      break;
    default:
      disassemblehex(ZIP_EXTRA_CONTENT(extra), ZIP_EXTRA_CONTENTSIZE(extra),
                     stdout);
      break;
  }
}

void ShowExternalAttributes(uint8_t *cf) {
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

void ShowExtras(uint8_t *lf, uint8_t *extras, uint16_t extrassize,
                bool islocal) {
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
      ShowExtra(lf, p, islocal);
      printf("%d:", (i + 2) * 10);
    }
  }
  putchar('\n');
}

void ShowLocalFileHeader(uint8_t *lf, uint16_t idx) {
  printf("\n/\t%s #%hu (%zu %s @ %#lx)\n", "local file", idx + 1,
         ZIP_LFILE_HDRSIZE(lf), "bytes", lf - map);
  show(".ascii", format(b1, "%`'.*s", 4, lf), "magic");
  show(".byte",
       firstnonnull(findnamebyid(kZipEraNames, ZIP_LFILE_VERSIONNEED(lf)),
                    gc(xasprintf("%d", ZIP_LFILE_VERSIONNEED(lf)))),
       "pkzip version need");
  show(".byte",
       firstnonnull(findnamebyid(kZipOsNames, ZIP_LFILE_OSNEED(lf)),
                    gc(xasprintf("%d", ZIP_LFILE_OSNEED(lf)))),
       "os need");
  ShowGeneralFlag(ZIP_LFILE_GENERALFLAG(lf));
  ShowCompressionMethod(ZIP_LFILE_COMPRESSIONMETHOD(lf));
  ShowTimestamp(ZIP_LFILE_LASTMODIFIEDTIME(lf), ZIP_LFILE_LASTMODIFIEDDATE(lf));
  show(
      ".long",
      format(b1, "%#x", ZIP_LFILE_CRC32(lf)), gc(xasprintf("%s (%#x)", "crc32z", GetZipLfileCompressedSize(lf) /* crc32_z(0, ZIP_LFILE_CONTENT(lf), GetZipLfileCompressedSize(lf)) */)));
  if (ZIP_LFILE_COMPRESSEDSIZE(lf) == 0xFFFFFFFF) {
    show(".long", "0xFFFFFFFF", "compressedsize (zip64)");
  } else {
    show(".long", "3f-2f",
         format(b1, "%s (%u %s)", "compressedsize",
                ZIP_LFILE_COMPRESSEDSIZE(lf), "bytes"));
  }
  show(".long",
       ZIP_LFILE_UNCOMPRESSEDSIZE(lf) == 0xFFFFFFFF
           ? "0xFFFFFFFF"
           : format(b1, "%u", ZIP_LFILE_UNCOMPRESSEDSIZE(lf)),
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
  ShowExtras(lf, ZIP_LFILE_EXTRA(lf), ZIP_LFILE_EXTRASIZE(lf), true);
  printf("2:");
  /* disassemblehex(ZIP_LFILE_CONTENT(lf), ZIP_LFILE_COMPRESSEDSIZE(lf),
   * stdout); */
  printf("3:\n");
}

void ShowCentralFileHeader(uint8_t *cf) {
  printf("\n/\t%s (%zu %s @ %#lx)\n", "central directory file header",
         ZIP_CFILE_HDRSIZE(cf), "bytes", cf - map);
  show(".ascii", format(b1, "%`'.*s", 4, cf), "magic");
  show(".byte", gc(xasprintf("%d", ZIP_CFILE_VERSIONMADE(cf))),
       "zip version made");
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
  ShowGeneralFlag(ZIP_CFILE_GENERALFLAG(cf));
  ShowCompressionMethod(ZIP_CFILE_COMPRESSIONMETHOD(cf));
  ShowTimestamp(ZIP_CFILE_LASTMODIFIEDTIME(cf), ZIP_CFILE_LASTMODIFIEDDATE(cf));
  show(".long", format(b1, "%#x", ZIP_CFILE_CRC32(cf)), "crc32z");
  if (ZIP_CFILE_COMPRESSEDSIZE(cf) == 0xFFFFFFFF) {
    show(".long", "0xFFFFFFFF", "compressedsize (zip64)");
  } else {
    show(".long", format(b1, "%u", ZIP_CFILE_COMPRESSEDSIZE(cf)),
         "compressedsize");
  }
  if (ZIP_CFILE_UNCOMPRESSEDSIZE(cf) == 0xFFFFFFFF) {
    show(".long", "0xFFFFFFFF", "compressedsize (zip64)");
  } else {
    show(".long", format(b1, "%u", ZIP_CFILE_UNCOMPRESSEDSIZE(cf)),
         "uncompressedsize");
  }
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
       RecreateFlags(kZipIattrNames, ZIP_CFILE_INTERNALATTRIBUTES(cf) & 1),
       "internalattributes");
  ShowExternalAttributes(cf);
  if (ZIP_CFILE_OFFSET(cf) == 0xFFFFFFFF) {
    show(".long", "0xFFFFFFFF", "lfile hdr offset (zip64)");
  } else {
    show(".long", format(b1, "0x%x", ZIP_CFILE_OFFSET(cf)), "lfile hdr offset");
  }
  printf("0:");
  show(".ascii",
       format(b1, "%`'s",
              gc(strndup(ZIP_CFILE_NAME(cf), ZIP_CFILE_NAMESIZE(cf)))),
       "name");
  printf("1:");
  ShowExtras(cf, ZIP_CFILE_EXTRA(cf), ZIP_CFILE_EXTRASIZE(cf), false);
  printf("2:");
  show(".ascii",
       format(b1, "%`'.*s", ZIP_CFILE_COMMENTSIZE(cf), ZIP_CFILE_COMMENT(cf)),
       "comment");
  printf("3:\n");
}

void ShowCentralDirHeader32(uint8_t *cd) {
  printf("\n/\t%s (%zu %s @ %#lx)\n", "end of central directory header",
         ZIP_CDIR_HDRSIZE(cd), "bytes", cd - map);
  show(".ascii", format(b1, "%`'.*s", 4, cd), "magic");
  show(".short", format(b1, "%hd", ZIP_CDIR_DISK(cd)), "disk");
  show(".short", format(b1, "%hd", ZIP_CDIR_STARTINGDISK(cd)), "startingdisk");
  show(".short", format(b1, "%hu", ZIP_CDIR_RECORDSONDISK(cd)),
       "recordsondisk");
  show(".short", format(b1, "%hu", ZIP_CDIR_RECORDS(cd)), "records");
  show(".long", format(b1, "%u", ZIP_CDIR_SIZE(cd)), "size");
  show(".long", format(b1, "0x%x", ZIP_CDIR_OFFSET(cd)), "cfile hdrs offset");
  show(".short", "1f-0f",
       format(b1, "%s (%hu %s)", "commentsize", ZIP_CDIR_COMMENTSIZE(cd),
              "bytes"));
  printf("0:");
  disassemblehex(ZIP_CDIR_COMMENT(cd), ZIP_CDIR_COMMENTSIZE(cd), stdout);
  printf("1:\n");
}

void ShowCentralDirHeader64(uint8_t *cd) {
  printf("\n/\t%s (%zu %s @ %#lx)\n", "zip64 end of central directory header",
         ZIP_CDIR64_HDRSIZE(cd), "bytes", cd - map);
  show(".ascii", format(b1, "%`'.*s", 4, cd), "magic");
  show(".quad", format(b1, "%lu", ZIP_CDIR64_HDRSIZE(cd) - 12), "hdr size");
  show(".short", format(b1, "%hd", ZIP_CDIR64_VERSIONMADE(cd)), "version made");
  show(".short", format(b1, "%hd", ZIP_CDIR64_VERSIONNEED(cd)), "version need");
  show(".long", format(b1, "%d", ZIP_CDIR64_DISK(cd)), "disk");
  show(".long", format(b1, "%d", ZIP_CDIR64_STARTINGDISK(cd)), "startingdisk");
  show(".quad", format(b1, "%lu", ZIP_CDIR64_RECORDSONDISK(cd)),
       "recordsondisk");
  show(".quad", format(b1, "%lu", ZIP_CDIR64_RECORDS(cd)), "records");
  show(".quad", format(b1, "%lu", ZIP_CDIR64_SIZE(cd)), "cdir size");
  show(".quad", format(b1, "0x%lx", ZIP_CDIR64_OFFSET(cd)), "cdir offset");
  printf("0:");
  disassemblehex(ZIP_CDIR64_COMMENT(cd), ZIP_CDIR64_COMMENTSIZE(cd), stdout);
  printf("1:\n");
  cd += ZIP_CDIR64_HDRSIZE(cd);
  printf("\n/\t%s (%zu %s)\n", "zip64 end of central directory locator",
         kZipCdir64LocatorSize, "bytes");
  show(".ascii", format(b1, "%`'.*s", 4, cd), "magic");
  show(".long", format(b1, "%d", READ32LE(cd + 4)), "startingdisk");
  show(".quad", format(b1, "0x%lx", READ64LE(cd + 4 + 4)), "eocd64 offset");
  show(".long", format(b1, "%d", READ32LE(cd + 4 + 4 + 8)), "totaldisks");
}

int IsZipEocd32(const uint8_t *p, size_t n, size_t i) {
  size_t offset;
  if (i > n || n - i < kZipCdirHdrMinSize) {
    return kZipErrorEocdOffsetOverflow;
  }
  if (READ32LE(p + i) != kZipCdirHdrMagic) {
    return kZipErrorEocdMagicNotFound;
  }
  if (i + ZIP_CDIR_HDRSIZE(p + i) > n) {
    return kZipErrorEocdSizeOverflow;
  }
  if (ZIP_CDIR_DISK(p + i) != ZIP_CDIR_STARTINGDISK(p + i)) {
    return kZipErrorEocdDiskMismatch;
  }
  if (ZIP_CDIR_RECORDSONDISK(p + i) != ZIP_CDIR_RECORDS(p + i)) {
    return kZipErrorEocdRecordsMismatch;
  }
  if (ZIP_CDIR_RECORDS(p + i) * kZipCfileHdrMinSize > ZIP_CDIR_SIZE(p + i)) {
    return kZipErrorEocdRecordsOverflow;
  }
  if (ckd_add(&offset, ZIP_CDIR_OFFSET(p + i), ZIP_CDIR_SIZE(p + i))) {
    return kZipErrorEocdOffsetSizeOverflow;
  }
  if (offset > i) {
    return kZipErrorCdirOffsetPastEocd;
  }
  return kZipOk;
}

uint8_t *GetZipCdir32(const uint8_t *p, size_t n) {
  int64_t i, e, err;
  if (n >= kZipCdirHdrMinSize) {
    i = n - kZipCdirHdrMinSize;
    e = MAX(0, n - 65536);
    for (; i >= e; --i) {
      if (READ32LE(p + i) == kZipCdirHdrMagic) {
        if ((err = IsZipEocd32(p, n, i)) == kZipOk) {
          return (/*unconst*/ uint8_t *)(p + i);
        }
        kprintf("warning: found eocd32 magic at offset 0x%lx that didn't look "
                "like an eocd32 record\n",
                i);
      }
    }
  }
  return NULL;
}

uint8_t *GetZipCdir64(const uint8_t *p, size_t n) {
  int64_t e, i, j;
  if (n >= kZipCdir64LocatorSize) {
    i = n - kZipCdir64LocatorSize;
    e = MAX(0, n - 65536);
    for (; i >= e; --i) {
      if (READ32LE(p + i) == kZipCdir64LocatorMagic) {
        if ((j = ZIP_LOCATE64_OFFSET(p + i)) + kZipCdir64HdrMinSize <= n) {
          CHECK_EQ(kZipCdir64HdrMagic, ZIP_CDIR64_MAGIC((uint8_t *)p + j));
          return (uint8_t *)p + j;
        }
        kprintf("warning: found eocd64 locator magic that "
                "didn't look like an eocd64 locator record\n");
      }
    }
  }
  return NULL;
}

void DisassembleZip(const char *path, uint8_t *p, size_t n) {
  size_t pos;
  uint16_t i;
  static int records;
  uint8_t *eocd32, *eocd64, *cdir, *cf, *lf;
  eocd32 = GetZipCdir32(p, n);
  eocd64 = GetZipCdir64(p, n);
  CHECK(eocd32 || eocd64);
  pos = 0;
  if (eocd64) {
    records = ZIP_CDIR64_RECORDS(eocd64);
    cdir = p + ZIP_CDIR64_OFFSET(eocd64);
  } else {
    records = ZIP_CDIR_RECORDS(eocd32);
    cdir = p + ZIP_CDIR_OFFSET(eocd32);
  }
  for (i = 0, cf = cdir; i < records; ++i, cf += ZIP_CFILE_HDRSIZE(cf)) {
    lf = p + GetZipCfileOffset(cf);
    CHECK_EQ(kZipLfileHdrMagic, ZIP_LFILE_MAGIC(lf));
    AdvancePosition(p, &pos, lf - p);
    ShowLocalFileHeader(lf, i);
    pos = (lf - p) + ZIP_LFILE_SIZE(lf);
  }
  for (i = 0, cf = cdir; i < records; ++i, cf += ZIP_CFILE_HDRSIZE(cf)) {
    CHECK_EQ(kZipCfileHdrMagic, ZIP_CFILE_MAGIC(cf));
    AdvancePosition(p, &pos, cf - p);
    ShowCentralFileHeader(cf);
    pos = (cf - p) + ZIP_CFILE_HDRSIZE(cf);
  }
  /* TODO(jart): This is absurd. */
  if (eocd32 && eocd64) {
    if (eocd32 < eocd64) {
      ShowCentralDirHeader32(eocd32);
      AdvancePosition(p, &pos, eocd32 - p);
      ShowCentralDirHeader64(eocd64);
      AdvancePosition(p, &pos, eocd64 - p);
    } else {
      /* pos = eocd64 - p + ZIP_CDIR_HDRSIZE(eocd64); */
      /* AdvancePosition(p, &pos, n); */
      ShowCentralDirHeader64(eocd64);
      AdvancePosition(p, &pos, eocd64 - p);
      /* pos = eocd32 - p + ZIP_CDIR_HDRSIZE(eocd32); */
      /* AdvancePosition(p, &pos, n); */
      ShowCentralDirHeader32(eocd32);
      AdvancePosition(p, &pos, eocd32 - p);
    }
  } else if (eocd32) {
    ShowCentralDirHeader32(eocd32);
    AdvancePosition(p, &pos, eocd32 - p);
  } else {
    ShowCentralDirHeader64(eocd64);
    AdvancePosition(p, &pos, eocd64 - p);
  }
  pos = n;
  AdvancePosition(p, &pos, n);
}

int main(int argc, char *argv[]) {
  int fd;
  struct stat st;
  ShowCrashReports();
  CHECK_EQ(2, argc);
  CHECK_NE(-1, (fd = open(argv[1], O_RDONLY)));
  CHECK_NE(-1, fstat(fd, &st));
  CHECK_GE(st.st_size, kZipCdirHdrMinSize);
  CHECK_NE(MAP_FAILED,
           (map = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0)));
  showtitle("αcτµαlly pδrταblε εxεcµταblε", "tool/decode/zip",
            basename(argv[1]), NULL, &kModelineAsm);
  DisassembleZip(argv[1], map, st.st_size);
  CHECK_NE(-1, munmap(map, st.st_size));
  CHECK_NE(-1, close(fd));
  return 0;
}
