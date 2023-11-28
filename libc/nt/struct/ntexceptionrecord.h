#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_NTEXCEPTIONRECORD_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_NTEXCEPTIONRECORD_H_

#define kNtExceptionMaximumParameters 15
#define kNtExceptionNoncontinuable    1


struct NtExceptionRecord {
  uint32_t ExceptionCode;                    /* kNtException... */
  uint32_t ExceptionFlags;                   /* kNtExceptionNoncontinuable */
  struct NtExceptionRecord *ExceptionRecord; /* nested exceptions */
  void *ExceptionAddress;                    /* %rip */
  uint32_t NumberParameters;                 /* #ExceptionInformation */
  uint64_t ExceptionInformation[kNtExceptionMaximumParameters];
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_NTEXCEPTIONRECORD_H_ */
