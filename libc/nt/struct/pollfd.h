#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_POLLFD_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_POLLFD_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct pollfd$nt {
  int64_t handle;
  int16_t events;
  int16_t revents;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_POLLFD_H_ */
