#ifndef COSMOPOLITAN_LIBC_INTRIN_DESCRIBEFLAGS_INTERNAL_H_
#define COSMOPOLITAN_LIBC_INTRIN_DESCRIBEFLAGS_INTERNAL_H_
#include "libc/mem/alloca.h"
COSMOPOLITAN_C_START_

struct thatispacked DescribeFlags {
  unsigned flag;
  const char *name;
};

const char *_DescribeFlags(char *, size_t, const struct DescribeFlags *, size_t,
                           const char *, unsigned) libcesque;

const char *_DescribeArchPrctlCode(char[12], int) libcesque;
const char *_DescribeCancelState(char[12], int, int *) libcesque;
const char *_DescribeClockName(char[32], int) libcesque;
const char *_DescribeControlKeyState(char[64], uint32_t) libcesque;
const char *_DescribeDirfd(char[12], int) libcesque;
const char *_DescribeDnotifyFlags(char[80], int) libcesque;
const char *_DescribeErrno(char[30], int) libcesque;
const char *_DescribeFcntlCmd(char[20], int) libcesque;
const char *_DescribeFlockType(char[12], int) libcesque;
const char *_DescribeFutexOp(char[64], int) libcesque;
const char *_DescribeHow(char[12], int) libcesque;
const char *_DescribeInOutInt64(char[23], ssize_t, int64_t *) libcesque;
const char *_DescribeItimer(char[12], int) libcesque;
const char *_DescribeMapFlags(char[64], int) libcesque;
const char *_DescribeMapping(char[8], int, int) libcesque;
const char *_DescribeMremapFlags(char[30], int) libcesque;
const char *_DescribeMsg(char[16], int) libcesque;
const char *_DescribeMsyncFlags(char[48], int) libcesque;
const char *_DescribeNtAllocationType(char[48], uint32_t);
const char *_DescribeNtConsoleInFlags(char[256], uint32_t) libcesque;
const char *_DescribeNtConsoleOutFlags(char[128], uint32_t) libcesque;
const char *_DescribeNtCreationDisposition(uint32_t) libcesque;
const char *_DescribeNtFileAccessFlags(char[512], uint32_t) libcesque;
const char *_DescribeNtFileFlagAttr(char[256], uint32_t) libcesque;
const char *_DescribeNtFileMapFlags(char[64], uint32_t) libcesque;
const char *_DescribeNtFileShareFlags(char[64], uint32_t) libcesque;
const char *_DescribeNtFiletypeFlags(char[64], uint32_t) libcesque;
const char *_DescribeNtLockFileFlags(char[64], uint32_t) libcesque;
const char *_DescribeNtMovFileInpFlags(char[256], uint32_t) libcesque;
const char *_DescribeNtPageFlags(char[64], uint32_t) libcesque;
const char *_DescribeNtPipeModeFlags(char[64], uint32_t) libcesque;
const char *_DescribeNtPipeOpenFlags(char[64], uint32_t) libcesque;
const char *_DescribeNtProcAccessFlags(char[256], uint32_t) libcesque;
const char *_DescribeNtStartFlags(char[128], uint32_t) libcesque;
const char *_DescribeNtSymlinkFlags(char[64], uint32_t) libcesque;
const char *_DescribeOpenFlags(char[128], int) libcesque;
const char *_DescribeOpenMode(char[15], int, int) libcesque;
const char *_DescribePersonalityFlags(char[128], int) libcesque;
const char *_DescribePollFlags(char[64], int) libcesque;
const char *_DescribeProtFlags(char[48], int) libcesque;
const char *_DescribePtrace(char[12], int) libcesque;
const char *_DescribePtraceEvent(char[32], int) libcesque;
const char *_DescribeRlimitName(char[20], int) libcesque;
const char *_DescribeSchedPolicy(char[48], int) libcesque;
const char *_DescribeSeccompOperation(int) libcesque;
const char *_DescribeSiCode(char[20], int, int) libcesque;
const char *_DescribeSigaltstackFlags(char[22], int) libcesque;
const char *_DescribeSleepFlags(char[16], int) libcesque;
const char *_DescribeSockLevel(char[12], int) libcesque;
const char *_DescribeSockOptname(char[32], int, int) libcesque;
const char *_DescribeSocketFamily(char[12], int) libcesque;
const char *_DescribeSocketProtocol(char[12], int) libcesque;
const char *_DescribeSocketType(char[64], int) libcesque;
const char *_DescribeStdioState(char[12], int) libcesque;
const char *_DescribeStringList(char[300], char *const[]) libcesque;
const char *_DescribeThreadCreateFlags(char[64], uint32_t) libcesque;
const char *_DescribeVirtualKeyCode(char[32], uint32_t) libcesque;
const char *_DescribeWhence(char[12], int) libcesque;
const char *_DescribeWhichPrio(char[12], int) libcesque;

