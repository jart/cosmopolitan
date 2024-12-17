#ifndef COSMOPOLITAN_THIRD_PARTY_TZ_LOCK_H_
#define COSMOPOLITAN_THIRD_PARTY_TZ_LOCK_H_
#include "libc/thread/thread.h"
COSMOPOLITAN_C_START_

extern pthread_mutex_t __localtime_lock_obj;

void __localtime_lock(void);
void __localtime_unlock(void);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_TZ_LOCK_H_ */
