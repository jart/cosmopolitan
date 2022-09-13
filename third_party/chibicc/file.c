#include "libc/intrin/bsf.h"
#include "third_party/chibicc/chibicc.h"

// Slurps contents of file.
char *read_file(const char *path) {
  char *p;
  FILE *fp;
  int buflen, nread, end, n;
  if (!strcmp(path, "-")) {
    fp = stdin;
  } else {
    fp = fopen(path, "r");
    if (!fp) return NULL;
  }
  buflen = 4096;
  nread = 0;
  p = calloc(1, buflen);
  for (;;) {
    end = buflen - 2;
    n = fread(p + nread, 1, end - nread, fp);
    if (n == 0) break;
    nread += n;
    if (nread == end) {
      buflen *= 2;
      p = realloc(p, buflen);
    }
  }
  if (fp != stdin) fclose(fp);
  if (nread > 0 && p[nread - 1] == '\\') {
    p[nread - 1] = '\n';
  } else if (nread == 0 || p[nread - 1] != '\n') {
    p[nread++] = '\n';
  }
  p[nread] = '\0';
  return p;
}

char *skip_bom(char *p) {
  // UTF-8 texts may start with a 3-byte "BOM" marker sequence.
  // If exists, just skip them because they are useless bytes.
  // (It is actually not recommended to add BOM markers to UTF-8
  // texts, but it's not uncommon particularly on Windows.)
  if (!memcmp(p, "\357\273\277", 3)) p += 3;
  return p;
}

// Replaces \r or \r\n with \n.
void canonicalize_newline(char *p) {
  char *q = p;
  for (;;) {
#if defined(__GNUC__) && defined(__x86_64__) && !defined(__chibicc__)  // :'(
    typedef char xmm_u __attribute__((__vector_size__(16), __aligned__(1)));
    typedef char xmm_t __attribute__((__vector_size__(16), __aligned__(16)));
    if (!((uintptr_t)p & 15)) {
      xmm_t v;
      unsigned m;
      xmm_t z = {0};
      xmm_t s = {'\r', '\r', '\r', '\r', '\r', '\r', '\r', '\r',
                 '\r', '\r', '\r', '\r', '\r', '\r', '\r', '\r'};
      xmm_t t = {'\n', '\n', '\n', '\n', '\n', '\n', '\n', '\n',
                 '\n', '\n', '\n', '\n', '\n', '\n', '\n', '\n'};
      for (;;) {
        v = *(const xmm_t *)p;
        m = __builtin_ia32_pmovmskb128((v == z) | (v == s) | (v == t));
        if (!m) {
          *(xmm_u *)q = v;
          p += 16;
          q += 16;
        } else {
          m = _bsf(m);
          memmove(q, p, m);
          p += m;
          q += m;
          break;
        }
      }
    }
#endif
    if (p[0]) {
      if (p[0] == '\r' && p[1] == '\n') {
        p += 2;
        *q++ = '\n';
      } else if (p[0] == '\r') {
        p += 1;
        *q++ = '\n';
      } else {
        *q++ = *p++;
      }
    } else {
      break;
    }
  }
  *q = '\0';
}

// Removes backslashes followed by a newline.
void remove_backslash_newline(char *p) {
  int i = 0, j = 0;
  // We want to keep the number of newline characters so that
  // the logical line number matches the physical one.
  // This counter maintain the number of newlines we have removed.
  int n = 0;
  bool instring = false;
  for (;;) {
#if defined(__GNUC__) && defined(__x86_64__) && !defined(__chibicc__)  // :'(
    typedef char xmm_u __attribute__((__vector_size__(16), __aligned__(1)));
    typedef char xmm_t __attribute__((__vector_size__(16), __aligned__(16)));
    if (!((uintptr_t)(p + i) & 15)) {
      xmm_t v;
      unsigned m;
      xmm_t A = {0};
      xmm_t B = {'/', '/', '/', '/', '/', '/', '/', '/',
                 '/', '/', '/', '/', '/', '/', '/', '/'};
      xmm_t C = {'"', '"', '"', '"', '"', '"', '"', '"',
                 '"', '"', '"', '"', '"', '"', '"', '"'};
      xmm_t D = {'\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\',
                 '\\', '\\', '\\', '\\', '\\', '\\', '\\', '\\'};
      xmm_t E = {'\n', '\n', '\n', '\n', '\n', '\n', '\n', '\n',
                 '\n', '\n', '\n', '\n', '\n', '\n', '\n', '\n'};
      for (;;) {
        v = *(const xmm_t *)(p + i);
        m = __builtin_ia32_pmovmskb128((v == A) | (v == B) | (v == C) |
                                       (v == D) | (v == E));
        if (!m) {
          *(xmm_u *)(p + j) = v;
          i += 16;
          j += 16;
        } else {
          m = _bsf(m);
          memmove(p + j, p + i, m);
          i += m;
          j += m;
          break;
        }
      }
    }
#endif
    if (p[i]) {
      if (instring) {
        if (p[i] == '"' && p[i - 1] != '\\') {
          instring = false;
        }
      } else {
        if (p[i] == '"') {
          instring = true;
        } else if (p[i] == '/' && p[i + 1] == '*') {
          p[j++] = p[i++];
          p[j++] = p[i++];
          while (p[i]) {
            if (p[i] == '*' && p[i + 1] == '/') {
              p[j++] = p[i++];
              p[j++] = p[i++];
              break;
            } else {
              p[j++] = p[i++];
            }
          }
          continue;
        }
      }
      if (p[i] == '\\' && p[i + 1] == '\n') {
        i += 2;
        n++;
      } else if (p[i] == '\n') {
        p[j++] = p[i++];
        for (; n > 0; n--) p[j++] = '\n';
      } else {
        p[j++] = p[i++];
      }
    } else {
      break;
    }
  }
  for (; n > 0; n--) p[j++] = '\n';
  p[j] = '\0';
}
