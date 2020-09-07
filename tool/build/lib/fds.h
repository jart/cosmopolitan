#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_FDS_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_FDS_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct MachineFds {
  size_t i, n;
  struct MachineFd {
    int fd;
    struct MachineFdCb {
      int (*close)(int);
      ssize_t (*read)(int, void *, size_t);
      ssize_t (*write)(int, const void *, size_t);
      int (*ioctl)(int, uint64_t, void *);
    } * cb;
  } * p;
  struct MachineFdClosed {
    unsigned fd;
    struct MachineFdClosed *next;
  } * closed;
};

int MachineFdAdd(struct MachineFds *);
void MachineFdRemove(struct MachineFds *, int);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_FDS_H_ */
