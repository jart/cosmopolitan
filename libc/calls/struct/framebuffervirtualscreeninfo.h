#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_FRAMEBUFFERVIRTUALSCREENINFO_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_FRAMEBUFFERVIRTUALSCREENINFO_H_

struct FrameBufferBitField {
  uint32_t offset;
  uint32_t length;
  uint32_t msb_right;
};

struct FrameBufferVirtualScreenInfo {
  uint32_t xres;
  uint32_t yres;
  uint32_t xres_virtual;
  uint32_t yres_virtual;
  uint32_t xoffset;
  uint32_t yoffset;
  uint32_t bits_per_pixel;
  uint32_t grayscale;
  struct FrameBufferBitField red;
  struct FrameBufferBitField green;
  struct FrameBufferBitField blue;
  struct FrameBufferBitField transp;
  uint32_t nonstd;
  uint32_t activate;
  uint32_t height;
  uint32_t width;
  uint32_t accel_flags;
  uint32_t pixclock;
  uint32_t left_margin;
  uint32_t right_margin;
  uint32_t upper_margin;
  uint32_t lower_margin;
  uint32_t hsync_len;
  uint32_t vsync_len;
  uint32_t sync;
  uint32_t vmode;
  uint32_t rotate;
  uint32_t colorspace;
  uint32_t reserved[4];
};

#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_FRAMEBUFFERVIRTUALSCREENINFO_H_ */
