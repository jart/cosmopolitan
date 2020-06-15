/* xerbla.f -- translated by f2c (version 20191129).
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

/* Table of constant values */

static integer c__1 = 1;

/* > \brief \b XERBLA */

/*  =========== DOCUMENTATION =========== */

/* Online html documentation available at */
/*            http://www.netlib.org/lapack/explore-html/ */

/*  Definition: */
/*  =========== */

/*       SUBROUTINE XERBLA( SRNAME, INFO ) */

/*       .. Scalar Arguments .. */
/*       CHARACTER*(*)      SRNAME */
/*       INTEGER            INFO */
/*       .. */

/* > \par Purpose: */
/*  ============= */
/* > */
/* > \verbatim */
/* > */
/* > XERBLA  is an error handler for the LAPACK routines. */
/* > It is called by an LAPACK routine if an input parameter has an */
/* > invalid value.  A message is printed and execution stops. */
/* > */
/* > Installers may consider modifying the STOP statement in order to */
/* > call system-specific exception-handling facilities. */
/* > \endverbatim */

/*  Arguments: */
/*  ========== */

/* > \param[in] SRNAME */
/* > \verbatim */
/* >          SRNAME is CHARACTER*(*) */
/* >          The name of the routine which called XERBLA. */
/* > \endverbatim */
/* > */
/* > \param[in] INFO */
/* > \verbatim */
/* >          INFO is INTEGER */
/* >          The position of the invalid parameter in the parameter list */
/* >          of the calling routine. */
/* > \endverbatim */

/*  Authors: */
/*  ======== */

/* > \author Univ. of Tennessee */
/* > \author Univ. of California Berkeley */
/* > \author Univ. of Colorado Denver */
/* > \author NAG Ltd. */

/* > \date December 2016 */

/* > \ingroup aux_blas */

/*  ===================================================================== */
/* Subroutine */ int xerbla_(char *srname, integer *info, ftnlen srname_len) {
  /* Format strings */
  static char fmt_9999[] =
      "(\002 ** On entry to \002,a,\002 parameter num"
      "ber \002,i2,\002 had \002,\002an illegal value\002)";

  /* Builtin functions */
  integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);
  /* Subroutine */ int s_stop(char *, ftnlen);

  /* Local variables */
  extern doublereal trmlen_(char *, ftnlen);

  /* Fortran I/O blocks */
  static cilist io___1 = {0, 6, 0, fmt_9999, 0};

  /*  -- Reference BLAS level1 routine (version 3.7.0) -- */
  /*  -- Reference BLAS is a software package provided by Univ. of Tennessee,
   * -- */
  /*  -- Univ. of California Berkeley, Univ. of Colorado Denver and NAG Ltd..--
   */
  /*     December 2016 */

  /*     .. Scalar Arguments .. */
  /*     .. */

  /* ===================================================================== */

  /*     .. Intrinsic Functions .. */
  /*     INTRINSIC          LEN_TRIM */
  /*     .. */
  /*     .. Executable Statements .. */

  s_wsfe(&io___1);
  do_fio(&c__1, srname, (integer)trmlen_(srname, srname_len));
  do_fio(&c__1, (char *)&(*info), (ftnlen)sizeof(integer));
  e_wsfe();

  s_stop("", (ftnlen)0);

  /*     End of XERBLA */

  return 0;
} /* xerbla_ */
