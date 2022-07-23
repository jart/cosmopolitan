#ifndef COSMOPOLITAN_LIBC_INTRIN_DESCRIBEFLAGS_INTERNAL_H_
#define COSMOPOLITAN_LIBC_INTRIN_DESCRIBEFLAGS_INTERNAL_H_
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/rlimit.h"
#include "libc/calls/struct/sched_param.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigaltstack.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timeval.h"
#include "libc/mem/alloca.h"
#include "libc/nt/struct/iovec.h"
#include "libc/nt/struct/securityattributes.h"
#include "libc/sock/struct/sockaddr.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct thatispacked DescribeFlags {
  unsigned flag;
  const char *name;
};

const char *DescribeFlags(char *, size_t, struct DescribeFlags *, size_t,
                          const char *, unsigned);

const char *DescribeCapability(char[20], int);
const char *DescribeClockName(char[32], int);
const char *DescribeDirfd(char[12], int);
const char *DescribeFrame(char[32], int);
const char *DescribeFutexResult(char[12], int);
const char *DescribeHow(char[12], int);
const char *DescribeMapFlags(char[64], int);
const char *DescribeMapping(char[8], int, int);
const char *DescribeNtConsoleInFlags(char[256], uint32_t);
const char *DescribeNtConsoleOutFlags(char[128], uint32_t);
const char *DescribeNtCreationDisposition(uint32_t);
const char *DescribeNtFileAccessFlags(char[512], uint32_t);
const char *DescribeNtFileFlagAttr(char[256], uint32_t);
const char *DescribeNtFileMapFlags(char[64], uint32_t);
const char *DescribeNtFileShareFlags(char[64], uint32_t);
const char *DescribeNtFiletypeFlags(char[64], uint32_t);
const char *DescribeNtMovFileInpFlags(char[256], uint32_t);
const char *DescribeNtPageFlags(char[64], uint32_t);
const char *DescribeNtPipeModeFlags(char[64], uint32_t);
const char *DescribeNtPipeOpenFlags(char[64], uint32_t);
const char *DescribeNtProcAccessFlags(char[256], uint32_t);
const char *DescribeNtSecurityAttributes(struct NtSecurityAttributes *);
const char *DescribeNtStartFlags(char[128], uint32_t);
const char *DescribeNtSymlinkFlags(char[64], uint32_t);
const char *DescribeOpenFlags(char[128], int);
const char *DescribePersonalityFlags(char[128], int);
const char *DescribePollFlags(char[64], int);
const char *DescribePrctlOperation(int);
const char *DescribeProtFlags(char[48], int);
const char *DescribeRemapFlags(char[48], int);
const char *DescribeRlimit(char[64], int, const struct rlimit *);
const char *DescribeRlimitName(char[20], int);
const char *DescribeSchedParam(char[32], const struct sched_param *);
const char *DescribeSchedPolicy(char[48], int);
const char *DescribeSeccompOperation(int);
const char *DescribeSigaction(char[256], int, const struct sigaction *);
const char *DescribeSigaltstk(char[128], int, const struct sigaltstack *);
const char *DescribeSigset(char[128], int, const sigset_t *);
const char *DescribeSockLevel(char[12], int);
const char *DescribeSockOptname(char[32], int, int);
const char *DescribeSockaddr(char[128], const struct sockaddr *, size_t);
const char *DescribeSocketFamily(char[12], int);
const char *DescribeSocketProtocol(char[12], int);
const char *DescribeSocketType(char[64], int);
const char *DescribeStat(char[300], int, const struct stat *);
const char *DescribeTimespec(char[45], int, const struct timespec *);
const char *DescribeTimeval(char[45], int, const struct timeval *);

void DescribeIov(const struct iovec *, int, ssize_t);
void DescribeIovNt(const struct NtIovec *, uint32_t, ssize_t);

