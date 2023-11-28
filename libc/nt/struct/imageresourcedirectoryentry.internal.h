#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGERESOURCEDIRECTORYENTRY_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGERESOURCEDIRECTORYENTRY_H_

struct NtImageResourceDirectoryEntry {
  /* TODO(jart): No bitfields. */
  union {
    struct {
      uint32_t NameOffset : 31;
      uint32_t NameIsString : 1;
    };
    uint32_t Name;
    uint16_t Id;
  };
  union {
    uint32_t OffsetToData;
    struct {
      uint32_t OffsetToDirectory : 31;
      uint32_t DataIsDirectory : 1;
    };
  };
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGERESOURCEDIRECTORYENTRY_H_ */
