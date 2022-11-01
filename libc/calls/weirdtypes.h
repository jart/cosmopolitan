#ifndef COSMOPOLITAN_LIBC_CALLS_WEIRDTYPES_H_
#define COSMOPOLITAN_LIBC_CALLS_WEIRDTYPES_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

/**
 * @fileoverview Types we'd prefer hadn't been invented.
 */

#define blkcnt_t      int64_t
#define blksize_t     int64_t /* int32_t on xnu */
#define cc_t          uint8_t
#define clock_t       int64_t  /* uint64_t on xnu */
#define dev_t         uint64_t /* int32_t on xnu */
#define fsblkcnt_t    uint64_t
#define fsfilcnt_t    int64_t /* uint32_t on xnu */
#define gid_t         uint32_t
#define id_t          uint32_t /* int32_t on linux/freebsd/etc. */
#define in_addr_t     uint32_t
#define in_addr_t     uint32_t
#define in_port_t     uint16_t
#define ino_t         uint64_t
#define key_t         int32_t
#define loff_t        int64_t
#define mode_t        uint32_t /* uint16_t on xnu */
#define nfds_t        uint64_t
#define off_t         int64_t
#define pid_t         int32_t
#define register_t    int64_t
#define sa_family_t   uint16_t /* bsd:uint8_t */
#define socklen_t     uint32_t
#define speed_t       uint32_t
#define suseconds_t   int64_t /* int32_t on xnu */
#define useconds_t    uint64_t /* uint32_t on xnu */
#define syscall_arg_t int64_t /* uint64_t on xnu */
#define tcflag_t      uint32_t
#define time_t        int64_t
#define timer_t       void*
#define uid_t         uint32_t
#define rlim_t        uint64_t /* int64_t on bsd */
#define clockid_t     int32_t
#define nlink_t       uint64_t

#define TIME_T_MAX __INT64_MAX__
#define TIME_T_MIN (-TIME_T_MAX - 1)

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_WEIRDTYPES_H_ */
