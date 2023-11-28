#ifndef COSMOPOLITAN_LIBC_SOCK_STRUCT_CMSGHDR_H_
#define COSMOPOLITAN_LIBC_SOCK_STRUCT_CMSGHDR_H_
COSMOPOLITAN_C_START_

#define CMSG_DATA(cmsg) ((unsigned char *)(((struct cmsghdr *)(cmsg)) + 1))

#define CMSG_FIRSTHDR(mhdr)                                 \
  ((size_t)(mhdr)->msg_controllen >= sizeof(struct cmsghdr) \
       ? (struct cmsghdr *)(mhdr)->msg_control              \
       : (struct cmsghdr *)0)

#define CMSG_NXTHDR(mhdr, cmsg)                           \
  ((cmsg)->cmsg_len < sizeof(struct cmsghdr) ||           \
           __CMSG_LEN(cmsg) + sizeof(struct cmsghdr) >=   \
               __MHDR_END(mhdr) - (unsigned char *)(cmsg) \
       ? 0                                                \
       : (struct cmsghdr *)__CMSG_NEXT(cmsg))

#define CMSG_ALIGN(len) \
  (((len) + sizeof(size_t) - 1) & (size_t) ~(sizeof(size_t) - 1))

#define CMSG_SPACE(len) (CMSG_ALIGN(len) + CMSG_ALIGN(sizeof(struct cmsghdr)))

#define CMSG_LEN(len) (CMSG_ALIGN(sizeof(struct cmsghdr)) + (len))

#define __CMSG_LEN(cmsg) \
  (((cmsg)->cmsg_len + sizeof(long) - 1) & ~(long)(sizeof(long) - 1))
#define __CMSG_NEXT(cmsg) ((unsigned char *)(cmsg) + __CMSG_LEN(cmsg))
#define __MHDR_END(mhdr) \
  ((unsigned char *)(mhdr)->msg_control + (mhdr)->msg_controllen)

struct cmsghdr { /* linux abi */
  uint32_t cmsg_len;
  uint32_t __pad1;
  int32_t cmsg_level;
  int32_t cmsg_type;
};

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SOCK_STRUCT_CMSGHDR_H_ */
