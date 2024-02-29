#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_MAP_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_MAP_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const int MAP_32BIT;
extern const int MAP_ANON;
extern const int MAP_ANONYMOUS;
extern const int MAP_CONCEAL;
extern const int MAP_DENYWRITE;
extern const int MAP_EXECUTABLE;
extern const int MAP_FILE;
extern const int MAP_FIXED;
extern const int MAP_FIXED_NOREPLACE;
extern const int MAP_HASSEMAPHORE;
extern const int MAP_INHERIT;
extern const int MAP_JIT;
extern const int MAP_LOCKED;
extern const int MAP_NOCACHE;
extern const int MAP_NOEXTEND;
extern const int MAP_NONBLOCK;
extern const int MAP_NORESERVE;
extern const int MAP_NOSYNC;
extern const int MAP_POPULATE;
extern const int MAP_PRIVATE;
extern const int MAP_SHARED;
extern const int MAP_SHARED_VALIDATE;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define MAP_FILE    0
#define MAP_SHARED  1
#define MAP_PRIVATE 2
#define MAP_STACK   6
#define MAP_TYPE    15
#define MAP_FIXED   16

#define MAP_ANONYMOUS       MAP_ANONYMOUS
#define MAP_CONCEAL         MAP_CONCEAL
#define MAP_EXECUTABLE      MAP_EXECUTABLE
#define MAP_FIXED_NOREPLACE MAP_FIXED_NOREPLACE
#define MAP_HASSEMAPHORE    MAP_HASSEMAPHORE
#define MAP_POPULATE        MAP_POPULATE
#define MAP_NORESERVE       MAP_NORESERVE

#define MAP_ANON   MAP_ANONYMOUS
#define MAP_NOCORE MAP_CONCEAL

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_MAP_H_ */
