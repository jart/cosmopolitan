(defconst cosmo-platform-constants-regex
  (let (

        (gcc412
         '("__cplusplus"
           "__OBJC__"
           "__STRICT_ANSI__"
           "__ELF__"
           "__VERSION__"
           "__OPTIMIZE__"
           "__OPTIMIZE_SIZE__"
           "__NO_INLINE__"
           "__CHAR_UNSIGNED__"
           "__WCHAR_UNSIGNED__"
           "__REGISTER_PREFIX__"
           "__USER_LABEL_PREFIX__"
           "__SIZE_TYPE__"
           "__PTRDIFF_TYPE__"
           "__WCHAR_TYPE__"
           "__CHAR16_TYPE__"
           "__CHAR32_TYPE__"
           "__WINT_TYPE__"
           "__INTMAX_TYPE__"
           "__DEPRECATED"
           "__EXCEPTIONS"
           "__USING_SJLJ_EXCEPTIONS__"
           "__GXX_WEAK__"
           "__NEXT_RUNTIME__"
           "__LP64__"
           "__SSP__"
           "__SSP_ALL__"
           "__unix__"
           "__vax__"
           "__ns16000__"
           "__pic__"
           "__pie__"
           "__PIC__"
           "__PIE__"
           "__ASSEMBLER__"
           "__code_model_large__"
           "__code_model_medium__"))

        (cpp92
         '("__SEG_FS"
           "__SEG_GS"
           "__BYTE_ORDER__"
           "__ORDER_LITTLE_ENDIAN__"
           "__ORDER_BIG_ENDIAN__"
           "__ORDER_PDP_ENDIAN__"
           "__FLOAT_WORD_ORDER__"
           "__GFORTRAN__"
           "__GCC_ASM_FLAG_OUTPUTS__"
           "__GNUC_GNU_INLINE__"
           "__GNUC_STDC_INLINE__"
           "__GXX_RTTI"
           "__GXX_EXPERIMENTAL_CXX0X__"
           "__SSP_STRONG__"
           "__SSP_EXPLICIT__"
           "__SANITIZE_ADDRESS__"
           "__SANITIZE_THREAD__"
           "__SANITIZE_MEMORY__"
           "__GCC_HAVE_SYNC_COMPARE_AND_SWAP_1"
           "__GCC_HAVE_SYNC_COMPARE_AND_SWAP_2"
           "__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4"
           "__GCC_HAVE_SYNC_COMPARE_AND_SWAP_8"
           "__GCC_HAVE_SYNC_COMPARE_AND_SWAP_16"
           "__HAVE_SPECULATION_SAFE_VALUE"
           "__GCC_HAVE_DWARF2_CFI_ASM"
           "__STDC_WANT_IEC_60559_TYPES_EXT__"
           "__GCC_IEC_559"
           "__SUPPORT_SNAN__"
           "__GCC_IEC_559_COMPLEX"
           "__NO_MATH_ERRNO__"
           "__gnu__"
           "_OPENMP"))

        (cosmo
         '("__LINKER__"))

        )
    (concat "\\_<"
            (regexp-opt (append cpp92 gcc412 cosmo))
            "\\_>")))

(provide 'cosmo-platform-constants)
