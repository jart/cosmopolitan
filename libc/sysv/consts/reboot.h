#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_REBOOT_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_REBOOT_H_
COSMOPOLITAN_C_START_

extern const unsigned RB_AUTOBOOT;
extern const unsigned RB_POWER_OFF;
extern const unsigned RB_POWERDOWN;
extern const unsigned RB_POWEROFF;
extern const unsigned RB_HALT_SYSTEM;
extern const unsigned RB_HALT;
extern const unsigned RB_SW_SUSPEND;
extern const unsigned RB_KEXEC;
extern const unsigned RB_ENABLE_CAD;
extern const unsigned RB_DISABLE_CAD;
extern const unsigned RB_NOSYNC;

#define RB_AUTOBOOT    RB_AUTOBOOT
#define RB_POWER_OFF   RB_POWER_OFF
#define RB_POWERDOWN   RB_POWERDOWN
#define RB_POWEROFF    RB_POWEROFF
#define RB_HALT_SYSTEM RB_HALT_SYSTEM
#define RB_HALT        RB_HALT
#define RB_SW_SUSPEND  RB_SW_SUSPEND
#define RB_KEXEC       RB_KEXEC
#define RB_ENABLE_CAD  RB_ENABLE_CAD
#define RB_DISABLE_CAD RB_DISABLE_CAD
#define RB_NOSYNC      RB_NOSYNC


COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_REBOOT_H_ */
