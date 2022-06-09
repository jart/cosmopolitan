#ifndef LIBC_ISYSTEM_SYS_STAT_H_
#define LIBC_ISYSTEM_SYS_STAT_H_
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/stat.macros.h"
#include "libc/calls/weirdtypes.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/consts/utime.h"
#include "libc/time/time.h"

#define st_atime      st_atim.tv_sec
#define st_atime_nsec st_atim.tv_nsec
#define st_mtime      st_mtim.tv_sec
#define st_mtime_nsec st_mtim.tv_nsec
#define st_ctime      st_ctim.tv_sec
#define st_ctime_nsec st_ctim.tv_nsec

#endif
