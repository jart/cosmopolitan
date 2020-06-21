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

#define int_fast8_t   __INT_FAST8_TYPE__
#define uint_fast8_t  __UINT_FAST8_TYPE__
#define int_fast16_t  __INT_FAST16_TYPE__
#define uint_fast16_t __UINT_FAST16_TYPE__
#define int_fast32_t  __INT_FAST32_TYPE__
#define uint_fast32_t __UINT_FAST32_TYPE__
#define int_fast64_t  __INT_FAST64_TYPE__
#define uint_fast64_t __UINT_FAST64_TYPE__

#define atomic_bool           _Atomic(_Bool)
#define atomic_bool32         atomic_int_fast32_t
#define atomic_char           _Atomic(char)
#define atomic_schar          _Atomic(signed char)
#define atomic_uchar          _Atomic(unsigned char)
#define atomic_short          _Atomic(short)
#define atomic_ushort         _Atomic(unsigned short)
#define atomic_int            _Atomic(int)
#define atomic_uint           _Atomic(unsigned int)
#define atomic_long           _Atomic(long)
#define atomic_ulong          _Atomic(unsigned long)
#define atomic_llong          _Atomic(long long)
#define atomic_ullong         _Atomic(unsigned long long)
#define atomic_char16_t       _Atomic(char16_t)
#define atomic_char32_t       _Atomic(char32_t)
#define atomic_wchar_t        _Atomic(wchar_t)
#define atomic_int_least8_t   _Atomic(int_least8_t)
#define atomic_uint_least8_t  _Atomic(uint_least8_t)
#define atomic_int_least16_t  _Atomic(int_least16_t)
#define atomic_uint_least16_t _Atomic(uint_least16_t)
#define atomic_int_least32_t  _Atomic(int_least32_t)
#define atomic_uint_least32_t _Atomic(uint_least32_t)
#define atomic_int_least64_t  _Atomic(int_least64_t)
#define atomic_uint_least64_t _Atomic(uint_least64_t)
#define atomic_int_fast8_t    _Atomic(int_fast8_t)
#define atomic_uint_fast8_t   _Atomic(uint_fast8_t)
#define atomic_int_fast16_t   _Atomic(int_fast16_t)
#define atomic_uint_fast16_t  _Atomic(uint_fast16_t)
#define atomic_int_fast32_t   _Atomic(int_fast32_t)
#define atomic_uint_fast32_t  _Atomic(uint_fast32_t)
#define atomic_int_fast64_t   _Atomic(int_fast64_t)
#define atomic_uint_fast64_t  _Atomic(uint_fast64_t)
#define atomic_intptr_t       _Atomic(intptr_t)
#define atomic_uintptr_t      _Atomic(uintptr_t)
#define atomic_size_t         _Atomic(size_t)
#define atomic_ptrdiff_t      _Atomic(ptrdiff_t)

#ifdef __CLANG_ATOMIC_BOOL_LOCK_FREE
#define ATOMIC_BOOL_LOCK_FREE     __CLANG_ATOMIC_BOOL_LOCK_FREE
#define ATOMIC_CHAR_LOCK_FREE     __CLANG_ATOMIC_CHAR_LOCK_FREE
#define ATOMIC_CHAR16_T_LOCK_FREE __CLANG_ATOMIC_CHAR16_T_LOCK_FREE
#define ATOMIC_CHAR32_T_LOCK_FREE __CLANG_ATOMIC_CHAR32_T_LOCK_FREE
#define ATOMIC_WCHAR_T_LOCK_FREE  __CLANG_ATOMIC_WCHAR_T_LOCK_FREE
#define ATOMIC_SHORT_LOCK_FREE    __CLANG_ATOMIC_SHORT_LOCK_FREE
#define ATOMIC_INT_LOCK_FREE      __CLANG_ATOMIC_INT_LOCK_FREE
#define ATOMIC_LONG_LOCK_FREE     __CLANG_ATOMIC_LONG_LOCK_FREE
#define ATOMIC_LLONG_LOCK_FREE    __CLANG_ATOMIC_LLONG_LOCK_FREE
#define ATOMIC_POINTER_LOCK_FREE  __CLANG_ATOMIC_POINTER_LOCK_FREE
#else
#define ATOMIC_BOOL_LOCK_FREE     __GCC_ATOMIC_BOOL_LOCK_FREE
#define ATOMIC_CHAR_LOCK_FREE     __GCC_ATOMIC_CHAR_LOCK_FREE
#define ATOMIC_CHAR16_T_LOCK_FREE __GCC_ATOMIC_CHAR16_T_LOCK_FREE
#define ATOMIC_CHAR32_T_LOCK_FREE __GCC_ATOMIC_CHAR32_T_LOCK_FREE
#define ATOMIC_WCHAR_T_LOCK_FREE  __GCC_ATOMIC_WCHAR_T_LOCK_FREE
#define ATOMIC_SHORT_LOCK_FREE    __GCC_ATOMIC_SHORT_LOCK_FREE
#define ATOMIC_INT_LOCK_FREE      __GCC_ATOMIC_INT_LOCK_FREE
#define ATOMIC_LONG_LOCK_FREE     __GCC_ATOMIC_LONG_LOCK_FREE
#define ATOMIC_LLONG_LOCK_FREE    __GCC_ATOMIC_LLONG_LOCK_FREE
#define ATOMIC_POINTER_LOCK_FREE  __GCC_ATOMIC_POINTER_LOCK_FREE
#endif

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_WEIRDTYPES_H_ */
