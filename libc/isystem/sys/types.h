#ifndef LIBC_ISYSTEM_SYS_TYPES_H_
#define LIBC_ISYSTEM_SYS_TYPES_H_
#include "libc/calls/makedev.h"
#include "libc/calls/weirdtypes.h"

#if defined(_GNU_SOURCE) || defined(_BSD_SOURCE)
#include "libc/bits/newbie.h"
#include "libc/calls/typedef/u.h"
#include "libc/calls/weirdtypes.h"
#include "libc/sock/select.h"
#include "libc/sysv/consts/endian.h"
#endif

#endif
