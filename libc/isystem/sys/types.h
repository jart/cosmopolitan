#ifndef LIBC_ISYSTEM_SYS_TYPES_H_
#define LIBC_ISYSTEM_SYS_TYPES_H_
#include "libc/calls/makedev.h"
#include "libc/calls/weirdtypes.h"

#if defined(_GNU_SOURCE) || defined(_BSD_SOURCE)
typedef unsigned char u_int8_t;
typedef unsigned short u_int16_t;
typedef unsigned u_int32_t;
typedef char *caddr_t;
typedef unsigned char u_char;
typedef unsigned short u_short, ushort;
typedef unsigned u_int, uint;
typedef unsigned long u_long, ulong;
typedef long long quad_t;
typedef unsigned long long u_quad_t;
#include "libc/isystem/endian.h"
#include "libc/isystem/sys/select.h"
#endif

#endif
