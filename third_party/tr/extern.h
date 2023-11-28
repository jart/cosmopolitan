#ifndef COSMOPOLITAN_THIRD_PARTY_TR_EXTERN_H_
#define COSMOPOLITAN_THIRD_PARTY_TR_EXTERN_H_
#include "libc/limits.h"
COSMOPOLITAN_C_START_

typedef struct {
  enum { STRING1, STRING2 } which;
  enum { EOS, INFINITE, NORMAL, RANGE, SEQUENCE, SET } state;
  int cnt;            /* character count */
  int lastch;         /* last character */
  int equiv[2];       /* equivalence set */
  int *set;           /* set of characters */
  unsigned char *str; /* user's string */
} STR;

#define NCHARS (UCHAR_MAX + 1) /* Number of possible characters. */
#define OOBCH  (UCHAR_MAX + 1) /* Out of band character value. */

int next(STR *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_TR_EXTERN_H_ */
