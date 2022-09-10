#ifndef COSMOPOLITAN_APE_LOADER_H_
#define COSMOPOLITAN_APE_LOADER_H_

#define APE_LOADER_BASE  0x200000
#define APE_LOADER_SIZE  0x200000
#define APE_LOADER_ENTRY 0x200400
#define APE_LOADER_BSS   (PAGESIZE * 2)
#define APE_LOADER_STACK 0x7f0000000000
#define APE_BLOCK_BASE   0x7e0000000000
#define APE_BLOCK_SIZE   0x000200000000

struct ApeLoader {
  int fd;
  int os;
  char *prog;
  char *page;
  void *systemcall;
};

#endif /* COSMOPOLITAN_APE_LOADER_H_ */
