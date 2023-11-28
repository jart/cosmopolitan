#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_FILECOMPRESSIONINFO_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_FILECOMPRESSIONINFO_H_

struct NtFileCompressionInfo {
  int64_t CompressedFileSize;
  uint16_t CompressionFormat;
  uint8_t CompressionUnitShift;
  uint8_t ChunkShift;
  uint8_t ClusterShift;
  uint8_t Reserved[3];
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_FILECOMPRESSIONINFO_H_ */
