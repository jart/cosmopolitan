#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGERESOURCEDIRECTORY_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGERESOURCEDIRECTORY_H_

struct NtImageResourceDirectory {
  uint32_t Characteristics;
  uint32_t TimeDateStamp;
  uint16_t MajorVersion;
  uint16_t MinorVersion;
  uint16_t NumberOfNamedEntries;
  uint16_t NumberOfIdEntries;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGERESOURCEDIRECTORY_H_ */
