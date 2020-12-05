#ifndef COSMOPOLITAN_LIBC_UBSAN_H_
#define COSMOPOLITAN_LIBC_UBSAN_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § runtime » behavior enforcement                            ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

struct UbsanSourceLocation {
  const char *file;
  uint32_t line;
  uint32_t column;
};

struct UbsanTypeDescriptor {
  uint16_t kind;
  uint16_t info;
  char name[];
};

struct UbsanTypeMismatchInfo {
  struct UbsanSourceLocation location;
  struct UbsanTypeDescriptor *type;
  uintptr_t alignment;
  uint8_t type_check_kind;
};

struct UbsanTypeMismatchInfoClang {
  struct UbsanSourceLocation location;
  struct UbsanTypeDescriptor *type;
  unsigned char log_alignment; /* https://reviews.llvm.org/D28244 */
  uint8_t type_check_kind;
};

struct UbsanOutOfBoundsInfo {
  struct UbsanSourceLocation location;
  struct UbsanTypeDescriptor *array_type;
  struct UbsanTypeDescriptor *index_type;
};

struct UbsanUnreachableData {
  struct UbsanSourceLocation location;
};

struct UbsanVlaBoundData {
  struct UbsanSourceLocation location;
  struct UbsanTypeDescriptor *type;
};

struct UbsanNonnullArgData {
  struct UbsanSourceLocation location;
  struct UbsanSourceLocation attr_location;
};

struct UbsanCfiBadIcallData {
  struct UbsanSourceLocation location;
  struct UbsanTypeDescriptor *type;
};

struct UbsanNonnullReturnData {
  struct UbsanSourceLocation location;
  struct UbsanSourceLocation attr_location;
};

struct UbsanFunctionTypeMismatchData {
  struct UbsanSourceLocation location;
  struct UbsanTypeDescriptor *type;
};

struct UbsanInvalidValueData {
  struct UbsanSourceLocation location;
  struct UbsanTypeDescriptor *type;
};

struct UbsanOverflowData {
  struct UbsanSourceLocation location;
  struct UbsanTypeDescriptor *type;
};

struct UbsanFloatCastOverflowData {
#if __GNUC__ + 0 >= 6
  struct UbsanSourceLocation location;
#endif
  struct UbsanTypeDescriptor *from_type;
  struct UbsanTypeDescriptor *to_type;
};

struct UbsanOutOfBoundsData {
  struct UbsanSourceLocation location;
  struct UbsanTypeDescriptor *arraytype;
  struct UbsanTypeDescriptor *index_type;
};

struct UbsanShiftOutOfBoundsData {
  struct UbsanSourceLocation location;
  struct UbsanTypeDescriptor *lhs_type;
  struct UbsanTypeDescriptor *rhs_type;
};

void __ubsan_abort(const struct UbsanSourceLocation *,
                   const char *) relegated hidden wontreturn;
void __ubsan_handle_type_mismatch(struct UbsanTypeMismatchInfo *,
                                  uintptr_t) relegated hidden wontreturn;
void ___ubsan_handle_type_mismatch_v1(struct UbsanTypeMismatchInfoClang *,
                                      uintptr_t) relegated hidden wontreturn;
void __ubsan_handle_float_cast_overflow(void *,
                                        void *) relegated hidden wontreturn;

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_UBSAN_H_ */
