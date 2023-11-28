#ifndef COSMOPOLITAN_LIBC_RUNTIME_MMAN_H_
#define COSMOPOLITAN_LIBC_RUNTIME_MMAN_H_
#include "libc/runtime/e820.internal.h"

/* Values for mman::pc_video_type. */
#define PC_VIDEO_TEXT     0
#define PC_VIDEO_BGR565   1
#define PC_VIDEO_BGR555   2
#define PC_VIDEO_BGRX8888 3
#define PC_VIDEO_RGBX8888 4

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct mman {
  int64_t pdp;                           /* 0x0500 */
  int32_t pdpi;                          /* 0x0508 */
  int32_t e820n;                         /* 0x050c */
  uint64_t frp;                          /* 0x0510 — free list of reclaimed,
                                                     previously used pages */
  struct SmapEntry e820[256];            /* 0x0518 */
  struct SmapEntry e820_end[0];          /* 0x1d18 */
  char pc_drive_base_table[11];          /* 0x1d18 */
  unsigned char pc_drive_type;           /* 0x1d23 */
  unsigned char pc_drive_last_sector;    /* 0x1d24 */
  unsigned short pc_drive_last_cylinder; /* 0x1d26 */
  unsigned char pc_drives_attached;      /* 0x1d28 */
  unsigned char pc_drive_last_head;      /* 0x1d29 */
  unsigned char pc_drive;                /* 0x1d2a */
  char bad_idt[2];                       /* 0x1d2b — IDTR value for an invalid
                                                     IDT; really 6 bytes which
                                                     overlap w/ fields below */
  unsigned char pc_drive_next_sector;    /* 0x1d2d */
  unsigned short pc_drive_next_cylinder; /* 0x1d2e */
  unsigned char pc_drive_next_head;      /* 0x1d30 */
  unsigned char pc_video_type;           /* 0x1d31 */
  unsigned short pc_video_stride;        /* 0x1d32 — line width, including any
                                                     invisible "pixels" — in
                                                     bytes (NOTE) */
  unsigned short pc_video_width;         /* 0x1d34 — width in chars. (text)
                                                     or pixels (graphics) */
  unsigned short pc_video_height;        /* 0x1d36 — height in chars. (text)
                                                     or pixels (graphics) */
  uint64_t pc_video_framebuffer;         /* 0x1d38 — physical address of
                                                     video frame buffer */
  uint64_t pc_video_framebuffer_size;    /* 0x1d40 */
  struct {                               /* 0x1d48 — starting cursor pos. */
    unsigned short y, x;
  } pc_video_curs_info;
  unsigned short pc_video_char_height; /* 0x1d4c — character height (useful
                                                   for setting cursor shape
                                                   in text mode) */
  uint64_t pc_acpi_rsdp;               /* 0x1d50 — pointer to ACPI RSDP;
                                                   NULL means to search for
                                                   it in legacy BIOS areas */
};

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_MMAN_H_ */
