#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGETHUNKDATA_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGETHUNKDATA_H_

struct NtImageThunkData {
  union {
    uint64_t ForwarderString;
    uint64_t Function;
    uint64_t Ordinal;
    uint64_t AddressOfData;
  } u1;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGETHUNKDATA_H_ */
