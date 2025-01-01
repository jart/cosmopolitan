#ifndef COSMOPOLITAN_THIRD_PARTY_GDTOA_LOCK_H_
#define COSMOPOLITAN_THIRD_PARTY_GDTOA_LOCK_H_
#include "libc/thread/thread.h"
COSMOPOLITAN_C_START_

void __gdtoa_lock(void);
void __gdtoa_unlock(void);
void __gdtoa_wipe(void);

void __gdtoa_lock1(void);
void __gdtoa_unlock1(void);
void __gdtoa_wipe1(void);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_GDTOA_LOCK_H_ */
