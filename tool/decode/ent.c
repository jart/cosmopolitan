/* PUBLIC DOMAIN */
/* clang-format off */
/*
	ENT  --  Entropy calculation and analysis of putative
		 random sequences.

        Designed and implemented by John "Random" Walker in May 1985.

	Multiple analyses of random sequences added in December 1985.

	Bit stream analysis added in September 1997.

	Terse mode output, getopt() command line processing,
	optional stdin input, and HTML documentation added in
	October 1998.

	Documentation for the -t (terse output) option added
	in July 2006.

	Replaced table look-up for chi square to probability
	conversion with algorithmic computation in January 2008.

	For additional information and the latest version,
	see http://www.fourmilab.ch/random/

*/

#include "libc/calls/calls.h"
#include "libc/math.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "third_party/getopt/getopt.internal.h"

#define UPDATE  "January 28th, 2008"

#define FALSE 0
#define TRUE  1

#ifdef M_PI
#define PI	 M_PI
#else
#define PI	 3.14159265358979323846
#endif

/* ISO 8859/1 Latin-1 alphabetic and upper and lower case bit vector tables. */
#define isISOspace(x)                                                  \
  ((isascii(((unsigned char)(x))) && isspace(((unsigned char)(x)))) || \
   ((x) == 0xA0))
#define isISOalpha(x)                      \
  ((isoalpha[(((unsigned char)(x))) / 8] & \
    (0x80 >> ((((unsigned char)(x))) % 8))) != 0)
#define isISOupper(x)                      \
  ((isoupper[(((unsigned char)(x))) / 8] & \
    (0x80 >> ((((unsigned char)(x))) % 8))) != 0)
#define isISOlower(x)                      \
  ((isolower[(((unsigned char)(x))) / 8] & \
    (0x80 >> ((((unsigned char)(x))) % 8))) != 0)
#define isISOprint(x) ((((x) >= ' ') && ((x) <= '~')) || ((x) >= 0xA0))
#define toISOupper(x)                                                          \
  (isISOlower(x)                                                               \
       ? (isascii(((unsigned char)(x))) ? toupper(x)                           \
                                        : (((((unsigned char)(x)) != 0xDF) &&  \
                                            (((unsigned char)(x)) != 0xFF))    \
                                               ? (((unsigned char)(x)) - 0x20) \
                                               : (x)))                         \
       : (x))
#define toISOlower(x)                                                    \
  (isISOupper(x)                                                         \
       ? (isascii(((unsigned char)(x))) ? tolower(x)                     \
                                        : (((unsigned char)(x)) + 0x20)) \
       : (x))

unsigned char isoalpha[32] = {
    0, 0, 0, 0, 0, 0, 0, 0, 127, 255, 255, 224, 127, 255, 255, 224,
    0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 254, 255, 255, 255, 254, 255};
unsigned char isoupper[32] = {0,   0, 0,   0,   0,   0,   0, 0, 127, 255, 255,
                              224, 0, 0,   0,   0,   0,   0, 0, 0,   0,   0,
                              0,   0, 255, 255, 254, 254, 0, 0, 0,   0};
unsigned char isolower[32] = {0, 0,   0,   0,   0,   0, 0,   0,   0,   0,  0,
                              0, 127, 255, 255, 224, 0, 0,   0,   0,   0,  0,
                              0, 0,   0,   0,   0,   1, 255, 255, 254, 255};

/*  HELP  --  Print information on how to call	*/

static void help(void)
{
        printf("ent --  Calculate entropy of file.  Call");
        printf("\n        with ent [options] [input-file]");
        printf("\n");
        printf("\n        Options:   -b   Treat input as a stream of bits");
        printf("\n                   -c   Print occurrence counts");
        printf("\n                   -f   Fold upper to lower case letters");
        printf("\n                   -t   Terse output in CSV format");
        printf("\n                   -u   Print this message\n");
        printf("\nBy John Walker");
        printf("\n   http://www.fourmilab.ch/");
        printf("\n   %s\n", UPDATE);
}

/*  Main program  */

