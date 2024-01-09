#ifndef COSMOPOLITAN_LIBC_INTRIN_DESCRIBEFLAGS_INTERNAL_H_
#define COSMOPOLITAN_LIBC_INTRIN_DESCRIBEFLAGS_INTERNAL_H_
#include "libc/mem/alloca.h"
COSMOPOLITAN_C_START_

struct thatispacked DescribeFlags {
  unsigned flag;
  const char *name;
};

const char *DescribeFlags(char *, size_t, const struct DescribeFlags *, size_t,
                          const char *, unsigned) libcesque;

const char *DescribeArchPrctlCode(char[12], int) libcesque;
const char *DescribeCancelState(char[12], int, int *) libcesque;
const char *DescribeCapability(char[32], int) libcesque;
const char *DescribeClockName(char[32], int) libcesque;
const char *DescribeControlKeyState(char[64], uint32_t) libcesque;
const char *DescribeDirfd(char[12], int) libcesque;
const char *DescribeDnotifyFlags(char[80], int) libcesque;
const char *DescribeErrno(char[30], int) libcesque;
const char *DescribeFcntlCmd(char[20], int) libcesque;
const char *DescribeFlockType(char[12], int) libcesque;
const char *DescribeFrame(char[32], int) libcesque;
const char *DescribeFutexOp(char[64], int) libcesque;
const char *DescribeHow(char[12], int) libcesque;
const char *DescribeInOutInt64(char[23], ssize_t, int64_t *) libcesque;
const char *DescribeItimer(char[12], int) libcesque;
const char *DescribeMapFlags(char[64], int) libcesque;
const char *DescribeMapping(char[8], int, int) libcesque;
const char *DescribeNtConsoleInFlags(char[256], uint32_t) libcesque;
const char *DescribeNtConsoleOutFlags(char[128], uint32_t) libcesque;
const char *DescribeNtCreationDisposition(uint32_t) libcesque;
const char *DescribeNtFileAccessFlags(char[512], uint32_t) libcesque;
const char *DescribeNtFileFlagAttr(char[256], uint32_t) libcesque;
const char *DescribeNtFileMapFlags(char[64], uint32_t) libcesque;
const char *DescribeNtFileShareFlags(char[64], uint32_t) libcesque;
const char *DescribeNtFiletypeFlags(char[64], uint32_t) libcesque;
const char *DescribeNtLockFileFlags(char[64], uint32_t) libcesque;
const char *DescribeNtMovFileInpFlags(char[256], uint32_t) libcesque;
const char *DescribeNtPageFlags(char[64], uint32_t) libcesque;
const char *DescribeNtPipeModeFlags(char[64], uint32_t) libcesque;
const char *DescribeNtPipeOpenFlags(char[64], uint32_t) libcesque;
const char *DescribeNtProcAccessFlags(char[256], uint32_t) libcesque;
const char *DescribeNtStartFlags(char[128], uint32_t) libcesque;
const char *DescribeNtSymlinkFlags(char[64], uint32_t) libcesque;
const char *DescribeOpenFlags(char[128], int) libcesque;
const char *DescribeOpenMode(char[15], int, int) libcesque;
const char *DescribePersonalityFlags(char[128], int) libcesque;
const char *DescribePollFlags(char[64], int) libcesque;
const char *DescribePrctlOperation(int) libcesque;
const char *DescribeProtFlags(char[48], int) libcesque;
const char *DescribePtrace(char[12], int) libcesque;
const char *DescribePtraceEvent(char[32], int) libcesque;
const char *DescribeRemapFlags(char[48], int) libcesque;
const char *DescribeRlimitName(char[20], int) libcesque;
const char *DescribeSchedPolicy(char[48], int) libcesque;
const char *DescribeSeccompOperation(int) libcesque;
const char *DescribeSiCode(char[20], int, int) libcesque;
const char *DescribeSleepFlags(char[16], int) libcesque;
const char *DescribeSockLevel(char[12], int) libcesque;
const char *DescribeSockOptname(char[32], int, int) libcesque;
const char *DescribeSocketFamily(char[12], int) libcesque;
const char *DescribeSocketProtocol(char[12], int) libcesque;
const char *DescribeSocketType(char[64], int) libcesque;
const char *DescribeStdioState(char[12], int) libcesque;
const char *DescribeStringList(char[300], char *const[]) libcesque;
const char *DescribeThreadCreateFlags(char[64], uint32_t) libcesque;
const char *DescribeVirtualKeyCode(char[32], uint32_t) libcesque;
const char *DescribeWhence(char[12], int) libcesque;
const char *DescribeWhichPrio(char[12], int) libcesque;

