#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_VIDYA_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_VIDYA_H_

#define VIDYA_ROWS              25
#define VIDYA_COLUMNS           80
#define VIDYA_SIZE              (VIDYA_ROWS * VIDYA_COLUMNS * 2)
#define VIDYA_MODE_MDA          7
#define VIDYA_MODE_CGA          3
#define VIDYA_ADDR_MDA          0xb0000
#define VIDYA_ADDR_CGA          0xb8000
#define VIDYA_ATTR_NORMAL       0x07    /* cozy default for both mda and cga */
#define VIDYA_REWIND            ~0x7fff /* derived from mode addr min. lzcnt */
#define VIDYA_SERVICE           0x10
#define VIDYA_SET_MODE          0x0003
#define VIDYA_SET_CURSOR        0x0100
#define VIDYA_SET_CURSOR_NONE   0x2000
#define VIDYA_SET_BLINKING      0x1003
#define VIDYA_SET_BLINKING_NONE 0x0000

#if !(__ASSEMBLER__ + __LINKER__ + 0)

enum VidyaMode {
  kVidyaModeMda = VIDYA_MODE_MDA,
  kVidyaModeCga = VIDYA_MODE_CGA
};

enum VidyaColor {
  kVidyaColorBlack = 0x0,
  kVidyaColorBlue = 0x1,
  kVidyaColorGreen = 0x2,
  kVidyaColorCyan = 0x3,
  kVidyaColorRed = 0x4,
  kVidyaColorMagenta = 0x5,
  kVidyaColorBrown = 0x6,
  kVidyaColorLightGray = 0x7,
  kVidyaColorDarkGray = 0x8,
  kVidyaColorLightBlue = 0x9,
  kVidyaColorLightGreen = 0xa,
  kVidyaColorLightCyan = 0xb,
  kVidyaColorLightRed = 0xc,
  kVidyaColorLightMagenta = 0xd,
  kVidyaColorYellow = 0xe,
  kVidyaColorWhite = 0xf
};

struct thatispacked VidyaCell {
  unsigned glyph : 8; /* IBM Code Page 437 */
  union VidyaAttr {
    enum {
      kVidyaAttrBlank = 0x00,
      kVidyaAttrNormal = VIDYA_ATTR_NORMAL,
      kVidyaAttrMdaFlipped = 0x70,
      kVidyaAttrMdaFlippedFaded = 0x78,
      kVidyaAttrMdaFlippedIntense = 0xf0,
      kVidyaAttrMdaFlippedFadedIntense = 0xf8
    } preset : 8;
    struct VidyaTextDecoration { /* MDA Only */
      unsigned underline : 1;
      unsigned __ignore1 : 1;
      unsigned bold : 1;
      unsigned __ignore2 : 3;
      unsigned intense : 1;
    } decoration;
    struct { /* CGA Only */
      enum VidyaColor fg : 4;
      enum VidyaColor bg : 4;
    } color;
  } attr;
};

typedef union VidyaAttr VidyaAttr;
typedef struct VidyaCell VidyaCell;
typedef struct VidyaCell VidyaPage[VIDYA_ROWS][VIDYA_COLUMNS];

__far VidyaPage *vinit(enum VidyaMode mode);
__far VidyaPage *vcls(__far VidyaCell *pos);
__far VidyaCell *vputc(__far VidyaCell *pos, int c);
__far VidyaCell *vputs(__far VidyaCell *pos, const char *str);
__far VidyaCell *vtput(__far VidyaCell *pos, const void *data, size_t size);
__far VidyaCell *vscroll(__far VidyaCell *pos, size_t bytes);

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_VIDYA_H_ */
