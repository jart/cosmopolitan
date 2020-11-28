#ifndef COSMOPOLITAN_LIBC_NT_ENUM_KEYEDEVENT_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_KEYEDEVENT_H_
#include "libc/nt/enum/accessmask.h"

#define kNtKeyedeventWait 0x00000001u
#define kNtKeyedeventWake 0x00000002u
#define kNtKeyedeventAllAccess \
  (kNtStandardRightsRequired | kNtKeyedeventWait | kNtKeyedeventWake)

#endif /* COSMOPOLITAN_LIBC_NT_ENUM_KEYEDEVENT_H_ */
