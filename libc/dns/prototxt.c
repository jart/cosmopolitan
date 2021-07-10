/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ This is free and unencumbered software released into the public domain.      │
│                                                                              │
│ Anyone is free to copy, modify, publish, use, compile, sell, or              │
│ distribute this software, either in source code form or as a compiled        │
│ binary, for any purpose, commercial or non-commercial, and by any            │
│ means.                                                                       │
│                                                                              │
│ In jurisdictions that recognize copyright laws, the author or authors        │
│ of this software dedicate any and all copyright interest in the              │
│ software to the public domain. We make this dedication for the benefit       │
│ of the public at large and to the detriment of our heirs and                 │
│ successors. We intend this dedication to be an overt act of                  │
│ relinquishment in perpetuity of all present and future rights to this        │
│ software under copyright law.                                                │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,              │
│ EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF           │
│ MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.       │
│ IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR            │
│ OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        │
│ ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR        │
│ OTHER DEALINGS IN THE SOFTWARE.                                              │
│                                                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/dns/prototxt.h"

#include "libc/bits/safemacros.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/fmt.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/nt/systeminfo.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"

static textwindows noinline char *GetNtProtocolsTxtPath(char *pathbuf,
                                                        uint32_t size) {
  /* protocol, not plural */
  const char *const kWinHostsPath = "\\drivers\\etc\\protocol";
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
 * Opens and searches /etc/protocols to find name for a given number.
 *
 * format of /etc/protocols is like this:
 *
 * # comment
 * # NAME           PROTOCOL        ALIASES
 * ip               0               IP
 * icmp             1               ICMP
 *
 * @param protonum is the protocol number
 * @param buf is a buffer to store the official name of the protocol
 * @param bufsize is the size of buf
 * @param filepath is the location of the protocols file
 *          (if NULL, uses /etc/protocols)
 * @return 0 on success, -1 on error
 *
 * @note aliases are not read from the file.
 */
int LookupProtoByNumber(const int protonum, char *buf, size_t bufsize,
                        const char *filepath) {
  FILE *f;
  char *line;
  char pathbuf[PATH_MAX];
  const char *path;
  size_t linesize;
  int found;
  char *name, *number, *comment, *tok;

  if (!(path = filepath)) {
    path = "/etc/protocols";
    if (IsWindows()) {
      path =
          firstnonnull(GetNtProtocolsTxtPath(pathbuf, ARRAYLEN(pathbuf)), path);
    }
  }

  if (bufsize == 0 || !(f = fopen(path, "r"))) {
    return -1;
  }
  line = NULL;
  linesize = 0;
  found = 0;

  while (found == 0 && (getline(&line, &linesize, f)) != -1) {
    if ((comment = strchr(line, '#'))) *comment = '\0';
    name = strtok_r(line, " \t\r\n\v", &tok);
    number = strtok_r(NULL, " \t\r\n\v", &tok);
    if (name && number && protonum == atoi(number)) {
      if (!memccpy(buf, name, '\0', bufsize)) {
        strcpy(buf, "");
        break;
      }
      found = 1;
    }
  }
  free(line);

  if (ferror(f)) {
    errno = ferror(f);
    return -1;
  }
  fclose(f);

  if (!found) return -1;

  return 0;
}

/**
 * Opens and searches /etc/protocols to find number for a given name.
 *
 * @param protoname is a NULL-terminated string
 * @param buf is a buffer to store the official name of the protocol
 * @param bufsize is the size of buf
 * @param filepath is the location of protocols file
 *          (if NULL, uses /etc/protocols)
 * @return -1 on error, or
 *          positive protocol number
 *
 * @note aliases are read from file for comparison, but not returned.
 * @see LookupProtoByNumber
 */
int LookupProtoByName(const char *protoname, char *buf, size_t bufsize,
                      const char *filepath) {
  FILE *f;
  char *line;
  char pathbuf[PATH_MAX];
  const char *path;
  size_t linesize;
  int found, result;
  char *name, *number, *alias, *comment, *tok;

  if (!(path = filepath)) {
    path = "/etc/protocols";
    if (IsWindows()) {
      path =
          firstnonnull(GetNtProtocolsTxtPath(pathbuf, ARRAYLEN(pathbuf)), path);
    }
  }

  if (bufsize == 0 || !(f = fopen(path, "r"))) {
    return -1;
  }
  line = NULL;
  linesize = 0;
  found = 0;
  result = -1;

  while (found == 0 && (getline(&line, &linesize, f)) != -1) {
    if ((comment = strchr(line, '#'))) *comment = '\0';
    name = strtok_r(line, " \t\r\n\v", &tok);
    number = strtok_r(NULL, "/ \t\r\n\v", &tok);
    if (name && number) {
      alias = name;
      while (alias && strcasecmp(alias, protoname) != 0)
        alias = strtok_r(NULL, " \t\r\n\v", &tok);

      if (alias) /* alias matched with protoname */
      {
        if (!memccpy(buf, name, '\0', bufsize)) {
          strcpy(buf, "");
          break;
        }
        result = atoi(number);
        found = 1;
      }
    }
  }
  free(line);

  if (ferror(f)) {
    errno = ferror(f);
    return -1;
  }
  fclose(f);

  if (!found) return -1;

  return result;
}
