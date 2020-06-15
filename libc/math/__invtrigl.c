#include "libc/math/__invtrigl.h"

static const long double pS0 = 1.66666666666666666631e-01L,
                         pS1 = -4.16313987993683104320e-01L,
                         pS2 = 3.69068046323246813704e-01L,
                         pS3 = -1.36213932016738603108e-01L,
                         pS4 = 1.78324189708471965733e-02L,
                         pS5 = -2.19216428382605211588e-04L,
                         pS6 = -7.10526623669075243183e-06L,
                         qS1 = -2.94788392796209867269e+00L,
                         qS2 = 3.27309890266528636716e+00L,
                         qS3 = -1.68285799854822427013e+00L,
                         qS4 = 3.90699412641738801874e-01L,
                         qS5 = -3.14365703596053263322e-02L;

const long double pio2_hi = 1.57079632679489661926L;
const long double pio2_lo = -2.50827880633416601173e-20L;

/* used in asinl() and acosl() */
/* R(x^2) is a rational approximation of (asin(x)-x)/x^3 with Remez algorithm */
long double __invtrigl_R(long double z) {
  long double p, q;
  p = z * (pS0 +
           z * (pS1 + z * (pS2 + z * (pS3 + z * (pS4 + z * (pS5 + z * pS6))))));
  q = 1.0 + z * (qS1 + z * (qS2 + z * (qS3 + z * (qS4 + z * qS5))));
  return p / q;
}