#define DescribeCapability(x)        DescribeCapability(alloca(20), x)
#define DescribeClockName(x)         DescribeClockName(alloca(32), x)
#define DescribeDirfd(dirfd)         DescribeDirfd(alloca(12), dirfd)
#define DescribeFrame(x)             DescribeFrame(alloca(32), x)
#define DescribeFutexResult(x)       DescribeFutexResult(alloca(12), x)
#define DescribeHow(x)               DescribeHow(alloca(12), x)
#define DescribeMapFlags(dirfd)      DescribeMapFlags(alloca(64), dirfd)
#define DescribeMapping(x, y)        DescribeMapping(alloca(8), x, y)
#define DescribeNtConsoleInFlags(x)  DescribeNtConsoleInFlags(alloca(256), x)
#define DescribeNtConsoleOutFlags(x) DescribeNtConsoleOutFlags(alloca(128), x)
#define DescribeNtFileAccessFlags(x) DescribeNtFileAccessFlags(alloca(512), x)
#define DescribeNtFileFlagAttr(x)    DescribeNtFileFlagAttr(alloca(256), x)
#define DescribeNtFileMapFlags(x)    DescribeNtFileMapFlags(alloca(64), x)
#define DescribeNtFileShareFlags(x)  DescribeNtFileShareFlags(alloca(64), x)
#define DescribeNtFiletypeFlags(x)   DescribeNtFiletypeFlags(alloca(64), x)
#define DescribeNtMovFileInpFlags(x) DescribeNtMovFileInpFlags(alloca(256), x)
#define DescribeNtPageFlags(x)       DescribeNtPageFlags(alloca(64), x)
#define DescribeNtPipeModeFlags(x)   DescribeNtPipeModeFlags(alloca(64), x)
#define DescribeNtPipeOpenFlags(x)   DescribeNtPipeOpenFlags(alloca(64), x)
#define DescribeNtProcAccessFlags(x) DescribeNtProcAccessFlags(alloca(256), x)
#define DescribeNtStartFlags(x)      DescribeNtStartFlags(alloca(128), x)
#define DescribeNtSymlinkFlags(x)    DescribeNtSymlinkFlags(alloca(64), x)
#define DescribeOpenFlags(x)         DescribeOpenFlags(alloca(128), x)
#define DescribePersonalityFlags(p)  DescribePersonalityFlags(alloca(128), p)
#define DescribePollFlags(p)         DescribePollFlags(alloca(64), p)
#define DescribeProtFlags(dirfd)     DescribeProtFlags(alloca(48), dirfd)
#define DescribeRemapFlags(dirfd)    DescribeRemapFlags(alloca(48), dirfd)
#define DescribeRlimit(rc, rl)       DescribeRlimit(alloca(64), rc, rl)
#define DescribeRlimitName(rl)       DescribeRlimitName(alloca(20), rl)
#define DescribeSchedParam(x)        DescribeSchedParam(alloca(32), x)
#define DescribeSchedPolicy(x)       DescribeSchedPolicy(alloca(48), x)
#define DescribeSigaction(rc, sa)    DescribeSigaction(alloca(256), rc, sa)
#define DescribeSigaltstk(rc, ss)    DescribeSigaltstk(alloca(128), rc, ss)
#define DescribeSigset(rc, ss)       DescribeSigset(alloca(128), rc, ss)
#define DescribeSockLevel(x)         DescribeSockLevel(alloca(12), x)
#define DescribeSockOptname(x, y)    DescribeSockOptname(alloca(32), x, y)
#define DescribeSockaddr(sa, sz)     DescribeSockaddr(alloca(128), sa, sz)
#define DescribeSocketFamily(x)      DescribeSocketFamily(alloca(12), x)
#define DescribeSocketProtocol(x)    DescribeSocketProtocol(alloca(12), x)
#define DescribeSocketType(x)        DescribeSocketType(alloca(64), x)
#define DescribeStat(rc, st)         DescribeStat(alloca(300), rc, st)
#define DescribeTimespec(rc, ts)     DescribeTimespec(alloca(45), rc, ts)
#define DescribeTimeval(rc, ts)      DescribeTimeval(alloca(45), rc, ts)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_DESCRIBEFLAGS_INTERNAL_H_ */
