#ifndef COSMOPOLITAN_LIBC_NT_ENUM_OFFERPRIORITY_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_OFFERPRIORITY_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

enum NtOfferPriority {
  /* TODO(jart): Are these values correct? */
  kNtVmOfferPriorityVeryLow = 1, /* 0x00001000? */
  kNtVmOfferPriorityLow,         /* 0x00002000? */
  kNtVmOfferPriorityBelowNormal, /* 0x00002000? */
  kNtVmOfferPriorityNormal       /* 0x00002000? */
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_ENUM_OFFERPRIORITY_H_ */
