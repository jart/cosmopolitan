#ifndef COSMOPOLITAN_LIBC_LOG_APPENDRESOURCEREPORT_INTERNAL_H_
#define COSMOPOLITAN_LIBC_LOG_APPENDRESOURCEREPORT_INTERNAL_H_
#include "libc/calls/struct/rusage.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void AppendResourceReport(char **, struct rusage *, const char *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_LOG_APPENDRESOURCEREPORT_INTERNAL_H_ */