#define DescribeArchPrctlCode(x)     DescribeArchPrctlCode(alloca(12), x)
#define DescribeCancelState(x, y)    DescribeCancelState(alloca(12), x, y)
#define DescribeCapability(x)        DescribeCapability(alloca(32), x)
#define DescribeClockName(x)         DescribeClockName(alloca(32), x)
#define DescribeControlKeyState(x)   DescribeControlKeyState(alloca(64), x)
#define DescribeDirfd(x)             DescribeDirfd(alloca(12), x)
#define DescribeDnotifyFlags(x)      DescribeDnotifyFlags(alloca(80), x)
#define DescribeErrno(x)             DescribeErrno(alloca(30), x)
#define DescribeFcntlCmd(x)          DescribeFcntlCmd(alloca(20), x)
#define DescribeFlockType(x)         DescribeFlockType(alloca(12), x)
#define DescribeFrame(x)             DescribeFrame(alloca(32), x)
#define DescribeFutexOp(x)           DescribeFutexOp(alloca(64), x)
#define DescribeHow(x)               DescribeHow(alloca(12), x)
#define DescribeInOutInt64(rc, x)    DescribeInOutInt64(alloca(23), rc, x)
#define DescribeItimer(x)            DescribeItimer(alloca(12), x)
#define DescribeMapFlags(x)          DescribeMapFlags(alloca(64), x)
#define DescribeMapping(x, y)        DescribeMapping(alloca(8), x, y)
#define DescribeNtConsoleInFlags(x)  DescribeNtConsoleInFlags(alloca(256), x)
#define DescribeNtConsoleOutFlags(x) DescribeNtConsoleOutFlags(alloca(128), x)
#define DescribeNtFileAccessFlags(x) DescribeNtFileAccessFlags(alloca(512), x)
#define DescribeNtFileFlagAttr(x)    DescribeNtFileFlagAttr(alloca(256), x)
#define DescribeNtFileMapFlags(x)    DescribeNtFileMapFlags(alloca(64), x)
#define DescribeNtFileShareFlags(x)  DescribeNtFileShareFlags(alloca(64), x)
#define DescribeNtFiletypeFlags(x)   DescribeNtFiletypeFlags(alloca(64), x)
#define DescribeNtLockFileFlags(x)   DescribeNtLockFileFlags(alloca(64), x)
#define DescribeNtMovFileInpFlags(x) DescribeNtMovFileInpFlags(alloca(256), x)
#define DescribeNtPageFlags(x)       DescribeNtPageFlags(alloca(64), x)
#define DescribeNtPipeModeFlags(x)   DescribeNtPipeModeFlags(alloca(64), x)
#define DescribeNtPipeOpenFlags(x)   DescribeNtPipeOpenFlags(alloca(64), x)
#define DescribeNtProcAccessFlags(x) DescribeNtProcAccessFlags(alloca(256), x)
#define DescribeNtStartFlags(x)      DescribeNtStartFlags(alloca(128), x)
#define DescribeNtSymlinkFlags(x)    DescribeNtSymlinkFlags(alloca(64), x)
#define DescribeOpenFlags(x)         DescribeOpenFlags(alloca(128), x)
#define DescribeOpenMode(x, y)       DescribeOpenMode(alloca(15), x, y)
#define DescribePersonalityFlags(p)  DescribePersonalityFlags(alloca(128), p)
#define DescribePollFlags(p)         DescribePollFlags(alloca(64), p)
#define DescribeProtFlags(x)         DescribeProtFlags(alloca(48), x)
#define DescribePtrace(i)            DescribePtrace(alloca(12), i)
#define DescribePtraceEvent(x)       DescribePtraceEvent(alloca(32), x)
#define DescribeRemapFlags(x)        DescribeRemapFlags(alloca(48), x)
#define DescribeRlimitName(rl)       DescribeRlimitName(alloca(20), rl)
#define DescribeSchedPolicy(x)       DescribeSchedPolicy(alloca(48), x)
#define DescribeSiCode(x, y)         DescribeSiCode(alloca(20), x, y)
#define DescribeSleepFlags(x)        DescribeSleepFlags(alloca(16), x)
#define DescribeSockLevel(x)         DescribeSockLevel(alloca(12), x)
#define DescribeSockOptname(x, y)    DescribeSockOptname(alloca(32), x, y)
#define DescribeSocketFamily(x)      DescribeSocketFamily(alloca(12), x)
#define DescribeSocketProtocol(x)    DescribeSocketProtocol(alloca(12), x)
#define DescribeSocketType(x)        DescribeSocketType(alloca(64), x)
#define DescribeStdioState(x)        DescribeStdioState(alloca(12), x)
#define DescribeStringList(x)        DescribeStringList(alloca(300), x)
#define DescribeThreadCreateFlags(x) DescribeThreadCreateFlags(alloca(64), x)
#define DescribeVirtualKeyCode(x)    DescribeVirtualKeyCode(alloca(32), x)
#define DescribeWhence(x)            DescribeWhence(alloca(12), x)
#define DescribeWhichPrio(x)         DescribeWhichPrio(alloca(12), x)

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_INTRIN_DESCRIBEFLAGS_INTERNAL_H_ */
