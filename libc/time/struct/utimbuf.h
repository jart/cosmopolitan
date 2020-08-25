#ifndef COSMOPOLITAN_LIBC_TIME_STRUCT_UTIMBUF_H_
#define COSMOPOLITAN_LIBC_TIME_STRUCT_UTIMBUF_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct utimbuf {
  int64_t actime;  /* access time */
  int64_t modtime; /* modified time */
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_TIME_STRUCT_UTIMBUF_H_ */
