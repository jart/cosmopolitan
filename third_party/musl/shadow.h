#ifndef COSMOPOLITAN_THIRD_PARTY_MUSL_SHADOW_H_
#define COSMOPOLITAN_THIRD_PARTY_MUSL_SHADOW_H_
#include "libc/stdio/stdio.h"
COSMOPOLITAN_C_START_

#define	SHADOW "/etc/shadow"

struct spwd {
	char *sp_namp;
	char *sp_pwdp;
	long sp_lstchg;
	long sp_min;
	long sp_max;
	long sp_warn;
	long sp_inact;
	long sp_expire;
	unsigned long sp_flag;
};

void setspent(void);
void endspent(void);
struct spwd *getspent(void);
struct spwd *fgetspent(FILE *);
struct spwd *sgetspent(const char *);
int putspent(const struct spwd *, FILE *);

struct spwd *getspnam(const char *);
int getspnam_r(const char *, struct spwd *, char *, size_t, struct spwd **);

int lckpwdf(void);
int ulckpwdf(void);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_MUSL_SHADOW_H_ */
