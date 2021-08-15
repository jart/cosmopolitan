#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_REBOOT_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_REBOOT_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const uint32_t RB_AUTOBOOT;
extern const uint32_t RB_POWER_OFF;
extern const uint32_t RB_POWERDOWN;
extern const uint32_t RB_POWEROFF;
extern const uint32_t RB_HALT_SYSTEM;
extern const uint32_t RB_HALT;
extern const uint32_t RB_SW_SUSPEND;
extern const uint32_t RB_KEXEC;
extern const uint32_t RB_ENABLE_CAD;
extern const uint32_t RB_DISABLE_CAD;
extern const uint32_t RB_NOSYNC;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define RB_AUTOBOOT    SYMBOLIC(RB_AUTOBOOT)
#define RB_POWER_OFF   SYMBOLIC(RB_POWER_OFF)
#define RB_POWERDOWN   SYMBOLIC(RB_POWERDOWN)
#define RB_POWEROFF    SYMBOLIC(RB_POWEROFF)
#define RB_HALT_SYSTEM SYMBOLIC(RB_HALT_SYSTEM)
#define RB_HALT        SYMBOLIC(RB_HALT)
#define RB_SW_SUSPEND  SYMBOLIC(RB_SW_SUSPEND)
#define RB_KEXEC       SYMBOLIC(RB_KEXEC)
#define RB_ENABLE_CAD  SYMBOLIC(RB_ENABLE_CAD)
#define RB_DISABLE_CAD SYMBOLIC(RB_DISABLE_CAD)
#define RB_NOSYNC      SYMBOLIC(RB_NOSYNC)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_REBOOT_H_ */
