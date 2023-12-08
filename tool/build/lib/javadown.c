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
#include "tool/build/lib/javadown.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"

#define FILEOVERVIEW "@fileoverview"

struct Lines {
  size_t n;
  struct Line {
    char *p;
    size_t n;
  } * p;
};

static char *SkipEmptyFirstLine(char *p) {
  int i = 0;
  while (p[i] == ' ' || p[i] == '\t') ++i;
  return p[i] == '\n' ? p + i + 1 : p;
}

static void DeleteLastEmptyLine(char *p, size_t n) {
  while (n && (p[n - 1] == ' ' || p[n - 1] == '\t')) --n;
  if (n && p[n - 1] == '\n') p[n - 1] = '\0';
}

static void AppendLine(struct Lines *lines) {
  lines->p = realloc(lines->p, ++lines->n * sizeof(*lines->p));
  bzero(lines->p + lines->n - 1, sizeof(*lines->p));
}

static void AppendTag(struct JavadownTags *tags) {
  tags->p = realloc(tags->p, ++tags->n * sizeof(*tags->p));
  bzero(tags->p + tags->n - 1, sizeof(*tags->p));
}

static unsigned GetSpacePrefixLen(const char *p, size_t n) {
  int i;
  for (i = 0; i < n; ++i) {
    if (p[i] != ' ' && p[i] != '\t') {
      break;
    }
  }
  return i;
}

static unsigned GetSpaceStarPrefixLen(const char *p, size_t n) {
  int i;
  i = GetSpacePrefixLen(p, n);
  if (i < n && (p[i] == '*' || p[i] == '/')) {
    return p[i + 1] == '/' ? i + 2 : i + 1;
  } else {
    return 0;
  }
}

static unsigned GetTagLen(const char *p, size_t n) {
  int i;
  for (i = 0; i < n; ++i) {
    if (!islower(p[i])) {
      break;
    }
  }
  return i;
}

static unsigned GetMinPrefixLen(struct Lines *lines,
                                unsigned GetPrefixLen(const char *, size_t)) {
  int i;
  unsigned n, m;
  for (m = -1, i = 0; i < lines->n; ++i) {
    if (lines->p[i].n) {
      n = GetPrefixLen(lines->p[i].p, lines->p[i].n);
      if (n < m) m = n;
    }
  }
  return m == -1 ? 0 : m;
}

static void RemovePrefixes(struct Lines *lines, unsigned m) {
  int i;
  for (i = 0; i < lines->n; ++i) {
    if (m <= lines->p[i].n) {
      lines->p[i].p += m;
      lines->p[i].n -= m;
    }
  }
}

static void SplitLines(struct Lines *lines, char *p) {
  char *q;
  DeleteLastEmptyLine(p, strlen(p));
  p = SkipEmptyFirstLine(p);
  for (;;) {
    AppendLine(lines);
    lines->p[lines->n - 1].p = p;
    lines->p[lines->n - 1].n = (q = strchr(p, '\n')) ? q - p : strlen(p);
    if (!q) break;
    p = q + 1;
  }
  RemovePrefixes(lines, GetMinPrefixLen(lines, GetSpaceStarPrefixLen));
  RemovePrefixes(lines, GetMinPrefixLen(lines, GetSpacePrefixLen));
}

static bool ConsumeFileOverview(struct Lines *lines) {
  if (lines->n && lines->p[0].n >= strlen(FILEOVERVIEW) &&
      startswith(lines->p[0].p, FILEOVERVIEW)) {
    lines->p[0].p += strlen(FILEOVERVIEW);
    lines->p[0].n -= strlen(FILEOVERVIEW);
    while (lines->p[0].n &&
           (lines->p[0].p[0] == ' ' || lines->p[0].p[0] == '\t')) {
      lines->p[0].p += 1;
      lines->p[0].n -= 1;
    }
    return true;
  } else {
    return false;
  }
}

static void RemoveTrailingWhitespace(struct Lines *lines) {
  int i;
  for (i = 0; i < lines->n; ++i) {
    while (lines->p[i].n && isspace(lines->p[i].p[lines->p[i].n - 1])) {
      --lines->p[i].n;
    }
  }
}

