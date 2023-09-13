#ifndef COSMOPOLITAN_LIBC_IRQ_APIC_INTERNAL_H_
#define COSMOPOLITAN_LIBC_IRQ_APIC_INTERNAL_H_

/**
 * @internal
 * @fileoverview Declarations for bare metal code to interact with I/O APICs
 *
 * @see Intel Corporation.  82093AA I/O Advanced Programmable Interrupt
 *      Controller (IOAPIC), 1996.  Intel order number 290566-001.
 */

#define IoApicRedTblLo(INTIN) (0x10 + 2 * (INTIN))
#define IoApicRedTblHi(INTIN) (0x11 + 2 * (INTIN))
#define kIoApicRedTblMasked   (1 << 16)

#if !(__ASSEMBLER__ + __LINKER__ + 0)

COSMOPOLITAN_C_START_

/**
 * @internal
 * Structure of memory-mapped registers for an I/O APIC.
 */
typedef struct {
  volatile uint32_t rIoRegSel;  /* register select (index) */
  volatile uint32_t :32, :32, :32;
  volatile uint32_t rIoWin;     /* register data */
} HwIoApic;

extern void _ApicDisableAll(void);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_IRQ_APIC_INTERNAL_H_ */
