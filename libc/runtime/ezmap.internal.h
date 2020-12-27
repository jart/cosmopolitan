#ifndef COSMOPOLITAN_LIBC_RUNTIME_EZMAP_INTERNAL_H_
#define COSMOPOLITAN_LIBC_RUNTIME_EZMAP_INTERNAL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct MappedFile {
  int fd;
  void *addr;
  size_t size;
};

int MapFileRead(const char *, struct MappedFile *) hidden;
int UnmapFile(struct MappedFile *) hidden;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_EZMAP_INTERNAL_H_ */
