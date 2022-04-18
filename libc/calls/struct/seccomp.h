#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SECCOMP_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SECCOMP_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct seccomp_data {
  int32_t nr;
  uint32_t arch;
  uint64_t instruction_pointer;
  uint64_t args[6];
};

struct seccomp_notif_sizes {
  uint16_t seccomp_notif;
  uint16_t seccomp_notif_resp;
  uint16_t seccomp_data;
};

struct seccomp_notif {
  uint64_t id;
  uint32_t pid;
  uint32_t flags;
  struct seccomp_data data;
};

struct seccomp_notif_resp {
  uint64_t id;
  int64_t val;
  int32_t error;
  uint32_t flags;
};

struct seccomp_notif_addfd {
  uint64_t id;
  uint32_t flags;
  uint32_t srcfd;
  uint32_t newfd;
  uint32_t newfd_flags;
};

#define SECCOMP_IOC_MAGIC      '!'
#define SECCOMP_IO(nr)         _IO(SECCOMP_IOC_MAGIC, nr)
#define SECCOMP_IOR(nr, type)  _IOR(SECCOMP_IOC_MAGIC, nr, type)
#define SECCOMP_IOW(nr, type)  _IOW(SECCOMP_IOC_MAGIC, nr, type)
#define SECCOMP_IOWR(nr, type) _IOWR(SECCOMP_IOC_MAGIC, nr, type)

#define SECCOMP_IOCTL_NOTIF_RECV     SECCOMP_IOWR(0, struct seccomp_notif)
#define SECCOMP_IOCTL_NOTIF_SEND     SECCOMP_IOWR(1, struct seccomp_notif_resp)
#define SECCOMP_IOCTL_NOTIF_ID_VALID SECCOMP_IOW(2, __u64)
#define SECCOMP_IOCTL_NOTIF_ADDFD    SECCOMP_IOW(3, struct seccomp_notif_addfd)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SECCOMP_H_ */
