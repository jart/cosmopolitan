#ifndef _LINUX_PARAM_H
#define _LINUX_PARAM_H
#include "libc/runtime/clktck.h"

#ifndef HZ
#define HZ CLK_TCK
#endif

#ifndef EXEC_PAGESIZE
#define EXEC_PAGESIZE 65536
#endif

#ifndef NOGROUP
#define NOGROUP (-1)
#endif

#define MAXHOSTNAMELEN 64

#endif
