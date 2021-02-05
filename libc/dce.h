#ifndef COSMOPOLITAN_LIBC_DCE_H_
#define COSMOPOLITAN_LIBC_DCE_H_
#include "libc/nexgen32e/kcpuids.h"
/*─────────────────────────────────────────────────────────────────────────────╗
│ cosmopolitan § autotune » dead code elimination                              │
╚─────────────────────────────────────────────────────────────────────────────*/

#ifndef SUPPORT_VECTOR
/**
 * Supported Platforms Tuning Knob (Runtime & Compile-Time)
 * Tuning this bitmask will remove platform polyfills at compile-time.
 */
#define SUPPORT_VECTOR 0b11111111
#endif

#define LINUX   1
#define METAL   2
#define WINDOWS 4
#define XNU     8
#define OPENBSD 16
#define FREEBSD 32
#define NETBSD  64

#ifdef NDEBUG
#define NoDebug() 1
#else
#define NoDebug() 0
#endif

#ifdef MODE_DBG
#define IsModeDbg() 1
#else
#define IsModeDbg() 0
#endif

#ifdef __MFENTRY__
#define HaveFentry() 1
#else
#define HaveFentry() 0
#endif

#ifdef TRUSTWORTHY
#define IsTrustworthy() 1
#else
#define IsTrustworthy() 0
#endif

#ifdef SECURITY_BLANKETS
#define UseSecurityBlankets() 1
#else
#define UseSecurityBlankets() 0
#endif

#ifdef __MGENERAL_REGS_ONLY__
#define UseGeneralRegsOnly() 1
#else
#define UseGeneralRegsOnly() 0
#endif

#ifdef TINY
#define IsTiny() 1
#else
#define IsTiny() 0
#endif

#ifdef __OPTIMIZE__
#define IsOptimized() 1
#else
#define IsOptimized() 0
#endif

#if defined(__PIE__) || defined(__PIC__)
#define IsPositionIndependent() 1
#else
#define IsPositionIndependent() 0
#endif

#define SupportsLinux()   ((SUPPORT_VECTOR & LINUX) == LINUX)
#define SupportsMetal()   ((SUPPORT_VECTOR & METAL) == METAL)
#define SupportsWindows() ((SUPPORT_VECTOR & WINDOWS) == WINDOWS)
#define SupportsXnu()     ((SUPPORT_VECTOR & XNU) == XNU)
#define SupportsFreebsd() ((SUPPORT_VECTOR & FREEBSD) == FREEBSD)
#define SupportsOpenbsd() ((SUPPORT_VECTOR & OPENBSD) == OPENBSD)
#define SupportsNetbsd()  ((SUPPORT_VECTOR & NETBSD) == NETBSD)
#define SupportsBsd()     (!!(SUPPORT_VECTOR & (XNU | FREEBSD | OPENBSD | NETBSD)))
#define SupportsSystemv() \
  ((SUPPORT_VECTOR & (LINUX | METAL | XNU | OPENBSD | FREEBSD | NETBSD)) != 0)

#ifndef __ASSEMBLER__
#define __HOSTOS    (__hostos & SUPPORT_VECTOR)
#define IsLinux()   ((__HOSTOS & LINUX) == LINUX)
#define IsMetal()   ((__HOSTOS & METAL) == METAL)
#define IsWindows() ((__HOSTOS & WINDOWS) == WINDOWS)
#define IsBsd()     ((__HOSTOS & (XNU | FREEBSD | OPENBSD | NETBSD)) != 0)
#define IsXnu()     ((__HOSTOS & XNU) == XNU)
#define IsFreebsd() ((__HOSTOS & FREEBSD) == FREEBSD)
#define IsOpenbsd() ((__HOSTOS & OPENBSD) == OPENBSD)
#define IsNetbsd()  ((__HOSTOS & NETBSD) == NETBSD)
#else
/* clang-format off */
#define IsLinux() $LINUX,__hostos(%rip)
#define IsMetal() $METAL,__hostos(%rip)
#define IsWindows() $WINDOWS,__hostos(%rip)
#define IsBsd() $XNU|FREEBSD|OPENBSD|NETBSD,__hostos(%rip)
#define IsXnu() $XNU,__hostos(%rip)
#define IsFreebsd() $FREEBSD,__hostos(%rip)
#define IsOpenbsd() $OPENBSD,__hostos(%rip)
#define IsNetbsd() $NETBSD,__hostos(%rip)
/* clang-format on */
#endif

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const int __hostos;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_DCE_H_ */
