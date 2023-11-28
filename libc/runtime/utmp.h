#ifndef COSMOPOLITAN_LIBC_RUNTIME_UTMP_H_
#define COSMOPOLITAN_LIBC_RUNTIME_UTMP_H_
#include "libc/calls/weirdtypes.h"
#include "libc/runtime/utmpx.h"

#define ACCOUNTING  9
#define UT_NAMESIZE 32
#define UT_HOSTSIZE 256
#define UT_LINESIZE 32

COSMOPOLITAN_C_START_

struct lastlog {
  time_t ll_time;
  char ll_line[UT_LINESIZE];
  char ll_host[UT_HOSTSIZE];
};

#define ut_time       ut_tv.tv_sec
#define ut_name       ut_user
#define ut_addr       ut_addr_v6[0]
#define utmp          utmpx
#define e_exit        __e_exit
#define e_termination __e_termination

int login_tty(int);
int utmpname(const char *);
struct utmp *getutent(void);
struct utmp *getutid(const struct utmp *);
struct utmp *getutline(const struct utmp *);
struct utmp *pututline(const struct utmp *);
void endutent(void);
void setutent(void);
void updwtmp(const char *, const struct utmp *);

#define _PATH_UTMP "/dev/null/utmp"
#define _PATH_WTMP "/dev/null/wtmp"

#define UTMP_FILE     _PATH_UTMP
#define WTMP_FILE     _PATH_WTMP
#define UTMP_FILENAME _PATH_UTMP
#define WTMP_FILENAME _PATH_WTMP

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_RUNTIME_UTMP_H_ */