#define DescribeCancelState(x, y)    _DescribeCancelState(alloca(12), x, y)
#define DescribeClockName(x)         _DescribeClockName(alloca(32), x)
#define DescribeControlKeyState(x)   _DescribeControlKeyState(alloca(64), x)
#define DescribeDirfd(x)             _DescribeDirfd(alloca(12), x)
#define DescribeDnotifyFlags(x)      _DescribeDnotifyFlags(alloca(80), x)
#define DescribeErrno(x)             _DescribeErrno(alloca(30), x)
#define DescribeFcntlCmd(x)          _DescribeFcntlCmd(alloca(20), x)
#define DescribeFlockType(x)         _DescribeFlockType(alloca(12), x)
#define DescribeFutexOp(x)           _DescribeFutexOp(alloca(64), x)
#define DescribeHow(x)               _DescribeHow(alloca(12), x)
#define DescribeInOutInt64(rc, x)    _DescribeInOutInt64(alloca(23), rc, x)
#define DescribeItimer(x)            _DescribeItimer(alloca(12), x)
#define DescribeMapFlags(x)          _DescribeMapFlags(alloca(64), x)
#define DescribeMapping(x, y)        _DescribeMapping(alloca(8), x, y)
#define DescribeMremapFlags(x)       _DescribeMremapFlags(alloca(30), x)
#define DescribeMsg(x)               _DescribeMsg(alloca(16), x)
#define DescribeMsyncFlags(x)        _DescribeMsyncFlags(alloca(48), x)
#define DescribeNtAllocationType(x)  _DescribeNtAllocationType(alloca(48), x)
#define DescribeNtConsoleInFlags(x)  _DescribeNtConsoleInFlags(alloca(256), x)
#define DescribeNtConsoleOutFlags(x) _DescribeNtConsoleOutFlags(alloca(128), x)
#define DescribeNtFileAccessFlags(x) _DescribeNtFileAccessFlags(alloca(512), x)
#define DescribeNtFileFlagAttr(x)    _DescribeNtFileFlagAttr(alloca(256), x)
#define DescribeNtFileMapFlags(x)    _DescribeNtFileMapFlags(alloca(64), x)
#define DescribeNtFileShareFlags(x)  _DescribeNtFileShareFlags(alloca(64), x)
#define DescribeNtFiletypeFlags(x)   _DescribeNtFiletypeFlags(alloca(64), x)
#define DescribeNtLockFileFlags(x)   _DescribeNtLockFileFlags(alloca(64), x)
#define DescribeNtMovFileInpFlags(x) _DescribeNtMovFileInpFlags(alloca(256), x)
#define DescribeNtPageFlags(x)       _DescribeNtPageFlags(alloca(64), x)
#define DescribeNtPipeModeFlags(x)   _DescribeNtPipeModeFlags(alloca(64), x)
#define DescribeNtPipeOpenFlags(x)   _DescribeNtPipeOpenFlags(alloca(64), x)
#define DescribeNtProcAccessFlags(x) _DescribeNtProcAccessFlags(alloca(256), x)
#define DescribeNtStartFlags(x)      _DescribeNtStartFlags(alloca(128), x)
#define DescribeNtSymlinkFlags(x)    _DescribeNtSymlinkFlags(alloca(64), x)
#define DescribeOpenFlags(x)         _DescribeOpenFlags(alloca(128), x)
#define DescribeOpenMode(x, y)       _DescribeOpenMode(alloca(15), x, y)
#define DescribePersonalityFlags(p)  _DescribePersonalityFlags(alloca(128), p)
#define DescribePollFlags(p)         _DescribePollFlags(alloca(64), p)
#define DescribeProtFlags(x)         _DescribeProtFlags(alloca(48), x)
#define DescribePtrace(i)            _DescribePtrace(alloca(12), i)
#define DescribePtraceEvent(x)       _DescribePtraceEvent(alloca(32), x)
#define DescribeRlimitName(rl)       _DescribeRlimitName(alloca(20), rl)
#define DescribeSchedPolicy(x)       _DescribeSchedPolicy(alloca(48), x)
#define DescribeSiCode(x, y)         _DescribeSiCode(alloca(20), x, y)
#define DescribeSigaltstackFlags(x)  _DescribeSigaltstackFlags(alloca(22), x)
#define DescribeSleepFlags(x)        _DescribeSleepFlags(alloca(16), x)
#define DescribeSockLevel(x)         _DescribeSockLevel(alloca(12), x)
#define DescribeSockOptname(x, y)    _DescribeSockOptname(alloca(32), x, y)
#define DescribeSocketFamily(x)      _DescribeSocketFamily(alloca(12), x)
#define DescribeSocketProtocol(x)    _DescribeSocketProtocol(alloca(12), x)
#define DescribeSocketType(x)        _DescribeSocketType(alloca(64), x)
#define DescribeStdioState(x)        _DescribeStdioState(alloca(12), x)
#define DescribeStringList(x)        _DescribeStringList(alloca(300), x)
#define DescribeThreadCreateFlags(x) _DescribeThreadCreateFlags(alloca(64), x)
#define DescribeVirtualKeyCode(x)    _DescribeVirtualKeyCode(alloca(32), x)
#define DescribeWhence(x)            _DescribeWhence(alloca(12), x)
#define DescribeWhichPrio(x)         _DescribeWhichPrio(alloca(12), x)

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_INTRIN_DESCRIBEFLAGS_INTERNAL_H_ */