int main(int argc, char *argv[])
{
	int i, oc, opt;
	long ccount[256];	      /* Bins to count occurrences of values */
	long totalc = 0;	      /* Total character count */
	char *samp;
	double montepi, chip,
	       scc, ent, mean, chisq;
	FILE *fp = stdin;
	int counts = FALSE,	      /* Print character counts */
	    fold = FALSE,	      /* Fold upper to lower */
	    binary = FALSE,	      /* Treat input as a bitstream */
	    terse = FALSE;	      /* Terse (CSV format) output */

        while ((opt = getopt(argc, argv, "bcftu?BCFTU")) != -1) {
	    switch (toISOlower(opt)) {
                 case 'b':
		    binary = TRUE;
		    break;

                 case 'c':
		    counts = TRUE;
		    break;

                 case 'f':
		    fold = TRUE;
		    break;

                 case 't':
		    terse = TRUE;
		    break;

                 case '?':
                 case 'u':
		    help();
		    return 0;
	    }
	}
	if (optind < argc) {
	   if (optind != (argc - 1)) {
              printf("Duplicate file name.\n");
	      help();
	      return 2;
	   }
           if ((fp = fopen(argv[optind], "rb")) == NULL) {
              printf("Cannot open file %s\n", argv[optind]);
	      return 2;
	   }
	}

        samp = binary ? "bit" : "byte";
	bzero(ccount, sizeof ccount);

	/* Initialise for calculations */

	rt_init(binary);

	/* Scan input file and count character occurrences */

	while ((oc = fgetc(fp)) != EOF) {
	   unsigned char ocb;

	   if (fold && isISOalpha(oc) && isISOupper(oc)) {
	      oc = toISOlower(oc);
	   }
	   ocb = (unsigned char) oc;
	   totalc += binary ? 8 : 1;
	   if (binary) {
	    int b;
	    unsigned char ob = ocb;

	    for (b = 0; b < 8; b++) {
		ccount[ob & 1]++;
		ob >>= 1;
	    }
	   } else {
	       ccount[ocb]++;	      /* Update counter for this bin */
	   }
	   rt_add(&ocb, 1);
	}
	fclose(fp);

	/* Complete calculation and return sequence metrics */

	rt_end(&ent, &chisq, &mean, &montepi, &scc);

	if (terse) {
           printf("0,File-%ss,Entropy,Chi-square,Mean,Monte-Carlo-Pi,Serial-Correlation\n",
              binary ? "bit" : "byte");
           printf("1,%ld,%f,%f,%f,%f,%f\n",
	      totalc, ent, chisq, mean, montepi, scc);
	}

	/* Calculate probability of observed distribution occurring from
	   the results of the Chi-Square test */

    	chip = pochisq(chisq, (binary ? 1 : 255));

	/* Print bin counts if requested */

	if (counts) {
	   if (terse) {
              printf("2,Value,Occurrences,Fraction\n");
	   } else {
              printf("Value Char Occurrences Fraction\n");
	   }
	   for (i = 0; i < (binary ? 2 : 256); i++) {
	      if (terse) {
                 printf("3,%d,%ld,%f\n", i,
		    ccount[i], ((double) ccount[i] / totalc));
	      } else {
		 if (ccount[i] > 0) {
                    printf("%3d   %c   %10ld   %f\n", i,
		       /* The following expression shows ISO 8859-1
			  Latin1 characters and blanks out other codes.
			  The test for ISO space replaces the ISO
			  non-blanking space (0xA0) with a regular
                          ASCII space, guaranteeing it's rendered
                          properly even when the font doesn't contain
			  that character, which is the case with many
			  X fonts. */
                       (!isISOprint(i) || isISOspace(i)) ? ' ' : i,
		       ccount[i], ((double) ccount[i] / totalc));
		 }
	      }
	   }
	   if (!terse) {
              printf("\nTotal:    %10ld   %f\n\n", totalc, 1.0);
	   }
	}

	/* Print calculated results */

	if (!terse) {
           printf("Entropy = %f bits per %s.\n", ent, samp);
           printf("\nOptimum compression would reduce the size\n");
           printf("of this %ld %s file by %d percent.\n\n", totalc, samp,
		    (short) ((100 * ((binary ? 1 : 8) - ent) /
			      (binary ? 1.0 : 8.0))));
	   printf(
              "Chi square distribution for %ld samples is %1.2f, and randomly\n",
	      totalc, chisq);
	   if (chip < 0.0001) {
              printf("would exceed this value less than 0.01 percent of the times.\n\n");
	   } else if (chip > 0.9999) {
              printf("would exceed this value more than than 99.99 percent of the times.\n\n");
	   } else {
              printf("would exceed this value %1.2f percent of the times.\n\n",
		 chip * 100);
	   }
	   printf(
              "Arithmetic mean value of data %ss is %1.4f (%.1f = random).\n",
	      samp, mean, binary ? 0.5 : 127.5);
           printf("Monte Carlo value for Pi is %1.9f (error %1.2f percent).\n",
	      montepi, 100.0 * (fabs(PI - montepi) / PI));
           printf("Serial correlation coefficient is ");
	   if (scc >= -99999) {
              printf("%1.6f (totally uncorrelated = 0.0).\n", scc);
	   } else {
              printf("undefined (all values equal!).\n");
	   }
	}

	return 0;
}
