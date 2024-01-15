#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_MADV_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_MADV_H_
COSMOPOLITAN_C_START_

extern const unsigned MADV_DODUMP;
extern const unsigned MADV_DOFORK;
extern const unsigned MADV_DONTDUMP;
extern const unsigned MADV_DONTFORK;
extern const unsigned MADV_DONTNEED;
extern const unsigned MADV_FREE;
extern const unsigned MADV_HUGEPAGE;
extern const unsigned MADV_HWPOISON;
extern const unsigned MADV_MERGEABLE;
extern const unsigned MADV_NOHUGEPAGE;
extern const unsigned MADV_NORMAL;
extern const unsigned MADV_RANDOM;
extern const unsigned MADV_REMOVE;
extern const unsigned MADV_SEQUENTIAL;
extern const unsigned MADV_UNMERGEABLE;
extern const unsigned MADV_WILLNEED;
extern const unsigned MADV_SOFT_OFFLINE;
extern const unsigned MADV_WIPEONFORK;
extern const unsigned MADV_KEEPONFORK;
extern const unsigned MADV_COLD;
extern const unsigned MADV_PAGEOUT;
extern const unsigned MADV_POPULATE_READ;
extern const unsigned MADV_POPULATE_WRITE;
extern const unsigned MADV_DONTNEED_LOCKED;
extern const unsigned MADV_COLLAPSE;

#define MADV_NORMAL     0
#define MADV_RANDOM     1
#define MADV_SEQUENTIAL 2
#define MADV_WILLNEED   3

#define MADV_DODUMP          MADV_DODUMP
#define MADV_DOFORK          MADV_DOFORK
#define MADV_DONTDUMP        MADV_DONTDUMP
#define MADV_DONTFORK        MADV_DONTFORK
#define MADV_DONTNEED        MADV_DONTNEED
#define MADV_FREE            MADV_FREE
#define MADV_HUGEPAGE        MADV_HUGEPAGE
#define MADV_HWPOISON        MADV_HWPOISON
#define MADV_MERGEABLE       MADV_MERGEABLE
#define MADV_NOHUGEPAGE      MADV_NOHUGEPAGE
#define MADV_REMOVE          MADV_REMOVE
#define MADV_UNMERGEABLE     MADV_UNMERGEABLE
#define MADV_SOFT_OFFLINE    MADV_SOFT_OFFLINE
#define MADV_WIPEONFORK      MADV_WIPEONFORK
#define MADV_KEEPONFORK      MADV_KEEPONFORK
#define MADV_COLD            MADV_COLD
#define MADV_PAGEOUT         MADV_PAGEOUT
#define MADV_POPULATE_READ   MADV_POPULATE_READ
#define MADV_POPULATE_WRITE  MADV_POPULATE_WRITE
#define MADV_DONTNEED_LOCKED MADV_DONTNEED_LOCKED
#define MADV_COLLAPSE        MADV_COLLAPSE

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_MADV_H_ */
