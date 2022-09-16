#ifndef COSMOPOLITAN_LIBC_RUNTIME_MMAN_H_
#define COSMOPOLITAN_LIBC_RUNTIME_MMAN_H_
#include "libc/runtime/e820.internal.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct mman {
  int64_t pdp;                           /* 0x0500 */
  int32_t pdpi;                          /* 0x0508 */
  int32_t e820n;                         /* 0x050c */
  struct SmapEntry e820[256];            /* 0x0510 */
  struct SmapEntry e820_end[0];          /* 0x1d10 */
  char pc_drive_base_table[11];          /* 0x1d10 */
  unsigned char pc_drive_type;           /* 0x1d1b */
  unsigned char pc_drive_last_sector;    /* 0x1d1c */
  unsigned short pc_drive_last_cylinder; /* 0x1d1e */
  unsigned char pc_drives_attached;      /* 0x1d20 */
  unsigned char pc_drive_last_head;      /* 0x1d21 */
  unsigned char pc_drive;                /* 0x1d22 */
  char bad_idt[6];                       /* 0x1d23 */
};

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_MMAN_H_ */
