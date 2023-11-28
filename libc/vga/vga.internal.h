#ifndef COSMOPOLITAN_LIBC_VGA_VGA_H_
#define COSMOPOLITAN_LIBC_VGA_VGA_H_
#include "libc/runtime/mman.internal.h"

/**
 * @internal
 * Preferred width of the video screen, in character units.
 */
#define VGA_PREFER_TTY_HEIGHT 30
/**
 * @internal
 * Preferred width of the video screen, in character units.
 */
#define VGA_PREFER_TTY_WIDTH 80
/**
 * @internal
 * Assumed height of each character in pixels, in graphics modes.
 */
#define VGA_ASSUME_CHAR_HEIGHT_PX 16
/** Assumed width of each character in pixels, in graphics modes. */
#define VGA_ASSUME_CHAR_WIDTH_PX 8

/*
 * VGA_USE_WCS, VGA_USE_BLINK, & VGA_PERSNICKETY_STATUS are configuration
 * knobs which can potentially be used to tweak the features to be compiled
 * into our VGA teletypewriter support.
 */

/**
 * @internal
 * If VGA_USE_WCS is defined, the tty code can maintain an array of the
 * Unicode characters "underlying" the 8-bit (or 9-bit) characters that are
 * actually displayed on the text screen.  This Unicode character
 * information is not used in Cosmopolitan as of now (Sep 2022).  A Linux
 * mailing list post suggests that such information could be of use to
 * applications such as screen readers.
 *
 * If VGA_USE_WCS is undefined, then the Unicode character information is
 * not maintained, & we arrange for the relevant logic to be optimized away
 * by the compiler.
 *
 * @see lkml.kernel.org/lkml/204888.1529277815@turing-police.cc.vt.edu/T/
 */
#undef VGA_USE_WCS
/**
 * @internal
 * The VGA hardware can be configured — via the IBM BIOS, or via port I/O —
 * to either support blinking characters, or support the use of bright
 * background colors, but not both.  There is a hardware setting that
 * controls whether the attribute byte for each plotted character is
 * interpreted as
 *
 *           foreground────┐
 * foreground intensity──┐ │
 *           background┐ │ │
 *             blinking│ │ │
 *                   │┌┴┐│┌┴┐
 *                   76543210
 *
 * or as
 *
 *           foreground────┐
 * foreground intensity──┐ │
 *           background┐ │ │
 * background intensity│ │ │
 *                   │┌┴┐│┌┴┐
 *                   76543210
 *
 * (NOTE: QEMU 6.2.0 does not emulate the VGA blinking feature.  However,
 * VirtualBox 6.1 does.)
 *
 * If VGA_USE_BLINK is defined, our VGA code will use the former mode, &
 * will support blinking characters.  If VGA_USE_BLINK is undefined, the
 * VGA code will instead implement bright background colors.
 *
 * @see Ralf Brown's Interrupt List, int 0x10, ax = 0x1003
 *      (https://www.delorie.com/djgpp/doc/rbinter/id/22/1.html)
 */
#undef VGA_USE_BLINK
/**
 * @internal
 * If VGA_PERSNICKETY_STATUS is defined, then when deciding how to return
 * status response codes (e.g. "\e[0n"), the tty code will pay attention to
 * the terminal's termios mode (TODO).  If undefined, the tty code will
 * simply return any response strings immediately, & will not echo them —
 * per the common use case.
 */
#undef VGA_PERSNICKETY_STATUS

/**
 * @internal
 * Flags which are passed to _StartTty().
 */
#define kTtyAllocWcs                       \
  0x01 /* allocate Unicode character array \
          (if VGA_USE_WCS also defined) */
#define kTtyKlog                             \
  0x02 /* the system might be in an abnormal \
          state, & we are setting up the tty \
          to show system messages */

/**
 * @internal
 * Flags for Tty::pr.  These govern properties of individual character cells.
 */
#define kTtyFg      0x0001
#define kTtyBg      0x0002
#define kTtyBold    0x0004
#define kTtyFlip    0x0008
#define kTtyFaint   0x0010
#define kTtyUnder   0x0020
#define kTtyDunder  0x0040
#define kTtyTrue    0x0080
#define kTtyBlink   0x0100
#define kTtyItalic  0x0200
#define kTtyFraktur 0x0400
#define kTtyStrike  0x0800
#define kTtyConceal 0x1000

/**
 * @internal
 * Flags for Tty::conf.  These govern the current state of the teletypewriter
 * as a whole.
 */
#define kTtyBell        0x001
#define kTtyRedzone     0x002
#define kTtyNocursor    0x004
#define kTtyBlinkcursor 0x008
#define kTtyNocanon     0x010
#define kTtyNoecho      0x020
#define kTtyNoopost     0x040
#define kTtyLed1        0x080
#define kTtyLed2        0x100
#define kTtyLed3        0x200
#define kTtyLed4        0x400

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct VgaTextCharCell {
  uint8_t ch, attr;
};

typedef union {
  uint32_t w;
  struct {
    uint8_t b, g, r, x;
  } bgr;
} TtyBgrxColor;

typedef union {
  uint32_t w;
  struct {
    uint8_t r, g, b, x;
  } rgb;
} TtyRgbxColor;

typedef union {
  uint16_t w;
} TtyBgr565Color;

typedef union {
  uint16_t w;
} TtyBgr555Color;

typedef TtyBgrxColor TtyCanvasColor;
struct Tty;

/**
 * @internal
 * Video console object type.
 */
