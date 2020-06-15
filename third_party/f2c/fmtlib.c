#define MAXINTLENGTH 23

#ifndef Allow_TYQUAD
#undef longint
#define longint long
#undef ulongint
#define ulongint unsigned long
#endif

char *f__icvt(longint value, int *ndigit, int *sign, int base) {
  static char buf[MAXINTLENGTH + 1];
  register int i;
  ulongint uvalue;
  if (value > 0) {
    uvalue = value;
    *sign = 0;
  } else if (value < 0) {
    uvalue = -value;
    *sign = 1;
  } else {
    *sign = 0;
    *ndigit = 1;
    buf[MAXINTLENGTH - 1] = '0';
    return &buf[MAXINTLENGTH - 1];
  }
  i = MAXINTLENGTH;
  do {
    buf[--i] = (uvalue % base) + '0';
    uvalue /= base;
  } while (uvalue > 0);
  *ndigit = MAXINTLENGTH - i;
  return &buf[i];
}
