#ifndef COSMOPOLITAN_LIBC_CALLS_KNTPRIORITYCOMBOS_H_
#define COSMOPOLITAN_LIBC_CALLS_KNTPRIORITYCOMBOS_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct NtPriorityCombo {
  int8_t nice; /* sorted by this */
  int8_t lg2tier;
  int8_t wut;
  int8_t prio;
};

hidden extern const unsigned kNtPriorityCombosLen;
hidden extern const struct NtPriorityCombo kNtPriorityCombos[];

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_KNTPRIORITYCOMBOS_H_ */
