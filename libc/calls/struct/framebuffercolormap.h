#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_FRAMEBUFFERCOLORMAP_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_FRAMEBUFFERCOLORMAP_H_

struct FrameBufferColorMap {
  uint32_t start;
  uint32_t len;
  uint16_t *red;
  uint16_t *green;
  uint16_t *blue;
  uint16_t *transp;
};

#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_FRAMEBUFFERCOLORMAP_H_ */
