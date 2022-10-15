#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_FRAMEBUFFERCOLORMAP_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_FRAMEBUFFERCOLORMAP_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct FrameBufferColorMap {
  uint32_t start;
  uint32_t len;
  uint16_t *red;
  uint16_t *green;
  uint16_t *blue;
  uint16_t *transp;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_FRAMEBUFFERCOLORMAP_H_ */
