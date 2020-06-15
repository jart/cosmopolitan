#ifndef COSMOPOLITAN_LIBC_NT_ENUM_THREADPRIORITY_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_THREADPRIORITY_H_

#define kNtThreadBasePriorityIdle  (-15)
#define kNtThreadBasePriorityMin   (-2)
#define kNtThreadBasePriorityMax   2
#define kNtThreadBasePriorityLowrt 15

#define kNtThreadPriorityIdle         kNtThreadBasePriorityIdle
#define kNtThreadPriorityLowest       kNtThreadBasePriorityMin
#define kNtThreadPriorityBelowNormal  (kNtThreadPriorityLowest + 1)
#define kNtThreadPriorityNormal       0
#define kNtThreadPriorityAboveNormal  (kNtThreadPriorityHighest - 1)
#define kNtThreadPriorityHighest      kNtThreadBasePriorityMax
#define kNtThreadPriorityTimeCritical kNtThreadBasePriorityLowrt

#endif /* COSMOPOLITAN_LIBC_NT_ENUM_THREADPRIORITY_H_ */
