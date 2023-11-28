#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_PDHFMTCOUNTERVALUE_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_PDHFMTCOUNTERVALUE_H_
COSMOPOLITAN_C_START_

struct NtPdhFmtCountervalue {
  uint32_t CStatus;
  union {
    int32_t longValue;
    double doubleValue;
    int64_t largeValue;
    const char *AnsiStringValue;
    const char16_t *WideStringValue;
  };
};

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_PDHFMTCOUNTERVALUE_H_ */