struct Tty {
  /**
   * Cursor position.  (y, x) = (0, 0) means the cursor is on the top left
   * character cell of the terminal.
   */
  unsigned short y, x;
  /** Height and width of terminal, in character units. */
  unsigned short yn, xn;
  /** Height and width of terminal, in pixels (if in graphics video mode). */
  unsigned short yp, xp;
  /**
   * Number of bytes (NOTE) occupied by each row of pixels, including any
   * invisible "pixels", in the video frame buffer.
   */
  unsigned short xsfb;
  /**
   * Number of bytes (NOTE) occupied by each row of pixels, including any
   * invisible "pixels", in the canvas.
   */
  unsigned short xs;
  /** Type of video buffer (from mman::pc_video_type). */
  unsigned char type;
  uint32_t u8;
  uint32_t n8;
  uint32_t pr;
  TtyCanvasColor fg, bg;
  /**
   * Height of each character in pixels.  In text modes, this is used in
   * setting the shape of the hardware cursor.
   */
  uint8_t yc;
  /**
   * Width of each character in pixels.  This is mostly useful only in
   * graphics modes.
   */
  uint8_t xc;
  uint32_t conf;
  unsigned short savey, savex;
  /** Actual video frame buffer as provided by the video card. */
  char *fb;
  /**
   * Canvas to draw into.  In text modes, this is simply the frame buffer
   * itself.  In graphics modes, this should be separate from the frame
   * buffer, & possibly allocated from main memory; we must arrange to
   * update the frame buffer from the canvas every now & then.
   *
   * During normal operation, the canvas's pixel format is given by
   * _TtyCanvasType(), & may be different from the frame buffer's.
   */
  char *canvas;
  /**
   * Which portions of the canvas have been updated & should later be drawn
   * to screen with Tty::update().
   */
  unsigned short updy1, updx1, updy2, updx2;
#ifdef VGA_USE_WCS
  wchar_t *wcs;
#endif
  wchar_t *xlat;
  enum TtyState {
    kTtyAscii,
    kTtyUtf8,
    kTtyEsc,
    kTtyCsi,
  } state;
  struct TtyEsc {
    unsigned i;
    char s[64];
  } esc;
  struct TtyInput {
    size_t i;
    char p[256];
  } input;
  void (*update)(struct Tty *);
  void (*drawchar)(struct Tty *, size_t, size_t, wchar_t);
  void (*eraselinecells)(struct Tty *, size_t, size_t, size_t);
  void (*movelinecells)(struct Tty *, size_t, size_t, size_t, size_t, size_t);
};

forceinline unsigned char _TtyCanvasType(struct Tty *tty) {
  return tty->type == PC_VIDEO_TEXT ? PC_VIDEO_TEXT : PC_VIDEO_BGRX8888;
}

forceinline unsigned short _TtyGetY(struct Tty *tty) {
  return tty->y;
}

forceinline unsigned short _TtyGetX(struct Tty *tty) {
  return tty->x;
}

/* Routines that implement normal graphical console output. */
void _TtyBgrxUpdate(struct Tty *);
void _TtyRgbxUpdate(struct Tty *);
void _TtyBgr565Update(struct Tty *);
void _TtyBgr555Update(struct Tty *);
void _TtyGraphDrawChar(struct Tty *, size_t, size_t, wchar_t);
void _TtyGraphEraseLineCells(struct Tty *, size_t, size_t, size_t);
void _TtyGraphMoveLineCells(struct Tty *, size_t, size_t, size_t, size_t,
                            size_t);

/*
 * Routines that implement emergency console output in graphical video modes.
 */
void _TtyKlog16Update(struct Tty *);
void _TtyKlog16DrawChar(struct Tty *, size_t, size_t, wchar_t);
void _TtyKlog16EraseLineCells(struct Tty *, size_t, size_t, size_t);
void _TtyKlog16MoveLineCells(struct Tty *, size_t, size_t, size_t, size_t,
                             size_t);
void _TtyKlog32Update(struct Tty *);
void _TtyKlog32DrawChar(struct Tty *, size_t, size_t, wchar_t);
void _TtyKlog32EraseLineCells(struct Tty *, size_t, size_t, size_t);
void _TtyKlog32MoveLineCells(struct Tty *, size_t, size_t, size_t, size_t,
                             size_t);

/* High-level teletypewriter routines. */
void _StartTty(struct Tty *, unsigned char, unsigned short, unsigned short,
               unsigned short, unsigned short, unsigned short, unsigned char,
               unsigned char, void *, unsigned);
ssize_t _TtyRead(struct Tty *, void *, size_t);
ssize_t _TtyWrite(struct Tty *, const void *, size_t);
ssize_t _TtyWriteInput(struct Tty *, const void *, size_t);
void _TtyResetOutputMode(struct Tty *);
void _TtyFullReset(struct Tty *);
void _TtyMoveLineCells(struct Tty *, size_t, size_t, size_t, size_t, size_t);
void _TtyMoveLines(struct Tty *, size_t, size_t, size_t);
void _TtyEraseLineCells(struct Tty *, size_t, size_t, size_t);
void _TtyEraseLines(struct Tty *, size_t, size_t);
void _TtySetY(struct Tty *, unsigned short);
void _TtySetX(struct Tty *, unsigned short);

extern const uint8_t _vga_font_default_direct[95][13];
extern struct Tty _vga_tty;

void _vga_reinit(struct Tty *, unsigned short, unsigned short, unsigned);
void _klog_vga(const char *, size_t);

struct Fd;
struct iovec;
ssize_t sys_readv_vga(struct Fd *, const struct iovec *, int);
ssize_t sys_writev_vga(struct Fd *, const struct iovec *, int);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_VGA_VGA_H_ */
