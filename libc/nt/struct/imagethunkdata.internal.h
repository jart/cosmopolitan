#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGETHUNKDATA_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGETHUNKDATA_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtImageThunkData {
  union {
    uint64_t ForwarderString;
    uint64_t Function;
    uint64_t Ordinal;
    uint64_t AddressOfData;
  } u1;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGETHUNKDATA_H_ */
