#include "third_party/f2c/f2c.h"
#include "third_party/f2c/fio.h"
#include "third_party/f2c/fmt.h"
/*write sequential formatted external*/

int x_wSL(Void) {
  int n = f__putbuf('\n');
  f__hiwater = f__recpos = f__cursor = 0;
  return (n == 0);
}

static int xw_end(Void) {
  int n;
  if (f__nonl) {
    f__putbuf(n = 0);
    fflush(f__cf);
  } else
    n = f__putbuf('\n');
  f__hiwater = f__recpos = f__cursor = 0;
  return n;
}

static int xw_rev(Void) {
  int n = 0;
  if (f__workdone) {
    n = f__putbuf('\n');
    f__workdone = 0;
  }
  f__hiwater = f__recpos = f__cursor = 0;
  return n;
}

/*start*/
integer s_wsfe(cilist *a) {
  int n;
  if (!f__init) f_init();
  f__reading = 0;
  f__sequential = 1;
  f__formatted = 1;
  f__external = 1;
  if (n = c_sfe(a)) return (n);
  f__elist = a;
  f__hiwater = f__cursor = f__recpos = 0;
  f__nonl = 0;
  f__scale = 0;
  f__fmtbuf = a->cifmt;
  f__cf = f__curunit->ufd;
  if (pars_f(f__fmtbuf) < 0) err(a->cierr, 100, "startio");
  f__putn = x_putc;
  f__doed = w_ed;
  f__doned = w_ned;
  f__doend = xw_end;
  f__dorevert = xw_rev;
  f__donewrec = x_wSL;
  fmt_bg();
  f__cplus = 0;
  f__cblank = f__curunit->ublnk;
  if (f__curunit->uwrt != 1 && f__nowwriting(f__curunit))
    err(a->cierr, errno, "write start");
  return (0);
}
