#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_MEMEXTENDEDPARAMETER_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_MEMEXTENDEDPARAMETER_H_

#define kNtMemExtendedParameterTypeBits 8

#define kNtMemExtendedParameterInvalidType         0
#define kNtMemExtendedParameterAddressRequirements 1
#define kNtMemExtendedParameterNumaNode            2
#define kNtMemExtendedParameterPartitionHandle     3
#define kNtMemExtendedParameterUserPhysicalHandle  4
#define kNtMemExtendedParameterAttributeFlags      5
#define kNtMemExtendedParameterImageMachine        6
#define kNtMemExtendedParameterMax                 7

#define kNtMemExtendedParameterGraphics          0x00000001
#define kNtMemExtendedParameterNonpaged          0x00000002
#define kNtMemExtendedParameterZeroPagesOptional 0x00000004
#define kNtMemExtendedParameterNonpagedLarge     0x00000008
#define kNtMemExtendedParameterNonpagedHuge      0x00000010
#define kNtMemExtendedParameterSoftFaultPages    0x00000020
#define kNtMemExtendedParameterEcCode            0x00000040
#define kNtMemExtendedParameterImageNoHpat       0x00000080

struct NtMemExtendedParameter {
  uint8_t Type;
  uint8_t Reserved[7];
  union {
    uint64_t ULong64;
    void *Pointer;
    size_t Size;
    intptr_t Handle;
    unsigned ULong;
  };
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_MEMEXTENDEDPARAMETER_H_ */
