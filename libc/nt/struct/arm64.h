#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_ARM64_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_ARM64_H_

struct NtArm64RuntimeFunction {
  uint32_t BeginAddress;
  union {
    uint32_t UnwindData;
    struct {
      uint32_t Flag : 2;
      uint32_t FunctionLength : 11;
      uint32_t RegF : 3;
      uint32_t RegI : 4;
      uint32_t H : 1;
      uint32_t CR : 2;
      uint32_t FrameSize : 9;
    };
  };
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_ARM64_H_ */
