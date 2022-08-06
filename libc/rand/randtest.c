/* clang-format off */
/*

	 Apply various randomness tests to a stream of bytes

		  by John Walker  --  September 1996
		       http://www.fourmilab.ch/

*/

#include "libc/math.h"

#define FALSE 0
#define TRUE  1

#define log2of10 3.32192809488736234787

static int binary = FALSE;	   /* Treat input as a bitstream */

static long ccount[256],	   /* Bins to count occurrences of values */
	    totalc = 0; 	   /* Total bytes counted */
static double prob[256];	   /* Probabilities per bin for entropy */

/*  RT_LOG2  --  Calculate log to the base 2  */

static double rt_log2(double x)
{
    return log2of10 * log10(x);
}

#define MONTEN	6		      /* Bytes used as Monte Carlo
					 co-ordinates.	This should be no more
					 bits than the mantissa of your
                                         "double" floating point type. */

static int mp, sccfirst;
static unsigned int monte[MONTEN];
static long inmont, mcount_;
static double cexp_, incirc, montex, montey, montepi,
	      scc, sccun, sccu0, scclast, scct1, scct2, scct3,
	      ent, chisq, datasum;

/*  RT_INIT  --  Initialise random test counters.  */

void rt_init(int binmode)
{
    int i;

    binary = binmode;	       /* Set binary / byte mode */

    /* Initialise for calculations */

    ent = 0.0;		       /* Clear entropy accumulator */
    chisq = 0.0;	       /* Clear Chi-Square */
    datasum = 0.0;	       /* Clear sum of bytes for arithmetic mean */

    mp = 0;		       /* Reset Monte Carlo accumulator pointer */
    mcount_ = 0; 	       /* Clear Monte Carlo tries */
    inmont = 0; 	       /* Clear Monte Carlo inside count */
    incirc = 65535.0 * 65535.0;/* In-circle distance for Monte Carlo */

    sccfirst = TRUE;	       /* Mark first time for serial correlation */
    scct1 = scct2 = scct3 = 0.0; /* Clear serial correlation terms */

    incirc = pow(pow(256.0, (double) (MONTEN / 2)) - 1, 2.0);

    for (i = 0; i < 256; i++) {
	ccount[i] = 0;
    }
    totalc = 0;
}

/*  RT_ADD  --	Add one or more bytes to accumulation.	*/

void rt_add(void *buf, int bufl)
{
    unsigned char *bp = buf;
    int oc, c, bean;

    while (bean = 0, (bufl-- > 0)) {
       oc = *bp++;

       do {
	  if (binary) {
	     c = !!(oc & 0x80);
	  } else {
	     c = oc;
	  }
	  ccount[c]++;		  /* Update counter for this bin */
	  totalc++;

	  /* Update inside / outside circle counts for Monte Carlo
	     computation of PI */

	  if (bean == 0) {
	      monte[mp++] = oc;       /* Save character for Monte Carlo */
	      if (mp >= MONTEN) {     /* Calculate every MONTEN character */
		 int mj;

		 mp = 0;
		 mcount_++;
		 montex = montey = 0;
		 for (mj = 0; mj < MONTEN / 2; mj++) {
		    montex = (montex * 256.0) + monte[mj];
		    montey = (montey * 256.0) + monte[(MONTEN / 2) + mj];
		 }
		 if ((montex * montex + montey *  montey) <= incirc) {
		    inmont++;
		 }
	      }
	  }

	  /* Update calculation of serial correlation coefficient */

	  sccun = c;
	  if (sccfirst) {
	     sccfirst = FALSE;
	     scclast = 0;
	     sccu0 = sccun;
	  } else {
	     scct1 = scct1 + scclast * sccun;
	  }
	  scct2 = scct2 + sccun;
	  scct3 = scct3 + (sccun * sccun);
	  scclast = sccun;
	  oc <<= 1;
       } while (binary && (++bean < 8));
    }
}

/*  RT_END  --	Complete calculation and return results.  */

void rt_end(double *r_ent, double *r_chisq, double *r_mean,
	    double *r_montepicalc, double *r_scc)
{
    int i;

    /* Complete calculation of serial correlation coefficient */

    scct1 = scct1 + scclast * sccu0;
    scct2 = scct2 * scct2;
    scc = totalc * scct3 - scct2;
    if (scc == 0.0) {
       scc = -100000;
    } else {
       scc = (totalc * scct1 - scct2) / scc;
    }

    /* Scan bins and calculate probability for each bin and
       Chi-Square distribution.  The probability will be reused
       in the entropy calculation below.  While we're at it,
       we sum of all the data which will be used to compute the
       mean. */

    cexp_ = totalc / (binary ? 2.0 : 256.0);  /* Expected count per bin */
    for (i = 0; i < (binary ? 2 : 256); i++) {
       double a = ccount[i] - cexp_;

       prob[i] = ((double) ccount[i]) / totalc;
       chisq += (a * a) / cexp_;
       datasum += ((double) i) * ccount[i];
    }

    /* Calculate entropy */

    for (i = 0; i < (binary ? 2 : 256); i++) {
       if (prob[i] > 0.0) {
	  ent += prob[i] * rt_log2(1 / prob[i]);
       }
    }

    /* Calculate Monte Carlo value for PI from percentage of hits
       within the circle */

    montepi = 4.0 * (((double) inmont) / mcount_);

    /* Return results through arguments */

    *r_ent = ent;
    *r_chisq = chisq;
    *r_mean = datasum / totalc;
    *r_montepicalc = montepi;
    *r_scc = scc;
}
