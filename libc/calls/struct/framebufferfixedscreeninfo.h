#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_FRAMEBUFFERFIXEDSCREENINFO_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_FRAMEBUFFERFIXEDSCREENINFO_H_

struct FrameBufferFixedScreenInfo {
  char id[16];
  uint64_t smem_start;
  uint32_t smem_len;
  uint32_t type;
  uint32_t type_aux;
  uint32_t visual;
  uint16_t xpanstep;
  uint16_t ypanstep;
  uint16_t ywrapstep;
  uint32_t line_length;
  uint64_t mmio_start;
  uint32_t mmio_len;
  uint32_t accel;
  uint16_t capabilities;
  uint16_t reserved[2];
};

#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_FRAMEBUFFERFIXEDSCREENINFO_H_ */
