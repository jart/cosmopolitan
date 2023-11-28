#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEDELAYLOADDESCRIPTOR_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEDELAYLOADDESCRIPTOR_H_

struct NtImageDelayloadDescriptor {
  union {
    uint32_t AllAttributes;
    struct {
      uint32_t RvaBased : 1;
      uint32_t ReservedAttributes : 31;
    };
  } Attributes;
  uint32_t DllNameRVA;
  uint32_t ModuleHandleRVA;
  uint32_t ImportAddressTableRVA;
  uint32_t ImportNameTableRVA;
  uint32_t BoundImportAddressTableRVA;
  uint32_t UnloadInformationTableRVA;
  uint32_t TimeDateStamp;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEDELAYLOADDESCRIPTOR_H_ */
