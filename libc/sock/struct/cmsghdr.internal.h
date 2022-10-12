#ifndef COSMOPOLITAN_LIBC_SOCK_STRUCT_CMSGHDR_INTERNAL_H_
#define COSMOPOLITAN_LIBC_SOCK_STRUCT_CMSGHDR_INTERNAL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct cmsghdr_bsd {
  uint32_t cmsg_len;
  int32_t cmsg_level;
  int32_t cmsg_type;
};

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SOCK_STRUCT_CMSGHDR_INTERNAL_H_ */
