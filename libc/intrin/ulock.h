#ifndef COSMOPOLITAN_ULOCK_H_
#define COSMOPOLITAN_ULOCK_H_
COSMOPOLITAN_C_START_

/* both wake and wait take one of these */
#define UL_COMPARE_AND_WAIT          1 /* multi-thread */
#define UL_UNFAIR_LOCK               2
#define UL_COMPARE_AND_WAIT_SHARED   3 /* multi-thread/process */
#define UL_UNFAIR_LOCK64_SHARED      4
#define UL_COMPARE_AND_WAIT64        5
#define UL_COMPARE_AND_WAIT64_SHARED 6

#define ULF_WAKE_ALL             0x00000100
#define ULF_WAKE_THREAD          0x00000200 /* takes wake_value */
#define ULF_WAKE_ALLOW_NON_OWNER 0x00000400

#define ULF_WAIT_WORKQ_DATA_CONTENTION 0x00010000
#define ULF_WAIT_CANCEL_POINT          0x00020000 /* raises eintr */
#define ULF_WAIT_ADAPTIVE_SPIN         0x00040000

int ulock_wake(uint32_t, void *, uint64_t) libcesque;
int ulock_wait(uint32_t, void *, uint64_t, uint32_t) libcesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_ULOCK_H_ */
