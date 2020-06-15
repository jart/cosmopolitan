/* trmlen.f -- translated by f2c (version 20191129).
   You must link the resulting object file with libf2c:
        on Microsoft Windows system, link with libf2c.lib;
        on Linux or Unix systems, link with .../path/to/libf2c.a -lm
        or, if you install libf2c.a in a standard place, with -lf2c -lm
        -- in that order, at the end of the command line, as in
                cc *.o -lf2c -lm
        Source for libf2c is in /netlib/f2c/libf2c.zip, e.g.,

                http://www.netlib.org/f2c/libf2c.zip
*/

#include "third_party/f2c/f2c.h"

extern void _uninit_f2c(void *, int, long);
extern double _0;

/* Length of character string, excluding trailing blanks */
/* Same thing as LEN_TRIM() */
integer trmlen_(char *t, ftnlen t_len) {
  /* System generated locals */
  integer ret_val;

  /* Builtin functions */
  integer i_len(char *, ftnlen);

  /* 	Parameter: */
  for (ret_val = i_len(t, t_len); ret_val >= 1; --ret_val) {
    /* L1: */
    if (*(unsigned char *)&t[ret_val - 1] != ' ') {
      return ret_val;
    }
  }
  ret_val = 1;
  return ret_val;
} /* trmlen_ */
