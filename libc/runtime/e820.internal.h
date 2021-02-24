#ifndef COSMOPOLITAN_LIBC_RUNTIME_E820_H_
#define COSMOPOLITAN_LIBC_RUNTIME_E820_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct SmapEntry {
  uint64_t addr;
  uint64_t size;
  enum {
    kMemoryUsable = 1,
    kMemoryUnusable = 2,
    kMemoryAcpiReclaimable = 3,
    kMemoryAcpiNvs = 4,
    kMemoryBad = 5
  } type;
  uint32_t acpi3;
};

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_E820_H_ */
