#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_AARCH64_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_AARCH64_H_

#define FPSIMD_MAGIC 0x46508001
#define ESR_MAGIC    0x45535201
#define EXTRA_MAGIC  0x45585401
#define SVE_MAGIC    0x53564501

COSMOPOLITAN_C_START_

struct _aarch64_ctx {
  uint32_t magic;
  uint32_t size;
};

struct fpsimd_context {
  struct _aarch64_ctx head;
  uint32_t fpsr;
  uint32_t fpcr;
  uint128_t vregs[32];
};

struct esr_context {
  struct _aarch64_ctx head;
  uint64_t esr;
};

struct extra_context {
  struct _aarch64_ctx head;
  uint64_t datap;
  uint32_t size;
  uint32_t __reserved[3];
};

struct sve_context {
  struct _aarch64_ctx head;
  uint16_t vl;
  uint16_t __reserved[3];
};

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_AARCH64_H_ */
