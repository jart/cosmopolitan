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
#include "tool/build/lib/asmdown.h"
#include "libc/mem/alg.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "tool/build/lib/javadown.h"

static bool IsSymbolChar1(char c) {
  return (c & 0x80) || ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z') ||
         c == '$' || c == '_';
}

static bool IsSymbolChar2(char c) {
  return (c & 0x80) || ('0' <= c && c <= '9') || ('A' <= c && c <= 'Z') ||
         ('a' <= c && c <= 'z') || c == '$' || c == '_';
}

static bool IsSymbolString(const char *s) {
  if (!IsSymbolChar1(*s++)) return false;
  while (*s) {
    if (!IsSymbolChar2(*s++)) return false;
  }
  return true;
}

/**
 * Extracts symbols and docstrings from .S file.
 *
 * Docstrings are multiline Fortran-style AT&T assembly comments
 * preceding a symbol that have at least one @tag line.
 *
 * @param code is contents of .S file
 * @param size is byte length of code
 * @return object that needs to be FreeAsmdown()'d
 * @note this function assumes canonical unix newlines
 */
struct Asmdown *ParseAsmdown(const char *code, size_t size) {
  struct Asmdown *ad;
  const char *p1;
  char *p2, *p3, *symbol, *alias;
  enum { BOL, COM, SYM, OTHER } state;
  int i, j, line, start_line, start_docstring, end_docstring, start_symbol;
  ad = calloc(1, sizeof(struct Asmdown));
  line = 1;
  start_line = 1;
  state = BOL;
  start_docstring = 0;
  end_docstring = 0;
  start_symbol = 0;
  for (i = 0; i < size; ++i) {
    switch (state) {
      case BOL:
        if (code[i] == '/') {
          start_line = line;
          start_docstring = i;
          state = COM;
        } else if (IsSymbolChar1(code[i])) {
          start_symbol = i;
          state = SYM;
        } else if (code[i] == '\n') {
          ++line;
        } else if (i + 10 < size && !memcmp(code + i, "\t.ftrace1\t", 10)) {
          ++line;
        } else if (i + 8 < size && !memcmp(code + i, "\t.alias\t", 8)) {
          p1 = code + i + 8;
          if ((p2 = strchr(p1, ',')) && (p3 = strchr(p2, '\n'))) {
            symbol = strndup(p1, p2 - p1);
            alias = strndup(p2 + 1, p3 - (p2 + 1));
            if (IsSymbolString(symbol) && IsSymbolString(alias)) {
              for (j = 0; j < ad->symbols.n; ++j) {
                if (!strcmp(ad->symbols.p[j].name, symbol)) {
                  ad->symbols.p = realloc(
                      ad->symbols.p, ++ad->symbols.n * sizeof(*ad->symbols.p));
                  ad->symbols.p[ad->symbols.n - 1].line = ad->symbols.p[j].line;
                  ad->symbols.p[ad->symbols.n - 1].name = strdup(alias);
                  ad->symbols.p[ad->symbols.n - 1].is_alias = true;
                  ad->symbols.p[ad->symbols.n - 1].javadown =
                      ad->symbols.p[j].javadown;
                  break;
                }
              }
            }
            free(symbol);
            free(alias);
          }
          state = OTHER;
        } else {
          state = OTHER;
        }
        break;
      case COM:
        if (code[i] == '\n') {
          ++line;
          if (i + 1 < size && code[i + 1] != '/') {
            state = BOL;
            end_docstring = i + 1;
            if (!memmem(code + start_docstring, end_docstring - start_docstring,
                        "/\t@", 3)) {
              start_docstring = 0;
              end_docstring = 0;
            }
          }
        }
        break;
      case SYM:
        if (code[i] == ':' && end_docstring > start_docstring) {
          ad->symbols.p =
              realloc(ad->symbols.p, ++ad->symbols.n * sizeof(*ad->symbols.p));
          ad->symbols.p[ad->symbols.n - 1].line = start_line;
          ad->symbols.p[ad->symbols.n - 1].name =
              strndup(code + start_symbol, i - start_symbol);
          ad->symbols.p[ad->symbols.n - 1].is_alias = false;
          ad->symbols.p[ad->symbols.n - 1].javadown = ParseJavadown(
              code + start_docstring, end_docstring - start_docstring);
          end_docstring = 0;
          start_docstring = 0;
          state = OTHER;
        } else if (code[i] == '\n') {
          ++line;
          state = BOL;
        } else if (!IsSymbolChar2(code[i])) {
          state = OTHER;
        }
        break;
      case OTHER:
        if (code[i] == '\n') {
          ++line;
          state = BOL;
        }
        break;
      default:
        __builtin_unreachable();
    }
  }
  return ad;
}

/**
 * Frees object returned by ParseAsmdown().
 */
void FreeAsmdown(struct Asmdown *ad) {
  int i;
  if (ad) {
    for (i = 0; i < ad->symbols.n; ++i) {
      free(ad->symbols.p[i].name);
      if (!ad->symbols.p[i].is_alias) {
        FreeJavadown(ad->symbols.p[i].javadown);
      }
    }
    free(ad->symbols.p);
    free(ad);
  }
}
