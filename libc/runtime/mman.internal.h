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
  unsigned char pc_video_type;           /* 0x1d29 */
  unsigned short pc_video_stride;        /* 0x1d2a — line width, including any
                                                     invisible "pixels" — in
                                                     bytes (NOTE) */
  unsigned short pc_video_width;         /* 0x1d2c — width in chars. (text)
                                                     or pixels (graphics) */
  unsigned short pc_video_height;        /* 0x1d2e — height in chars. (text)
                                                     or pixels (graphics) */
  uint64_t pc_video_framebuffer;         /* 0x1d30 — physical address of
                                                     video frame buffer */
  uint64_t pc_video_framebuffer_size;    /* 0x1d38 */
};

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

/* Values for mman::pc_video_type.  TODO: implement graphics modes. */
#define PC_VIDEO_TEXT     0
#define PC_VIDEO_BGR565   1
#define PC_VIDEO_BGR555   2
#define PC_VIDEO_BGRX8888 3
#define PC_VIDEO_RGBX8888 4

#endif /* COSMOPOLITAN_LIBC_RUNTIME_MMAN_H_ */
