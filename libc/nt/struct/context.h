#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_CONTEXT_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_CONTEXT_H_

struct NtM128A {
  uint64_t Low;
  int64_t High;
};

struct NtXmmSaveArea32 { /* basically same as struct FpuState */
  uint16_t ControlWord;
  uint16_t StatusWord;
  uint8_t TagWord;
  uint8_t Reserved1;
  uint16_t ErrorOpcode;
  uint32_t ErrorOffset;
  uint16_t ErrorSelector;
  uint16_t Reserved2;
  uint32_t DataOffset;
  uint16_t DataSelector;
  uint16_t Reserved3;
  uint32_t MxCsr;
  uint32_t MxCsr_Mask;
  struct NtM128A FloatRegisters[8];
  struct NtM128A XmmRegisters[16];
  uint8_t Reserved4[96];
};

struct NtContext {
  uint64_t P1Home, P2Home, P3Home, P4Home, P5Home, P6Home;
  uint32_t ContextFlags;
  uint32_t MxCsr;
  uint16_t SegCs, SegDs, SegEs, SegFs, SegGs, SegSs;
  uint32_t EFlags;
  uint64_t Dr0, Dr1, Dr2, Dr3, Dr6, Dr7;
  uint64_t Rax, Rcx, Rdx, Rbx, Rsp, Rbp, Rsi, Rdi;
  uint64_t R8, R9, R10, R11, R12, R13, R14, R15;
  uint64_t Rip;
  union {
    struct NtXmmSaveArea32 FltSave;
    struct NtXmmSaveArea32 FloatSave;
    struct {
      struct NtM128A Header[2];
      struct NtM128A Legacy[8];
      struct NtM128A Xmm0, Xmm1, Xmm2, Xmm3, Xmm4, Xmm5, Xmm6, Xmm7;
      struct NtM128A Xmm8, Xmm9, Xmm10, Xmm11, Xmm12, Xmm13, Xmm14, Xmm15;
    };
  };
  struct NtM128A VectorRegister[26];
  uint64_t VectorControl;
  uint64_t DebugControl;
  uint64_t LastBranchToRip;
  uint64_t LastBranchFromRip;
  uint64_t LastExceptionToRip;
  uint64_t LastExceptionFromRip;
} forcealign(16);

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_CONTEXT_H_ */
