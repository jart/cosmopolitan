#ifndef LIBC_ISYSTEM_SYS_MMAN_H_
#define LIBC_ISYSTEM_SYS_MMAN_H_
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/mlock.h"
#include "libc/sysv/consts/msync.h"
#include "libc/sysv/consts/posix.h"
#include "libc/sysv/consts/prot.h"
#if defined(_GNU_SOURCE) || defined(_BSD_SOURCE)
#include "libc/sysv/consts/madv.h"
#endif
#if defined(_GNU_SOURCE)
#include "libc/sysv/consts/mfd.h"
#include "libc/sysv/consts/mremap.h"
#endif
#endif
