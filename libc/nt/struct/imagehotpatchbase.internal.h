#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEHOTPATCHBASE_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEHOTPATCHBASE_H_

struct NtImageHotPatchBase {
  uint32_t SequenceNumber;
  uint32_t Flags;
  uint32_t OriginalTimeDateStamp;
  uint32_t OriginalCheckSum;
  uint32_t CodeIntegrityInfo;
  uint32_t CodeIntegritySize;
  uint32_t PatchTable;
  uint32_t BufferOffset;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEHOTPATCHBASE_H_ */
