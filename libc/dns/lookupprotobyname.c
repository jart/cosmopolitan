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
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/intrin/safemacros.internal.h"
#include "libc/dce.h"
#include "libc/dns/prototxt.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"

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
  const char *path;
  size_t linesize;
  int found, result;
  char pathbuf[PATH_MAX];
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
