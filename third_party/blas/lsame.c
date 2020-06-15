/* lsame.f -- translated by f2c (version 20191129).
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

/* > \brief \b LSAME */

/*  =========== DOCUMENTATION =========== */

/* Online html documentation available at */
/*            http://www.netlib.org/lapack/explore-html/ */

/*  Definition: */
/*  =========== */

/*       LOGICAL FUNCTION LSAME(CA,CB) */

/*       .. Scalar Arguments .. */
/*       CHARACTER CA,CB */
/*       .. */

/* > \par Purpose: */
/*  ============= */
/* > */
/* > \verbatim */
/* > */
/* > LSAME returns .TRUE. if CA is the same letter as CB regardless of */
/* > case. */
/* > \endverbatim */

/*  Arguments: */
/*  ========== */

/* > \param[in] CA */
/* > \verbatim */
/* >          CA is CHARACTER*1 */
/* > \endverbatim */
/* > */
/* > \param[in] CB */
/* > \verbatim */
/* >          CB is CHARACTER*1 */
/* >          CA and CB specify the single characters to be compared. */
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
logical lsame_(char *ca, char *cb) {
  /* System generated locals */
  logical ret_val;

  /* Local variables */
  static integer inta, intb, zcode;

  /*  -- Reference BLAS level1 routine (version 3.1) -- */
  /*  -- Reference BLAS is a software package provided by Univ. of Tennessee,
   * -- */
  /*  -- Univ. of California Berkeley, Univ. of Colorado Denver and NAG Ltd..--
   */
  /*     December 2016 */

  /*     .. Scalar Arguments .. */
  /*     .. */

  /* ===================================================================== */

  /*     .. Intrinsic Functions .. */
  /*     .. */
  /*     .. Local Scalars .. */
  /*     .. */

  /*     Test if the characters are equal */

  ret_val = *(unsigned char *)ca == *(unsigned char *)cb;
  if (ret_val) {
    return ret_val;
  }

  /*     Now test for equivalence if both characters are alphabetic. */

  zcode = 'Z';

  /*     Use 'Z' rather than 'A' so that ASCII can be detected on Prime */
  /*     machines, on which ICHAR returns a value with bit 8 set. */
  /*     ICHAR('A') on Prime machines returns 193 which is the same as */
  /*     ICHAR('A') on an EBCDIC machine. */

  inta = *(unsigned char *)ca;
  intb = *(unsigned char *)cb;

  if (zcode == 90 || zcode == 122) {

    /*        ASCII is assumed - ZCODE is the ASCII code of either lower or */
    /*        upper case 'Z'. */

    if (inta >= 97 && inta <= 122) {
      inta += -32;
    }
    if (intb >= 97 && intb <= 122) {
      intb += -32;
    }

  } else if (zcode == 233 || zcode == 169) {

    /*        EBCDIC is assumed - ZCODE is the EBCDIC code of either lower or */
    /*        upper case 'Z'. */

    if (inta >= 129 && inta <= 137 || inta >= 145 && inta <= 153 ||
        inta >= 162 && inta <= 169) {
      inta += 64;
    }
    if (intb >= 129 && intb <= 137 || intb >= 145 && intb <= 153 ||
        intb >= 162 && intb <= 169) {
      intb += 64;
    }

  } else if (zcode == 218 || zcode == 250) {

    /*        ASCII is assumed, on Prime machines - ZCODE is the ASCII code */
    /*        plus 128 of either lower or upper case 'Z'. */

    if (inta >= 225 && inta <= 250) {
      inta += -32;
    }
    if (intb >= 225 && intb <= 250) {
      intb += -32;
    }
  }
  ret_val = inta == intb;

  /*     RETURN */

  /*     End of LSAME */

  return ret_val;
} /* lsame_ */
