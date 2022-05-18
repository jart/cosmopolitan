#ifndef COSMOPOLITAN_LIBC_CALLS_WEIRDTYPES_H_
#define COSMOPOLITAN_LIBC_CALLS_WEIRDTYPES_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

/**
 * @fileoverview Types we'd prefer hadn't been invented.
 */

#define blkcnt_t      int64_t
#define blksize_t     int64_t /* int32_t on xnu */
#define cc_t          uint8_t
#define clock_t       int64_t /* uint64_t on xnu */
#define cpu_set_t     uint64_t
#define dev_t         uint64_t /* int32_t on xnu */
#define fsblkcnt_t    int64_t
#define fsfilcnt_t    int64_t /* uint32_t on xnu */
#define gid_t         uint32_t
#define id_t          uint32_t /* int32_t on linux/freebsd/etc. */
#define in_addr_t     uint32_t
#define in_addr_t     uint32_t
#define in_port_t     uint16_t
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
#define syscall_arg_t int64_t /* uint64_t on xnu */
#define tcflag_t      uint32_t
#define time_t        int64_t
#define timer_t       void*
#define uid_t         uint32_t
#define rlim_t        uint64_t /* int64_t on bsd */

typedef __INT_FAST8_TYPE__ int_fast8_t;
typedef __UINT_FAST8_TYPE__ uint_fast8_t;
typedef __INT_FAST16_TYPE__ int_fast16_t;
typedef __UINT_FAST16_TYPE__ uint_fast16_t;
typedef __INT_FAST32_TYPE__ int_fast32_t;
typedef __UINT_FAST32_TYPE__ uint_fast32_t;
typedef __INT_FAST64_TYPE__ int_fast64_t;
typedef __UINT_FAST64_TYPE__ uint_fast64_t;

#define TIME_T_MAX      __INT64_MAX__
#define UINT_FAST64_MAX __UINT_FAST64_MAX__
#define UINT_FAST8_MAX  __UINT_FAST8_MAX__
#define INT_FAST32_MAX  __INT_FAST32_MAX__
#define INT_FAST16_MAX  __INT_FAST16_MAX__
#define UINT_FAST32_MAX __UINT_FAST32_MAX__
#define INT_FAST8_MAX   __INT_FAST8_MAX__
#define INT_FAST64_MAX  __INT_FAST64_MAX__
#define UINT_FAST16_MAX __UINT_FAST16_MAX__

#define TIME_T_MIN      (-TIME_T_MAX - 1)
#define UINT_FAST64_MIN (-UINT_FAST64_MAX - 1)
#define UINT_FAST8_MIN  (-UINT_FAST8_MAX - 1)
#define INT_FAST32_MIN  (-INT_FAST32_MAX - 1)
#define INT_FAST16_MIN  (-INT_FAST16_MAX - 1)
#define UINT_FAST32_MIN (-UINT_FAST32_MAX - 1)
#define INT_FAST8_MIN   (-INT_FAST8_MAX - 1)
#define INT_FAST64_MIN  (-INT_FAST64_MAX - 1)
#define UINT_FAST16_MIN (-UINT_FAST16_MAX - 1)

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_WEIRDTYPES_H_ */
