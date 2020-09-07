#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/bits/bits.h"
#include "libc/inttypes.h"
#include "libc/literal.h"
#include "libc/math.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"

static const char kHeaderBin[] = "\
/\t┌sign\n\
/\t│ ┌exponent\n\
/\t│ │             ┌intpart\n\
/\t│ │             │ ┌fraction\n\
/\t│ │             │ │\n\
/\t│┌┴────────────┐│┌┴────────────────────────────────────────────────────────────┐\n";

static const char kHeaderHex[] = "\
/\t ┌sign/exponent\n\
/\t │   ┌intpart/fraction\n\
/\t │   │\n\
/\t┌┴─┐┌┴─────────────┐\n";

void dobin(const char *op, long double x, FILE *f) {
  uint16_t hi;
  uint64_t lo;
  uint8_t buf[16];
  memcpy(buf, &x, sizeof(x));
  memcpy(&lo, &buf[0], sizeof(lo));
  memcpy(&hi, &buf[8], sizeof(hi));
  fprintf(f, "/\t%016" PRIb16 "%064" PRIb64 " %-8s % 17.14Lf\n", hi, lo, op, x);
}

void dohex(const char *op, long double x, FILE *f) {
  uint16_t hi;
  uint64_t lo;
  uint8_t buf[16];
  memcpy(buf, &x, sizeof(x));
  memcpy(&lo, &buf[0], sizeof(lo));
  memcpy(&hi, &buf[8], sizeof(hi));
  fprintf(f, "/\t%04" PRIx16 "%016" PRIx64 " %-8s % 17.14Lf\n", hi, lo, op, x);
}

#define DOBIN(OP)               \
  dobin(" " #OP, OP(), stdout); \
  dobin("-" #OP, -OP(), stdout)

#define DOHEX(OP)               \
  dohex(" " #OP, OP(), stdout); \
  dohex("-" #OP, -OP(), stdout)

int main(int argc, char *argv[]) {
  fputs(kHeaderBin, stdout);
  DOBIN(fldz);
  DOBIN(fld1);
  DOBIN(fldpi);
  DOBIN(fldl2t);
  DOBIN(fldlg2);
  DOBIN(fldln2);
  DOBIN(fldl2e);
  fputc('\n', stdout);
  fputs(kHeaderHex, stdout);
  DOHEX(fldz);
  DOHEX(fld1);
  DOHEX(fldpi);
  DOHEX(fldl2t);
  DOHEX(fldlg2);
  DOHEX(fldln2);
  DOHEX(fldl2e);
  return 0;
}

// clang-format off
//
//	┌sign
//	│ ┌exponent
//	│ │             ┌intpart
//	│ │             │ ┌fraction
//	│ │             │ │
//	│┌┴────────────┐│┌┴────────────────────────────────────────────────────────────┐
//	00000000000000000000000000000000000000000000000000000000000000000000000000000000  fldz    0.0000000000000000000
//	10000000000000000000000000000000000000000000000000000000000000000000000000000000 -fldz    -0.0000000000000000000
//	00111111111111111000000000000000000000000000000000000000000000000000000000000000  fld1    1.0000000000000000000
//	10111111111111111000000000000000000000000000000000000000000000000000000000000000 -fld1    -1.0000000000000000000
//	01000000000000001100100100001111110110101010001000100001011010001100001000110101  fldpi   3.1415926540000000000
//	11000000000000001100100100001111110110101010001000100001011010001100001000110101 -fldpi   -3.1415926540000000000
//	01000000000000001101010010011010011110000100101111001101000110111000101011111110  fldl2t  3.3219280950000000000
//	11000000000000001101010010011010011110000100101111001101000110111000101011111110 -fldl2t  -3.3219280950000000000
//	00111111111111011001101000100000100110101000010011111011110011111111011110011001  fldlg2  0.3010299960000000000
//	10111111111111011001101000100000100110101000010011111011110011111111011110011001 -fldlg2  -0.3010299960000000000
//	00111111111111101011000101110010000101111111011111010001110011110111100110101100  fldln2  0.6931471810000000000
//	10111111111111101011000101110010000101111111011111010001110011110111100110101100 -fldln2  -0.6931471810000000000
//	00111111111111111011100010101010001110110010100101011100000101111111000010111100  fldl2e  1.4426950410000000000
//	10111111111111111011100010101010001110110010100101011100000101111111000010111100 -fldl2e  -1.4426950410000000000
//
//	 ┌sign/exponent
//	 │   ┌intpart/fraction
//	 │   │
//	┌┴─┐┌┴─────────────┐
//	00000000000000000000  fldz    0.0000000000000000000
//	80000000000000000000 -fldz    -0.0000000000000000000
//	3fff8000000000000000  fld1    1.0000000000000000000
//	bfff8000000000000000 -fld1    -1.0000000000000000000
//	4000c90fdaa22168c235  fldpi   3.1415926540000000000
//	c000c90fdaa22168c235 -fldpi   -3.1415926540000000000
//	4000d49a784bcd1b8afe  fldl2t  3.3219280950000000000
//	c000d49a784bcd1b8afe -fldl2t  -3.3219280950000000000
//	3ffd9a209a84fbcff799  fldlg2  0.3010299960000000000
//	bffd9a209a84fbcff799 -fldlg2  -0.3010299960000000000
//	3ffeb17217f7d1cf79ac  fldln2  0.6931471810000000000
//	bffeb17217f7d1cf79ac -fldln2  -0.6931471810000000000
//	3fffb8aa3b295c17f0bc  fldl2e  1.4426950410000000000
//	bfffb8aa3b295c17f0bc -fldl2e  -1.4426950410000000000
