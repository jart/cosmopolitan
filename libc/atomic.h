#ifndef COSMOPOLITAN_LIBC_ATOMIC_H_
#define COSMOPOLITAN_LIBC_ATOMIC_H_

#define atomic_bool           _Atomic(_Bool)
#define atomic_bool32         _Atomic(__INT32_TYPE__)
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
#define atomic_char16_t       _Atomic(__CHAR16_TYPE__)
#define atomic_char32_t       _Atomic(__CHAR32_TYPE__)
#define atomic_wchar_t        _Atomic(__WCHAR_TYPE__)
#define atomic_intptr_t       _Atomic(__INTPTR_TYPE__)
#define atomic_uintptr_t      _Atomic(__UINTPTR_TYPE__)
#define atomic_size_t         _Atomic(__SIZE_TYPE__)
#define atomic_ptrdiff_t      _Atomic(__PTRDIFF_TYPE__)
#define atomic_int_fast8_t    _Atomic(__INT_FAST8_TYPE__)
#define atomic_uint_fast8_t   _Atomic(__UINT_FAST8_TYPE__)
#define atomic_int_fast16_t   _Atomic(__INT_FAST16_TYPE__)
#define atomic_uint_fast16_t  _Atomic(__UINT_FAST16_TYPE__)
#define atomic_int_fast32_t   _Atomic(__INT_FAST32_TYPE__)
#define atomic_uint_fast32_t  _Atomic(__UINT_FAST32_TYPE__)
#define atomic_int_fast64_t   _Atomic(__INT_FAST64_TYPE__)
#define atomic_uint_fast64_t  _Atomic(__UINT_FAST64_TYPE__)
#define atomic_int_least8_t   _Atomic(__INT_LEAST8_TYPE__)
#define atomic_uint_least8_t  _Atomic(__UINT_LEAST8_TYPE__)
#define atomic_int_least16_t  _Atomic(__INT_LEAST16_TYPE__)
#define atomic_uint_least16_t _Atomic(__UINT_LEAST16_TYPE__)
#define atomic_int_least32_t  _Atomic(__INT_LEAST32_TYPE__)
#define atomic_uint_least32_t _Atomic(__UINT_LEAST32_TYPE__)
#define atomic_int_least64_t  _Atomic(__INT_LEAST64_TYPE__)
#define atomic_uint_least64_t _Atomic(__UINT_LEAST64_TYPE__)

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

#endif /* COSMOPOLITAN_LIBC_ATOMIC_H_ */
