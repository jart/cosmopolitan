/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/fmt/magnumstrs.internal.h"
#include "libc/limits.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/str/tab.internal.h"
#include "third_party/getopt/getopt.internal.h"
#include "third_party/mbedtls/sha256.h"

#define USAGE \
  "[-?hbctw] [PATH...]\n\
  -h          help\n\
  -c          check mode\n\
  -b          binary mode\n\
  -t          textual mode\n\
  -w          warning mode\n\
\n\
cosmopolitan sha256sum v1.1\n\
copyright 2022 justine alexandra roberts tunney\n\
notice licenses are embedded in the binary\n\
https://twitter.com/justinetunney\n\
https://github.com/jart\n\
\n\
This program is an Actually Portable Executable (APE) rewrite of the\n\
GNU coreutils sha256sum command. Our version is better because it goes\n\
400% faster than coreutils if the Intel® SHA-NI™ ISA is available,\n\
otherwise this command goes 50% faster than coreutils. This executable\n\
will work consistently on Linux/Mac/Windows/FreeBSD/NetBSD/OpenBSD, so\n\
consider vendoring it in your repo to avoid platform portability toil.\n\
"

static bool g_warn;
static char g_mode;
static bool g_check;
static int g_mismatches;
static const char *prog;

static wontreturn void PrintUsage(int rc, int fd) {
  tinyprint(fd, "Usage: ", prog, USAGE, NULL);
  exit(rc);
}

static void GetOpts(int argc, char *argv[]) {
  int opt;
  g_mode = ' ';
  while ((opt = getopt(argc, argv, "hbctw")) != -1) {
    switch (opt) {
      case 'w':
        g_warn = true;
        break;
      case 'c':
        g_check = true;
        break;
      case 't':
        g_mode = ' ';
        break;
      case 'b':
        g_mode = '*';
        break;
      case 'h':
        PrintUsage(0, 1);
      default:
        PrintUsage(1, 2);
    }
  }
}

static bool IsModeCharacter(char c) {
  switch (c) {
    case ' ':
    case '*':
      return true;
    default:
      return false;
  }
}

static bool IsSupportedPath(const char *path) {
  size_t i;
  for (i = 0;; ++i) {
    switch (path[i]) {
      case 0:
        if (i) return true;
        // fallthrough
      case '\r':
      case '\n':
      case '\\':
        tinyprint(2, prog, ": ", path, ": unsupported path\n", NULL);
        return false;
      default:
        break;
    }
  }
}

static bool GetDigest(const char *path, FILE *f, unsigned char digest[32]) {
  size_t got;
  unsigned char buf[512];
  mbedtls_sha256_context ctx;
  mbedtls_sha256_init(&ctx);
  unassert(!mbedtls_sha256_starts_ret(&ctx, false));
  while ((got = fread(buf, 1, sizeof(buf), f))) {
    unassert(!mbedtls_sha256_update_ret(&ctx, buf, got));
  }
  if (ferror(f)) {
    tinyprint(2, prog, ": ", path, ": ", strerror(errno), "\n", NULL);
    return false;
  }
  unassert(!mbedtls_sha256_finish_ret(&ctx, digest));
  mbedtls_sha256_free(&ctx);
  return true;
}

static bool ProduceDigest(const char *path, FILE *f) {
  char hexdigest[65];
  char mode[2] = {g_mode};
  unsigned char digest[32];
  if (!IsSupportedPath(path)) return false;
  if (!GetDigest(path, f, digest)) return false;
  hexpcpy(hexdigest, digest, 32);
  tinyprint(1, hexdigest, " ", mode, path, "\n", NULL);
  return true;
}

static bool CheckDigests(const char *path, FILE *f) {
  FILE *f2;
  bool k = true;
  int a, b, i, line;
  const char *path2, *status;
  unsigned char wantdigest[32], gotdigest[32];
  char buf[64 + 2 + PATH_MAX + 1 + 1], *p;
  for (line = 0; fgets(buf, sizeof(buf), f); ++line) {
    if (!*chomp(buf)) continue;
    for (p = buf, i = 0; i < 32; ++i) {
      if ((a = kHexToInt[*p++ & 255]) == -1) goto InvalidLine;
      if ((b = kHexToInt[*p++ & 255]) == -1) goto InvalidLine;
      wantdigest[i] = a << 4 | b;
    }
    if (*p++ != ' ') goto InvalidLine;
    if (!IsModeCharacter(*p++)) goto InvalidLine;
    path2 = p;
    if (!*path2) goto InvalidLine;
    if (!IsSupportedPath(path2)) continue;
    if ((f2 = fopen(path2, "rb"))) {
      if (GetDigest(path2, f2, gotdigest)) {
        if (!memcmp(wantdigest, gotdigest, 32)) {
          status = "OK";
        } else {
          status = "FAILED";
          ++g_mismatches;
          k = false;
        }
        tinyprint(1, path2, ": ", status, "\n", NULL);
      } else {
        k = false;
      }
      fclose(f2);
    } else {
      tinyprint(2, prog, ": ", path2, ": ", strerror(errno), "\n", NULL);
      k = false;
    }
    continue;
  InvalidLine:
    if (g_warn) {
      char linestr[12];
      FormatInt32(linestr, line + 1);
      tinyprint(2, prog, ": ", path, ":", linestr, ": ",
                "improperly formatted checksum line", "\n", NULL);
    }
  }
  if (ferror(f)) {
    tinyprint(2, prog, ": ", path, ": ", strerror(errno), "\n", NULL);
    k = false;
  }
  return k;
}

static bool Process(const char *path, FILE *f) {
  if (g_check) {
    return CheckDigests(path, f);
  } else {
    return ProduceDigest(path, f);
  }
}

int main(int argc, char *argv[]) {
  int i;
  FILE *f;
  bool k = true;
  prog = argv[0];
  if (!prog) prog = "sha256sum";
  GetOpts(argc, argv);
  if (optind == argc) {
    f = stdin;
    k &= Process("-", f);
  } else {
    for (i = optind; i < argc; ++i) {
      if ((f = fopen(argv[i], "rb"))) {
        k &= Process(argv[i], f);
        fclose(f);
      } else {
        tinyprint(2, prog, ": ", argv[i], ": ", strerror(errno), "\n", NULL);
        k = false;
      }
    }
  }
  if (g_mismatches) {
    char ibuf[12];
    FormatInt32(ibuf, g_mismatches);
    tinyprint(2, prog, ": WARNING: ", ibuf,
              " computed checksum did NOT match\n", NULL);
  }
  return !k;
}
