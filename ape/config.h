#ifndef APE_CONFIG_H_
#define APE_CONFIG_H_
#include "ape/relocations.h"
#include "libc/macros.h"

/*
 * Post-Initialization Read-Only Code Size Threshold.
 *
 * An executable needs to have at least this much code, before the
 * linker adds non-mandatory 4kb alignments. The benefit is better
 * memory protection. The tradeoff is sparser binaries.
 */
#ifndef APE_PIRO_THRESHOLD
#ifdef CONFIG_DBG
#define APE_PIRO_THRESHOLD 0x1000
#else
#define APE_PIRO_THRESHOLD 0x10000
#endif
#endif

#ifndef METAL_STDIN
#define METAL_STDIN COM1
#endif
#ifndef METAL_STDOUT
#define METAL_STDOUT COM1
#endif
#ifndef METAL_STDERR
#define METAL_STDERR COM1
#endif

#ifndef VIDYA_MODE
#define VIDYA_MODE VIDYA_MODE_CGA
#endif

/* FPU Control Word (x87) Exception Masks
   @see Intel Manual V1 §8.1.5

       IM: Invalid Operation ───────────────┐
       DM: Denormal Operand ───────────────┐│
       ZM: Zero Divide ───────────────────┐││
       OM: Overflow ─────────────────────┐│││
       UM: Underflow ───────────────────┐││││
       PM: Precision ──────────────────┐│││││
      PC: Precision Control ────────┐  ││││││
       {float,∅,double,long double} │  ││││││
      RC: Rounding Control ───────┐ │  ││││││
       {even, →-∞, →+∞, →0}       │┌┤  ││││││
                                 ┌┤││  ││││││
                                d││││rr││││││*/
#define X87_NORMAL    0b000000000001101111111
#define X87_DTOA      0b000000000001000000000
#define X87_DTOA_MASK 0b000000000001100000000
#ifndef X87_DEFAULT
#define X87_DEFAULT X87_NORMAL
#endif

#ifndef UART_BAUD_RATE
#define UART_BAUD_RATE 9600 /* bits per second ∈ [50,115200] */
#endif
#define UART_CONF_DLR (1843200 /*hz*/ / 16 /*wut*/ / (UART_BAUD_RATE))
#ifndef UART_CONF_IIR
/*                      ┌interrupt trigger level {1,4,8,14}
                        │ ┌enable 64 byte fifo (UART 16750+)
                        │ │ ┌select dma mode
                        │ │ │┌clear transmit fifo
                        │ │ ││┌clear receive fifo
                        ├┐│ │││┌enable fifos*/
#define UART_CONF_IIR 0b00000000
#endif
#ifndef UART_CONF_LCR
/*                      ┌dlab: flips configuration mode state
                        │┌enable break signal
                        ││ ┌parity {none,odd,even,high,low}
                        ││ │ ┌extra stop bit
                        ││ │ │┌data word length (bits+5)
                        ││┌┴┐│├┐*/
#define UART_CONF_LCR 0b01000011
#endif

#define XLM(VAR)                            (XLM_BASE_REAL + XLM_##VAR)
#define XLMV(VAR)                           (__xlm + XLM_##VAR)
#define XLM_BASE_REAL                       0x1000
#define XLM_E820                            0
#define XLM_E820_SIZE                       0x2000
#define XLM_BIOS_DATA_AREA                  0x2000
#define XLM_BIOS_DATA_AREA_SIZE             256
#define XLM_DRIVE_BASE_TABLE                0x2200 /* drive values are contiguous */
#define XLM_DRIVE_BASE_TABLE_SIZE           11
#define XLM_DRIVE_TYPE                      0x220b
#define XLM_DRIVE_TYPE_SIZE                 1
#define XLM_DRIVE_LAST_SECTOR               0x220c /* 1-based inclusive, e.g. 18 */
#define XLM_DRIVE_LAST_SECTOR_SIZE          1
#define XLM_DRIVE_LAST_CYLINDER             0x220d /* 0-based incl, e.g. 79 */
#define XLM_DRIVE_LAST_CYLINDER_SIZE        2
#define XLM_DRIVE_ATTACHED                  0x220f
#define XLM_DRIVE_ATTACHED_SIZE             1
#define XLM_DRIVE_LAST_HEAD                 0x2210 /* 0-based inclusive, e.g. 1 */
#define XLM_DRIVE_LAST_HEAD_SIZE            1
#define XLM_DRIVE                           0x2211
#define XLM_DRIVE_SIZE                      1
#define XLM_HAVEEXTMEMKB                    0x2212
#define XLM_HAVEEXTMEMKB_SIZE               4
#define XLM_VIDEO_POSITION_FAR_POINTER      0x2216 /* video cursor far pointer */
#define XLM_VIDEO_POSITION_FAR_POINTER_SIZE 4
#define XLM_PAGE_TABLE_STACK_POINTER        0x2220
#define XLM_PAGE_TABLE_STACK_POINTER_SIZE   8
#define XLM_BADIDT                          0x2230
#define XLM_BADIDT_SIZE                     6
#define XLM_LOADSTATE                       0x2240
#define XLM_LOADSTATE_SIZE                  8
#define XLM_SIZE                            ROUNDUP(XLM_LOADSTATE + XLM_LOADSTATE_SIZE, 0x1000)
#define IMAGE_BASE_REAL                     (XLM_BASE_REAL + XLM_SIZE)

#if !defined(__LINKER__) && !defined(__ASSEMBLER__)
extern char __xlm[XLM_SIZE];
#endif /* !defined(__LINKER__) && !defined(__ASSEMBLER__) */

#endif /* APE_CONFIG_H_ */
