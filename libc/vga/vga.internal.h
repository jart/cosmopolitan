#ifndef COSMOPOLITAN_LIBC_VGA_VGA_INTERNAL_H_
#define COSMOPOLITAN_LIBC_VGA_VGA_INTERNAL_H_

/*
 * VGA_TTY_HEIGHT, VGA_TTY_WIDTH, VGA_USE_WCS, & VGA_PERSNICKETY_STATUS are
 * configuration knobs which can potentially be used to tweak the features
 * to be compiled into our VGA teletypewriter support.
 */

/**
 * Height of the video screen, in character units.  Undefine if the height
 * may vary at runtime.
 */
#define VGA_TTY_HEIGHT 25
/**
 * Width of the video screen, in character units.  Undefine if the width may
 * vary at runtime.
 */
#define VGA_TTY_WIDTH 80
/**
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
 * If VGA_PERSNICKETY_STATUS is defined, then when deciding how to return
 * status response codes (e.g. "\e[0n"), the tty code will pay attention to
 * the terminal's termios mode (TODO).  If undefined, the tty code will
 * simply return any response strings immediately, & will not echo them —
 * per the common use case.
 */
#undef VGA_PERSNICKETY_STATUS

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
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/iovec.internal.h"

COSMOPOLITAN_C_START_

struct VgaTextCharCell {
  uint8_t ch, attr;
};

struct Tty {
  unsigned short y, x;
#ifndef VGA_TTY_HEIGHT
  unsigned short yn;
#endif
#ifndef VGA_TTY_WIDTH
  unsigned short xn;
#endif
  uint32_t u8;
  uint32_t n8;
  uint32_t pr;
  uint8_t fg, bg, chr_ht;
  uint32_t conf;
  unsigned short savey, savex;
  struct VgaTextCharCell *ccs;
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
};

void _StartTty(struct Tty *, unsigned short, unsigned short,
               unsigned short, unsigned short, unsigned char,
               void *, wchar_t *);
ssize_t _TtyRead(struct Tty *, void *, size_t);
ssize_t _TtyWrite(struct Tty *, const void *, size_t);
ssize_t _TtyWriteInput(struct Tty *, const void *, size_t);
void _TtyResetOutputMode(struct Tty *);
void _TtyFullReset(struct Tty *);
void _TtyMemmove(struct Tty *, size_t, size_t, size_t);
void _TtyErase(struct Tty *, size_t, size_t);
void _TtySetY(struct Tty *, unsigned short);
void _TtySetX(struct Tty *, unsigned short);

extern struct Tty _vga_tty;

ssize_t sys_readv_vga(struct Fd *, const struct iovec *, int);
ssize_t sys_writev_vga(struct Fd *, const struct iovec *, int);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_VGA_VGA_INTERNAL_H_ */
