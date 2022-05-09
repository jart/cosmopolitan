#ifndef COSMOPOLITAN_LIBC_INTRIN_DESCRIBEFLAGS_INTERNAL_H_
#define COSMOPOLITAN_LIBC_INTRIN_DESCRIBEFLAGS_INTERNAL_H_
#include "libc/nt/struct/securityattributes.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct thatispacked DescribeFlags {
  unsigned flag;
  const char *name;
};

const char *DescribeFlags(char *, size_t, struct DescribeFlags *, size_t,
                          const char *, unsigned);

const char *DescribeMapFlags(int);
const char *DescribeProtFlags(int);
const char *DescribeRemapFlags(int);
const char *DescribeSeccompOperationFlags(int);
const char *DescribePollFlags(char *, size_t, int);

const char *DescribeNtPageFlags(uint32_t);
const char *DescribeNtStartFlags(uint32_t);
const char *DescribeNtFileMapFlags(uint32_t);
const char *DescribeNtFiletypeFlags(uint32_t);
const char *DescribeNtPipeOpenFlags(uint32_t);
const char *DescribeNtPipeModeFlags(uint32_t);
const char *DescribeNtFileShareFlags(uint32_t);
const char *DescribeNtFileAccessFlags(uint32_t);
const char *DescribeNtSymbolicLinkFlags(uint32_t);
const char *DescribeNtProcessAccessFlags(uint32_t);
const char *DescribeNtMoveFileInputFlags(uint32_t);
const char *DescribeNtCreationDisposition(uint32_t);
const char *DescribeNtConsoleModeInputFlags(uint32_t);
const char *DescribeNtConsoleModeOutputFlags(uint32_t);
const char *DescribeNtFileFlagsAndAttributes(uint32_t);
const char *DescribeNtSecurityAttributes(struct NtSecurityAttributes *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_DESCRIBEFLAGS_INTERNAL_H_ */