static int ExtractTitle(struct Javadown *jd, struct Lines *lines) {
  int i;
  char *p;
  size_t n;
  for (p = NULL, n = i = 0; i < lines->n; ++i) {
    if (!lines->p[i].n) {
      ++i;
      break;
    }
    if (*lines->p[i].p == '@') {
      break;
    }
    if (i) {
      p = realloc(p, ++n);
      p[n - 1] = ' ';
    }
    p = realloc(p, n + lines->p[i].n);
    memcpy(p + n, lines->p[i].p, lines->p[i].n);
    n += lines->p[i].n;
  }
  p = realloc(p, n + 1);
  p[n] = '\0';
  jd->title = p;
  return i;
}

static int ExtractText(struct Javadown *jd, struct Lines *lines, int i) {
  int j;
  char *p;
  size_t n;
  for (p = NULL, n = j = 0; i + j < lines->n; ++j) {
    if (lines->p[i + j].n && lines->p[i + j].p[0] == '@') break;
    if (j) {
      p = realloc(p, ++n);
      p[n - 1] = '\n';
    }
    p = realloc(p, n + lines->p[i + j].n);
    memcpy(p + n, lines->p[i + j].p, lines->p[i + j].n);
    n += lines->p[i + j].n;
  }
  p = realloc(p, n + 1);
  p[n] = '\0';
  jd->text = p;
  return i;
}

static void ExtractTags(struct Javadown *jd, struct Lines *lines, int i) {
  char *tag, *text, *p2;
  unsigned taglen, textlen, n2;
  for (; i < lines->n; ++i) {
    if (!lines->p[i].n) continue;
    if (lines->p[i].p[0] != '@') continue;
    tag = lines->p[i].p + 1;
    taglen = GetTagLen(tag, lines->p[i].n - 1);
    if (!taglen) continue;
    text = tag + taglen;
    tag = strndup(tag, taglen);
    textlen = lines->p[i].n - 1 - taglen;
    while (textlen && isspace(*text)) {
      ++text;
      --textlen;
    }
    text = strndup(text, textlen);
    while (i + 1 < lines->n &&
           (!lines->p[i + 1].n || lines->p[i + 1].p[0] != '@')) {
      ++i;
      p2 = lines->p[i].p;
      n2 = lines->p[i].n;
      if (n2 && *p2 == '\t') {
        p2 += 1;
        n2 -= 1;
      }
      if (n2 >= 4 && !memcmp(p2, "    ", 4)) {
        p2 += 4;
        n2 -= 4;
      }
      text = realloc(text, textlen + 1 + n2 + 1);
      text[textlen] = '\n';
      memcpy(text + textlen + 1, p2, n2);
      textlen += 1 + n2;
      text[textlen] = '\0';
    }
    AppendTag(&jd->tags);
    jd->tags.p[jd->tags.n - 1].tag = tag;
    jd->tags.p[jd->tags.n - 1].text = text;
  }
}

/**
 * Parses javadown.
 *
 * @param data should point to text inside the slash star markers
 * @param size is length of data in bytes
 * @return object that should be passed to FreeJavadown()
 */
struct Javadown *ParseJavadown(const char *data, size_t size) {
  int i;
  char *p;
  struct Lines lines;
  struct Javadown *jd;
  bzero(&lines, sizeof(lines));
  jd = calloc(1, sizeof(struct Javadown));
  p = strndup(data, size);
  SplitLines(&lines, p);
  RemoveTrailingWhitespace(&lines);
  jd->isfileoverview = ConsumeFileOverview(&lines);
  i = ExtractTitle(jd, &lines);
  i = ExtractText(jd, &lines, i);
  ExtractTags(jd, &lines, i);
  free(lines.p);
  free(p);
  return jd;
}

/**
 * Frees object returned by ParseJavadown().
 */
void FreeJavadown(struct Javadown *jd) {
  int i;
  if (jd) {
    for (i = 0; i < jd->tags.n; ++i) {
      free(jd->tags.p[i].tag);
      free(jd->tags.p[i].text);
    }
    free(jd->tags.p);
    free(jd->title);
    free(jd->text);
    free(jd);
  }
}
