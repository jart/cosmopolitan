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
  int i = 0, j = 0;
  while (p[i]) {
    if (p[i] == '\r' && p[i + 1] == '\n') {
      i += 2;
      p[j++] = '\n';
    } else if (p[i] == '\r') {
      i++;
      p[j++] = '\n';
    } else {
      p[j++] = p[i++];
    }
  }
  p[j] = '\0';
}

// Removes backslashes followed by a newline.
void remove_backslash_newline(char *p) {
  int i = 0, j = 0;
  // We want to keep the number of newline characters so that
  // the logical line number matches the physical one.
  // This counter maintain the number of newlines we have removed.
  int n = 0;
  bool instring = false;
  while (p[i]) {
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
  }
  for (; n > 0; n--) p[j++] = '\n';
  p[j] = '\0';
}
