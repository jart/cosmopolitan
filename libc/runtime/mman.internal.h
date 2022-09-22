#ifndef COSMOPOLITAN_LIBC_RUNTIME_MMAN_H_
#define COSMOPOLITAN_LIBC_RUNTIME_MMAN_H_
#include "libc/runtime/e820.internal.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct mman {
  int64_t pdp;                           /* 0x0500 */
  int32_t pdpi;                          /* 0x0508 */
  int32_t e820n;                         /* 0x050a */
  struct SmapEntry e820[256];            /* 0x0510 */
  char pc_drive_base_table[11];          /* 0x1510 */
  unsigned char pc_drive_type;           /* 0x151b */
  unsigned char pc_drive_last_sector;    /* 0x151c */
  unsigned short pc_drive_last_cylinder; /* 0x151d */
  unsigned char pc_drives_attached;      /* 0x151f */
  unsigned char pc_drive_last_head;      /* 0x1520 */
  unsigned char pc_drive;                /* 0x1521 */
  char bad_idt[6];                       /* 0x1522 */
};

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_MMAN_H_ */
