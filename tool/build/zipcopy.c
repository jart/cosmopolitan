/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/magnumstrs.internal.h"
#include "libc/limits.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/zip.internal.h"
#include "third_party/getopt/getopt.h"

static int infd;
static int outfd;
static ssize_t insize;
static ssize_t outsize;
static const char *inpath;
static const char *outpath;
static unsigned char *inmap;

nullterminated() static void Print(int fd, const char *s, ...) {
  va_list va;
  char buf[2048];
  va_start(va, s);
  buf[0] = 0;
  do {
    strlcat(buf, s, sizeof(buf));
  } while ((s = va_arg(va, const char *)));
  write(fd, buf, strlen(buf));
  va_end(va);
}

static wontreturn void Die(const char *path, const char *reason) {
  Print(2, path, ": ", reason, "\n", NULL);
  exit(1);
}

static wontreturn void SysExit(const char *path, const char *func) {
  const char *errstr;
  if (!(errstr = _strerdoc(errno))) errstr = "EUNKNOWN";
  Print(2, path, ": ", func, " failed with ", errstr, "\n", NULL);
  exit(1);
}

static wontreturn void PrintUsage(int fd, int exitcode) {
  Print(fd, "\
NAME\n\
\n\
  Cosmopolitan Zip Copier\n\
\n\
SYNOPSIS\n\
\n\
  ",
        program_invocation_name, " [FLAGS] SRC DST\n\
\n\
DESCRIPTION\n\
\n\
  This tool copies the zip artifacts, if they exist, to the\n\
  end of the destination file. If no zip files exist within\n\
  the input file, then this tool performs no operation. The\n\
  output file will be created if it doesn't exist, which is\n\
  useful for creating new zip archives possibly to clean up\n\
  holes between input records. If the destination file does\n\
  exist, e.g. an executable, then any existing content will\n\
  be preserved, including an existing zip archive. That may\n\
  however lead to bloat that's not easy to access.\n\
\n\
FLAGS\n\
\n\
  -h            show this help\n\
\n\
EXAMPLE\n\
\n\
  objcopy -SO binary foo.com.dbg foo.com\n\
  zipcopy foo.com.dbg foo.com\n\
\n\
\n\
",
        NULL);
  exit(exitcode);
}

static void GetOpts(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, "h")) != -1) {
    switch (opt) {
      case 'h':
        PrintUsage(1, 0);
      default:
        PrintUsage(2, 1);
    }
  }
  if (optind + 2 != argc) {
    PrintUsage(2, 1);
  }
  inpath = argv[optind + 0];
  outpath = argv[optind + 1];
}

static void CopyZip(void) {
  int rela, recs;
  unsigned long ldest, cdest, ltotal, ctotal, length;
  unsigned char *ineof, *stop, *eocd, *cdir, *lfile, *cfile;

  // find zip eocd header
  ineof = (unsigned char *)inmap + insize;
  eocd = ineof - kZipCdirHdrMinSize;
  stop = MAX(eocd - 65536, inmap);
  for (;; --eocd) {
    if (eocd < stop) return;
    if (READ32LE(eocd) == kZipCdirHdrMagic) {
      break;
    }
  }

  // validate input
  recs = ltotal = ctotal = 0;
  cdir = inmap + ZIP_CDIR_OFFSET(eocd);
  stop = cdir + ZIP_CDIR_SIZE(eocd);
  for (cfile = cdir; cfile < stop; cfile += ZIP_CFILE_HDRSIZE(cfile)) {
    if (++recs >= 65536) {
      Die(inpath, "too many zip files");
    }
    if (cfile < inmap ||                        //
        cfile + kZipCfileHdrMinSize > ineof ||  //
        cfile + ZIP_CFILE_HDRSIZE(cfile) > ineof) {
      Die(inpath, "zip directory overflow");
    }
    if (READ32LE(cfile) != kZipCfileHdrMagic) {
      Die(inpath, "zip directory corrupted");
    }
    if ((rela = ZIP_CFILE_OFFSET(cfile)) < 0) {
      Die(inpath, "zip directory offset negative or zip64");
    }
    lfile = inmap + rela;
    if (lfile < inmap ||                        //
        lfile + kZipLfileHdrMinSize > ineof ||  //
        lfile + ZIP_LFILE_SIZE(lfile) > ineof) {
      Die(inpath, "zip local file overflow");
    }
    if (READ32LE(lfile) != kZipLfileHdrMagic) {
      Die(inpath, "zip local file corrupted");
    }
    ctotal += ZIP_CFILE_HDRSIZE(cfile);
    ltotal += ZIP_LFILE_SIZE(lfile);
  }
  if (outsize + ltotal + ctotal + ZIP_CDIR_HDRSIZE(eocd) > INT_MAX) {
    Die(outpath, "the time has come to upgrade to zip64");
  }

  // write output
  if ((outfd = open(outpath, O_WRONLY | O_CREAT, 0644)) == -1) {
    SysExit(outpath, "open");
  }
  if ((outsize = lseek(outfd, 0, SEEK_END)) == -1) {
    SysExit(outpath, "lseek");
  }
  ldest = outsize;
  cdest = outsize + ltotal;
  for (cfile = cdir; cfile < stop; cfile += ZIP_CFILE_HDRSIZE(cfile)) {
    lfile = inmap + ZIP_CFILE_OFFSET(cfile);
    WRITE32LE(cfile + kZipCfileOffsetOffset, ldest);
    // write local file
    length = ZIP_LFILE_SIZE(lfile);
    if (pwrite(outfd, lfile, length, ldest) != length) {
      SysExit(outpath, "lfile pwrite");
    }
    ldest += length;
    // write directory entry
    length = ZIP_CFILE_HDRSIZE(cfile);
    if (READ32LE(cfile) != kZipCfileHdrMagic) notpossible;
    if (pwrite(outfd, cfile, length, cdest) != length) {
      SysExit(outpath, "lfile pwrite");
    }
    cdest += length;
  }
  if (ldest != outsize + ltotal) __builtin_trap();
  if (cdest != outsize + ltotal + ctotal) __builtin_trap();
  WRITE32LE(eocd + kZipCdirOffsetOffset, outsize + ltotal);
  length = ZIP_CDIR_HDRSIZE(eocd);
  if (pwrite(outfd, eocd, length, cdest) != length) {
    SysExit(outpath, "eocd pwrite");
  }
  if (close(outfd)) {
    SysExit(outpath, "close");
  }
}

int main(int argc, char *argv[]) {
  int i, opt;
  if (!IsOptimized()) {
    ShowCrashReports();
  }
  GetOpts(argc, argv);
  if ((infd = open(inpath, O_RDONLY)) == -1) {
    SysExit(inpath, "open");
  }
  if ((insize = lseek(infd, 0, SEEK_END)) == -1) {
    SysExit(inpath, "lseek");
  }
  if (!insize) {
    Die(inpath, "file is empty");
  }
  if ((inmap = mmap(0, insize, PROT_READ | PROT_WRITE, MAP_PRIVATE, infd, 0)) ==
      MAP_FAILED) {
    SysExit(inpath, "mmap");
  }
  CopyZip();
  if (munmap(inmap, insize)) {
    SysExit(inpath, "munmap");
  }
  if (close(infd)) {
    SysExit(inpath, "close");
  }
}
