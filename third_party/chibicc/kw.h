#ifndef COSMOPOLITAN_THIRD_PARTY_CHIBICC_KW_H_
#define COSMOPOLITAN_THIRD_PARTY_CHIBICC_KW_H_

#define KW_STRUCT                       1  /* keyword typename */
#define KW_STATIC                       2  /* keyword typename */
#define KW_VOID                         3  /* keyword typename */
#define KW_CHAR                         4  /* keyword typename */
#define KW_UNSIGNED                     5  /* keyword typename */
#define KW_LONG                         6  /* keyword typename */
#define KW_UNION                        7  /* keyword typename */
#define KW_DOUBLE                       8  /* keyword typename */
#define KW_CONST                        9  /* keyword typename */
#define KW_FLOAT                        10 /* keyword typename */
#define KW_SHORT                        11 /* keyword typename */
#define KW_SIGNED                       12 /* keyword typename */
#define KW_ENUM                         13 /* keyword typename */
#define KW_AUTO                         14 /* keyword typename */
#define KW_REGISTER                     15 /* keyword typename */
#define KW__NORETURN                    16 /* keyword typename */
#define KW_EXTERN                       17 /* keyword typename */
#define KW_INLINE                       18 /* keyword typename */
#define KW_INT                          19 /* keyword typename */
#define KW_RESTRICT                     20 /* keyword typename */
#define KW_TYPEDEF                      21 /* keyword typename */
#define KW_TYPEOF                       22 /* keyword typename */
#define KW_VOLATILE                     23 /* keyword typename */
#define KW__ALIGNAS                     24 /* keyword typename */
#define KW__ATOMIC                      25 /* keyword typename */
#define KW__BOOL                        26 /* keyword typename */
#define KW__THREAD_LOCAL                27 /* keyword typename */
#define KW___INT128                     28 /* keyword typename */
#define KW_IF                           33 /* keyword */
#define KW_RETURN                       34 /* keyword */
#define KW_CASE                         35 /* keyword */
#define KW_ELSE                         36 /* keyword */
#define KW_FOR                          37 /* keyword */
#define KW_DO                           38 /* keyword */
#define KW_SIZEOF                       39 /* keyword */
#define KW_WHILE                        40 /* keyword */
#define KW_SWITCH                       41 /* keyword */
#define KW_BREAK                        42 /* keyword */
#define KW_CONTINUE                     43 /* keyword */
#define KW_ASM                          44 /* keyword */
#define KW_DEFAULT                      45 /* keyword */
#define KW___ATTRIBUTE__                46 /* keyword */
#define KW_GOTO                         47 /* keyword */
#define KW__ALIGNOF                     48 /* keyword */
#define KW_INCLUDE                      64
#define KW_IFDEF                        65
#define KW_IFNDEF                       66
#define KW_DEFINE                       67
#define KW_DEFINED                      68
#define KW_ELIF                         69
#define KW_ENDIF                        70
#define KW_ERROR                        71
#define KW_INCLUDE_NEXT                 72
#define KW_LINE                         73
#define KW_PRAGMA                       74
#define KW_STRCHR                       75
#define KW_STRLEN                       76
#define KW_STRPBRK                      77
#define KW_STRSTR                       78
#define KW_UNDEF                        79
#define KW__GENERIC                     80
#define KW__STATIC_ASSERT               81
#define KW___VA_OPT__                   82
#define KW___ALIGNOF__                  83
#define KW___ASM__                      84
#define KW___BUILTIN_ADD_OVERFLOW       85
#define KW___BUILTIN_ASSUME_ALIGNED     86
#define KW___BUILTIN_CONSTANT_P         87
#define KW___BUILTIN_EXPECT             88
#define KW___BUILTIN_FFS                89
#define KW___BUILTIN_FFSL               90
#define KW___BUILTIN_FFSLL              91
#define KW___BUILTIN_FPCLASSIFY         92
#define KW___BUILTIN_MUL_OVERFLOW       93
#define KW___BUILTIN_NEG_OVERFLOW       94
#define KW___BUILTIN_OFFSETOF           95
#define KW___BUILTIN_POPCOUNT           96
#define KW___BUILTIN_POPCOUNTL          97
#define KW___BUILTIN_POPCOUNTLL         98
#define KW___BUILTIN_REG_CLASS          99
#define KW___BUILTIN_STRCHR             100
#define KW___BUILTIN_STRLEN             101
#define KW___BUILTIN_STRPBRK            102
#define KW___BUILTIN_STRSTR             103
#define KW___BUILTIN_SUB_OVERFLOW       104
#define KW___BUILTIN_TYPES_COMPATIBLE_P 105
#define KW_LP                           106
#define KW_RP                           107
#define KW_LB                           108
#define KW_RB                           109
#define KW_PLUS                         110
#define KW_MINUS                        111
#define KW_AMP                          112
#define KW_STAR                         113
#define KW_EXCLAIM                      114
#define KW_TILDE                        115
#define KW_INCREMENT                    116
#define KW_DECREMENT                    117
#define KW_LOGAND                       118
#define KW_LOGOR                        119
#define KW_ARROW                        120
#define KW_DOT                          121
#define KW___ATOMIC_LOAD                122
#define KW___SYNC_LOCK_TEST_AND_SET     123
#define KW___SYNC_LOCK_RELEASE          124
#define KW___BUILTIN_IA32_PMOVMSKB128   125
#define KW___BUILTIN_IA32_MOVNTDQ       126
#define KW___ATOMIC_FETCH_ADD           127
#define KW___ATOMIC_TEST_AND_SET        128
#define KW___ATOMIC_CLEAR               129
#define KW___ATOMIC_STORE               130
#define KW___ATOMIC_STORE_N             131
#define KW___ATOMIC_LOAD_N              132
#define KW___ATOMIC_FETCH_SUB           133
#define KW___ATOMIC_FETCH_AND           134
#define KW___ATOMIC_FETCH_OR            135
#define KW___ATOMIC_FETCH_XOR           136
#define KW___ATOMIC_COMPARE_EXCHANGE_N  137
#define KW___ATOMIC_EXCHANGE_N          138

COSMOPOLITAN_C_START_

unsigned char GetKw(const char *, size_t);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_CHIBICC_KW_H_ */
