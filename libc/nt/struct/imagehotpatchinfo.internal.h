#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEHOTPATCHINFO_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEHOTPATCHINFO_H_

struct NtImageHotPatchInfo {
  uint32_t Version;
  uint32_t Size;
  uint32_t SequenceNumber;
  uint32_t BaseImageList;
  uint32_t BaseImageCount;
  uint32_t BufferOffset;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEHOTPATCHINFO_H_ */
