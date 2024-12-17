#ifndef COSMOPOLITAN_LIBC_THREAD_LOCK_H_
#define COSMOPOLITAN_LIBC_THREAD_LOCK_H_
COSMOPOLITAN_C_START_

//
//                                                     ┌undead
//                                                     │
//                                                     │┌dead
//                                                     ││
//                                                     ││┌robust
//                                                     │││
//                                                     │││  ┌depth
//                                                     │││  │
//         COSMOPOLITAN MUTEXES                        │││  │   ┌waited
//                                                     │││  │   │
//                                                     │││  │   │┌locked
//                                                     │││  │   ││
//                                                     │││  │   ││┌pshared
//                owner                                │││  │   │││
//                 tid                                 │││  │   │││┌type
//                  │                                  │││  │   ││││
//   ┌──────────────┴───────────────┐                  │││┌─┴──┐│││├┐
// 0b0000000000000000000000000000000000000000000000000000000000000000
//

#define MUTEX_DEPTH_MIN 0x00000020ull
#define MUTEX_DEPTH_MAX 0x000007e0ull

#define MUTEX_TYPE(word)    ((word) & 3)
#define MUTEX_PSHARED(word) ((word) & 4)
#define MUTEX_LOCKED(word)  ((word) & 8)
#define MUTEX_WAITED(word)  ((word) & 16)
#define MUTEX_DEPTH(word)   ((word) & MUTEX_DEPTH_MAX)
#define MUTEX_OWNER(word)   ((word) >> 32)

#define MUTEX_LOCK(word)                 (((word) & 7) | 8)
#define MUTEX_UNLOCK(word)               ((word) & 7)
#define MUTEX_SET_WAITED(word)           ((word) | 16)
#define MUTEX_SET_TYPE(word, type)       (((word) & ~3ull) | (type))
#define MUTEX_SET_PSHARED(word, pshared) (((word) & ~4ull) | (pshared))
#define MUTEX_INC_DEPTH(word)            ((word) + MUTEX_DEPTH_MIN)
#define MUTEX_DEC_DEPTH(word)            ((word) - MUTEX_DEPTH_MIN)
#define MUTEX_SET_OWNER(word, tid)       ((uint64_t)(tid) << 32 | (uint32_t)(word))

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_THREAD_LOCK_H_ */
