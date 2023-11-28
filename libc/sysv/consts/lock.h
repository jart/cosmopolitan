#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_LOCK_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_LOCK_H_
COSMOPOLITAN_C_START_

extern const int LOCK_EX;
extern const int LOCK_NB;
extern const int LOCK_SH;
extern const int LOCK_UN;
extern const int LOCK_UNLOCK_CACHE;

COSMOPOLITAN_C_END_

#define LOCK_EX 2
#define LOCK_NB LOCK_NB
#define LOCK_SH LOCK_SH
#define LOCK_UN LOCK_UN


#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_LOCK_H_ */
