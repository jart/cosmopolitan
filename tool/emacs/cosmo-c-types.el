(defconst cosmo-c-types-regex
  (let (

        ;; (kar
        ;;  '("short unsigned int"
        ;;    "int"
        ;;    "long unsigned int"
        ;;    "char"
        ;;    "long"
        ;;    "long signed int"
        ;;    "signed long"
        ;;    "unsigned short int"
        ;;    "short int"
        ;;    "signed short int"
        ;;    "unsigned"
        ;;    "long unsigned"
        ;;    "unsigned short"
        ;;    "short signed int"
        ;;    "short unsigned"
        ;;    "unsigned char"
        ;;    "signed int"
        ;;    "unsigned long"
        ;;    "long int"
        ;;    "unsigned int"
        ;;    "signed short"
        ;;    "unsigned long int"
        ;;    "short signed"
        ;;    "signed long int"
        ;;    "signed char"
        ;;    "long signed"))

        (ansi
         '("size_t"
           "wint_t"
           ;; "void"
           "wchar_t"
           ;; "long double"
           ))

        ;; (kar
        ;;  '("char"
        ;;    "short"
        ;;    "int"
        ;;    "long"
        ;;    "signed"
        ;;    "unsigned"))

        ;; (ansi
        ;;  '("size_t"
        ;;    "wint_t"
        ;;    "void"
        ;;    "wchar_t"))

        (c99
         '("bool"
           "_Bool"
           "unsigned long long int"
           "int32_t"
           "uint_least64_t"
           "long long signed"
           "intptr_t"
           "uintmax_t"
           "long long unsigned int"
           "int_fast32_t"
           "int16_t"
           "int64_t"
           "int_fast16_t"
           "int_fast64_t"
           "errno_t"
           "uint_fast32_t"
           "int_least8_t"
           "uint_least16_t"
           "long long signed int"
           "long long"
           "char16_t"
           "uint_least32_t"
           "int_least64_t"
           "int_least16_t"
           "int_fast8_t"
           "uint_least8_t"
           "uintptr_t"
           "ssize_t"
           "time_t"
           "long long int"
           "unsigned long long"
           "int8_t"
           "long long unsigned"
           "signed long long int"
           "int_least32_t"
           "uint8_t"
           "uint_fast64_t"
           "intmax_t"
           "uint_fast16_t"
           "signed long long"
           "uint32_t"
           "ptrdiff_t"
           "char32_t"
           "uint64_t"
           "uint16_t"
           "uint_fast8_t"
           "complex float"
           "complex double"
           "complex long double"))

        (c11
         '("atomic_uintptr_t"
           "atomic_uchar"
           "atomic_int_least32_t"
           "atomic_uint_least64_t"
           "atomic_int_fast32_t"
           "atomic_uint_least16_t"
           "atomic_short"
           "atomic_size_t"
           "atomic_uint"
           "atomic_char16_t"
           "atomic_ullong"
           "atomic_uint_fast16_t"
           "atomic_int_fast8_t"
           "atomic_uint_least32_t"
           "atomic_ptrdiff_t"
           "atomic_uintmax_t"
           "atomic_int_least16_t"
           "atomic_long"
           "atomic_int"
           "atomic_int_fast16_t"
           "atomic_uint_least8_t"
           "atomic_ushort"
           "atomic_int_least8_t"
           "atomic_ulong"
           "atomic_char32_t"
           "atomic_schar"
           "atomic_intmax_t"
           "atomic_int_least64_t"
           "atomic_uint_fast64_t"
           "atomic_wchar_t"
           "atomic_uint_fast8_t"
           "atomic_int_fast64_t"
           "atomic_llong"
           "atomic_bool"
           "atomic_intptr_t"
           "atomic_uint_fast32_t"
           "atomic_char"))

        (gnu
         '("__int128"
           "unsigned __int128"
           "signed __int128"
           "_Float16"
           "_Complex _Float16"
           "_Float16x"
           "_Complex _Float16x"
           "_Float32"
           "_Complex _Float32"
           "_Float32x"
           "_Complex _Float32x"
           "_Float64"
           "_Complex _Float64"
           "_Float64x"
           "_Complex _Float64x"
           "_Float128"
           "_Complex _Float128"))

        (cxx17
         '())

        (cosmo
         '("int_least128_t"
           "int_fast128_t"
           "mbedtls_mpi_sint"
           "mbedtls_mpi_uint"
           "bool32"
           "int128_t"
           "uint128_t"
           "axdx_t"))

        (linux
         '("i8"
           "u8"
           "i16"
           "u16"
           "i32"
           "u32"
           "i64"
           "u64"
           "off_t"
           "rlim_t"))

        (x86intrin
         '("__v8hu"
           "__v16qi"
           "__v4su"
           "__v8su"
           "__v16qu"
           "__v16qs"
           "__v8hi"
           "__v4hi"
           "__v2df"
           "__v2di"
           "__v4si"
           "__v8si"
           "__m1"
           "__v2du"
           "__m2"
           "__v1di"
           "__v4sf"
           "__v8sf"
           "__v2si"
           "__m64"
           "__v2sf"
           "__v8qi"
           "__v32qi"
           "__m128"
           "__m128d"
           "__m128i"
           "__m128_u"
           "__m128d_u"
           "__m128i_u"
           "__m256"
           "__m256d"
           "__m256i"
           "__m256_u"
           "__m256d_u"
           "__m256i_u"))

)
    (concat "\\_<"
            (regexp-opt (append ;; kar
                                ansi
                                c99
                                c11
                                gnu
                                cxx17
                                cosmo
                                linux
                                x86intrin))
            "\\_>")))

(provide 'cosmo-c-types)
