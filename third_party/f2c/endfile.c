#include "libc/calls/calls.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/stdio.h"
#include "third_party/f2c/fio.h"

extern char *f__r_mode[], *f__w_mode[];

integer f_end(alist *a) {
  unit *b;
  FILE *tf;
  if (a->aunit >= MXUNIT || a->aunit < 0) err(a->aerr, 101, "endfile");
  b = &f__units[a->aunit];
  if (b->ufd == NULL) {
    char nbuf[10];
    sprintf(nbuf, "fort.%ld", (long)a->aunit);
    if (tf = fopen(nbuf, f__w_mode[0])) fclose(tf);
    return (0);
  }
  b->uend = 1;
  return (b->useek ? t_runc(a) : 0);
}

int t_runc(alist *a) {
  OFF_T loc, len;
  unit *b;
  int rc;
  FILE *bf;
  b = &f__units[a->aunit];
  if (b->url) return (0); /*don't truncate direct files*/
  loc = ftell(bf = b->ufd);
  fseek(bf, (OFF_T)0, SEEK_END);
  len = ftell(bf);
  if (loc >= len || b->useek == 0) return (0);
  if (b->urw & 2) fflush(b->ufd); /* necessary on some Linux systems */
  rc = ftruncate(fileno(b->ufd), loc);
  /* The following FSEEK is unnecessary on some systems, */
  /* but should be harmless. */
  fseek(b->ufd, (OFF_T)0, SEEK_END);
  if (rc) err(a->aerr, 111, "endfile");
  return 0;
}
