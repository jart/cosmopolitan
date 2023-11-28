#ifndef COSMOPOLITAN_THIRD_PARTY_MUSL_RAND48_H_
#define COSMOPOLITAN_THIRD_PARTY_MUSL_RAND48_H_
COSMOPOLITAN_C_START_

double drand48(void);
double erand48(unsigned short[3]);
long int lrand48(void);
long int nrand48(unsigned short[3]);
long mrand48(void);
long jrand48(unsigned short[3]);
void srand48(long);
unsigned short *seed48(unsigned short[3]);
void lcong48(unsigned short[7]);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_MUSL_RAND48_H_ */
