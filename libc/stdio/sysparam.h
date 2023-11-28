#ifndef COSMOPOLITAN_LIBC_SYSPARAM_H_
#define COSMOPOLITAN_LIBC_SYSPARAM_H_

#define MAXSYMLINKS    20
#define MAXHOSTNAMELEN 64
#define MAXNAMLEN      255
#define MAXPATHLEN     PATH_MAX
#define NBBY           8
#define NGROUPS        32
#define CANBSIZ        255
#define NOFILE         256
#define NCARGS         131072
#define DEV_BSIZE      512
#define NOGROUP        (-1)

COSMOPOLITAN_C_START_

#define __bitop(x, i, o) ((x)[(i) / 8] o(1 << (i) % 8))
#define setbit(x, i)     __bitop(x, i, |=)
#define clrbit(x, i)     __bitop(x, i, &= ~)
#define isset(x, i)      __bitop(x, i, &)
#define isclr(x, i)      !isset(x, i)

#undef roundup
#define roundup(n, d) (howmany(n, d) * (d))
#define powerof2(n)   !(((n)-1) & (n))
#define howmany(n, d) (((n) + ((d)-1)) / (d))

#ifdef MIN
#undef MIN
#endif
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#ifdef MAX
#undef MAX
#endif
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SYSPARAM_H_ */
