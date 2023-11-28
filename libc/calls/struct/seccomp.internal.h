#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SECCOMP_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SECCOMP_H_

#define SECCOMP_SET_MODE_STRICT          0
#define SECCOMP_SET_MODE_FILTER          1
#define SECCOMP_GET_ACTION_AVAIL         2
#define SECCOMP_GET_NOTIF_SIZES          3
#define SECCOMP_FILTER_FLAG_TSYNC        (1UL << 0)
#define SECCOMP_FILTER_FLAG_LOG          (1UL << 1)
#define SECCOMP_FILTER_FLAG_SPEC_ALLOW   (1UL << 2)
#define SECCOMP_FILTER_FLAG_NEW_LISTENER (1UL << 3)
#define SECCOMP_FILTER_FLAG_TSYNC_ESRCH  (1UL << 4)
#define SECCOMP_RET_KILL_PROCESS         0x80000000U
#define SECCOMP_RET_KILL_THREAD          0x00000000U
#define SECCOMP_RET_KILL                 SECCOMP_RET_KILL_THREAD
#define SECCOMP_RET_TRAP                 0x00030000U
#define SECCOMP_RET_ERRNO                0x00050000U
#define SECCOMP_RET_USER_NOTIF           0x7fc00000U
#define SECCOMP_RET_TRACE                0x7ff00000U
#define SECCOMP_RET_LOG                  0x7ffc0000U
#define SECCOMP_RET_ALLOW                0x7fff0000U
#define SECCOMP_RET_ACTION_FULL          0xffff0000U
#define SECCOMP_RET_ACTION               0x7fff0000U
#define SECCOMP_RET_DATA                 0x0000ffffU
#define SECCOMP_USER_NOTIF_FLAG_CONTINUE (1UL << 0)
#define SECCOMP_ADDFD_FLAG_SETFD         (1UL << 0)
#define SECCOMP_ADDFD_FLAG_SEND          (1UL << 1)

COSMOPOLITAN_C_START_

#define SECCOMP_IOC_MAGIC      '!'
#define SECCOMP_IO(nr)         _IO(SECCOMP_IOC_MAGIC, nr)
#define SECCOMP_IOR(nr, type)  _IOR(SECCOMP_IOC_MAGIC, nr, type)
#define SECCOMP_IOW(nr, type)  _IOW(SECCOMP_IOC_MAGIC, nr, type)
#define SECCOMP_IOWR(nr, type) _IOWR(SECCOMP_IOC_MAGIC, nr, type)

#define SECCOMP_IOCTL_NOTIF_RECV     SECCOMP_IOWR(0, struct seccomp_notif)
#define SECCOMP_IOCTL_NOTIF_SEND     SECCOMP_IOWR(1, struct seccomp_notif_resp)
#define SECCOMP_IOCTL_NOTIF_ID_VALID SECCOMP_IOW(2, __u64)
#define SECCOMP_IOCTL_NOTIF_ADDFD    SECCOMP_IOW(3, struct seccomp_notif_addfd)

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

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SECCOMP_H_ */
