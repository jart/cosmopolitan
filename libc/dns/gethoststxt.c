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
#include "libc/bits/pushpop.h"
#include "libc/bits/safemacros.h"
#include "libc/dce.h"
#include "libc/dns/hoststxt.h"
#include "libc/fmt/fmt.h"
#include "libc/macros.h"
#include "libc/nt/systeminfo.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"

static struct HostsTxt *g_hoststxt;
static struct HostsTxtInitialStaticMemory {
  struct HostsTxt ht;
  struct HostsTxtEntry entries[8];
  char strings[64];
} g_hoststxt_init;

static textwindows noinline char *getnthoststxtpath(char *pathbuf,
                                                    uint32_t size) {
  const char *const kWinHostsPath = "\\drivers\\etc\\hosts";
  uint32_t len = GetSystemDirectoryA(&pathbuf[0], size);
  if (len && len + strlen(kWinHostsPath) + 1 < size) {
    if (pathbuf[len] == '\\') pathbuf[len--] = '\0';
    memcpy(&pathbuf[len], kWinHostsPath, strlen(kWinHostsPath) + 1);
    return &pathbuf[0];
  } else {
    return NULL;
  }
}

/**
 * Returns parsed sorted singleton hardcoded hostname→ip4 map.
 *
 * @note yoinking realloc() ensures there's no size limits
 */
const struct HostsTxt *gethoststxt(void) {
  struct HostsTxtInitialStaticMemory *init = &g_hoststxt_init;
  if (!g_hoststxt) {
    g_hoststxt = &init->ht;
    init->ht.entries.n = pushpop(ARRAYLEN(init->entries));
    init->ht.entries.p = init->entries;
    init->ht.strings.n = pushpop(ARRAYLEN(init->strings));
    init->ht.strings.p = init->strings;
    __cxa_atexit(freehoststxt, &g_hoststxt, NULL);
    char pathbuf[PATH_MAX];
    const char *path = "/etc/hosts";
    if (IsWindows()) {
      path = firstnonnull(getnthoststxtpath(pathbuf, ARRAYLEN(pathbuf)), path);
    }
    FILE *f;
    if (!(f = fopen(path, "r")) || parsehoststxt(g_hoststxt, f) == -1) {
      if (!IsTiny()) fprintf(stderr, "%s: %s: %m\n", "warning", path);
    }
    fclose(f);
    sorthoststxt(g_hoststxt);
  }
  return g_hoststxt;
}
