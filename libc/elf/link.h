#ifndef COSMOPOLITAN_ELF_LINK_H_
#define COSMOPOLITAN_ELF_LINK_H_
#include "libc/elf/scalar.h"
#include "libc/elf/struct/dyn.h"
#include "libc/elf/struct/phdr.h"
COSMOPOLITAN_C_START_

#define ElfW(type) Elf64_##type

struct dl_phdr_info {
  Elf64_Addr dlpi_addr;
  const char *dlpi_name;
  const Elf64_Phdr *dlpi_phdr;
  Elf64_Half dlpi_phnum;
  unsigned long long int dlpi_adds;
  unsigned long long int dlpi_subs;
  size_t dlpi_tls_modid;
  void *dlpi_tls_data;
};

struct link_map {
  Elf64_Addr l_addr;
  char *l_name;
  Elf64_Dyn *l_ld;
  struct link_map *l_next;
  struct link_map *l_prev;
};

struct r_debug {
  int r_version;
  struct link_map *r_map;
  Elf64_Addr r_brk;
  enum { RT_CONSISTENT, RT_ADD, RT_DELETE } r_state;
  Elf64_Addr r_ldbase;
};

int dl_iterate_phdr(int (*)(struct dl_phdr_info *, size_t, void *), void *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_ELF_LINK_H_ */
