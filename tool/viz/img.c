#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/log/log.h"
#include "libc/mem/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/x/x.h"
#include "libc/x/xasprintf.h"
#include "net/http/escape.h"
#include "net/http/http.h"
#include "third_party/getopt/getopt.internal.h"
#include "third_party/stb/stb_image.h"

#define USAGE \
  " [FLAGS] IMG...\n\
Utility for printing HTML <img> tags.\n\
\n\
FLAGS\n\
\n\
  -h      Help\n\
  -a      Wrap with <a> tag\n\
  -u      Embed data:base64 URI\n"

int scale;
bool linktag;
bool datauri;
bool sizeonly;

void PrintUsage(int rc, FILE *f) {
  fputs("Usage: ", f);
  fputs(program_invocation_name, f);
  fputs(USAGE, f);
  exit(rc);
}

void PrintImg(const char *path) {
  size_t n;
  int yn, xn, cn, w, h;
  void *img, *pix, *src;
  if (!(img = gc(xslurp(path, &n)))) exit(2);
  if (!(pix = gc(stbi_load_from_memory(img, n, &xn, &yn, &cn, 0)))) exit(3);
  if (linktag) {
    printf("<a href=\"%s\"\n  >", path);
  }
  src = (void *)path;
  if (datauri) {
    src = xasprintf("data:%s;base64,%s", FindContentType(path, -1),
                    gc(EncodeBase64(img, n, &n)));
  }
  w = (xn + (1 << scale) / 2) >> scale;
  h = (yn + (1 << scale) / 2) >> scale;
  if (sizeonly) {
    printf("width=\"%d\" height=\"%d\"", w, h);
  } else {
    printf("<img width=\"%d\" height=\"%d\" alt=\"[%s]\"\n     src=\"%s\">", w,
           h, path, src);
  }
  if (linktag) {
    printf("</a>");
  }
  printf("\n");
}

int main(int argc, char *argv[]) {
  if (!NoDebug()) ShowCrashReports();
  int i;
  while ((i = getopt(argc, argv, "?huas01234")) != -1) {
    switch (i) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
        scale = i - '0';
        break;
      case 's':
        sizeonly = true;
        break;
      case 'a':
        linktag = true;
        break;
      case 'u':
        datauri = true;
        break;
      case '?':
      case 'h':
        PrintUsage(0, stdout);
      default:
        PrintUsage(1, stderr);
    }
  }
  if (optind == argc) {
    PrintUsage(1, stderr);
  }
  for (i = optind; i < argc; ++i) {
    PrintImg(argv[i]);
  }
}
