/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "ape/lib/pc.h"

#define ICW1_ICW4 0x01      /* ICW4 (not) needed */
#define ICW1_SINGLE 0x02    /* Single (cascade) mode */
#define ICW1_INTERVAL4 0x04 /* Call address interval 4 (8) */
#define ICW1_LEVEL 0x08     /* Level triggered (edge) mode */
#define ICW1_INIT 0x10      /* Initialization - required! */

#define ICW4_8086 0x01       /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO 0x02       /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE 0x08  /* Buffered mode/slave */
#define ICW4_BUF_MASTER 0x0C /* Buffered mode/master */
#define ICW4_SFNM 0x10       /* Special fully nested (not) */

static inline void io_wait(void) {
  /* Magic technique from Linux, according to:
   * wiki.osdev.org/index.php?title=Inline_Assembly/Examples&oldid=23541
   */
  outb(0x80, 0);
}

void PIC_sendEOI(unsigned char irq) {
  if (irq >= 8) outb(PIC2_CMD, PIC_EOI);
  outb(PIC1_CMD, PIC_EOI);
}

bool AreInterruptsEnabled() {
  return (eflags() & kInterruptFlag) == kInterruptFlag;
}

nodiscard forceinline unsigned long irqdisable(void) {
  unsigned long eflags;
  asm("pushf\n\t"
      "cli\n\t"
      "pop\t%0"
      : "=r"(eflags)
      : /* no inputs */
      : "cc");
  return eflags;
}

forceinline void irqrestore(unsigned long eflags) {
  asm volatile(
      "push\t%0\n\t"
      "popf"
      : /* no outputs */
      : "rm"(eflags)
      : "cc");
}

/**
 * @param offset1 is vector offset for master PIC
 *     vectors on the master become offset1..offset1+7
 * @param offset2 is same for slave PIC: offset2..offset2+7
 **/
void PIC_remap(int offset1, int offset2) {
  unsigned char a1, a2;
  a1 = inb(PIC1_DATA);  // save masks
  a2 = inb(PIC2_DATA);
  outb(PIC1_CMD,
       ICW1_INIT |
           ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
  io_wait();
  outb(PIC2_CMD, ICW1_INIT | ICW1_ICW4);
  io_wait();
  outb(PIC1_DATA, offset1);  // ICW2: Master PIC vector offset
  io_wait();
  outb(PIC2_DATA, offset2);  // ICW2: Slave PIC vector offset
  io_wait();
  outb(PIC1_DATA, 4);  // ICW3: tell Master PIC that there is a slave PIC at
                       // IRQ2 (0000 0100)
  io_wait();
  outb(PIC2_DATA, 2);  // ICW3: tell Slave PIC its cascade identity (0000 0010)
  io_wait();
  outb(PIC1_DATA, ICW4_8086);
  io_wait();
  outb(PIC2_DATA, ICW4_8086);
  io_wait();
  outb(PIC1_DATA, a1);  // restore saved masks.
  outb(PIC2_DATA, a2);
}

void IRQ_set_mask(unsigned char IRQline) {
  uint16_t port;
  uint8_t value;
  if (IRQline < 8) {
    port = PIC1_DATA;
  } else {
    port = PIC2_DATA;
    IRQline -= 8;
  }
  value = inb(port) | (1 << IRQline);
  outb(port, value);
}

void IRQ_clear_mask(unsigned char IRQline) {
  uint16_t port;
  uint8_t value;
  if (IRQline < 8) {
    port = PIC1_DATA;
  } else {
    port = PIC2_DATA;
    IRQline -= 8;
  }
  value = inb(port) & ~(1 << IRQline);
  outb(port, value);
}

static uint16_t __pic_get_irq_reg(int ocw3) {
  /* OCW3 to PIC CMD to get the register values.  PIC2 is chained, and
   * represents IRQs 8-15.  PIC1 is IRQs 0-7, with 2 being the chain */
  outb(PIC1_CMD, ocw3);
  outb(PIC2_CMD, ocw3);
  return (inb(PIC2_CMD) << 8) | inb(PIC1_CMD);
}

/* Returns the combined value of the cascaded PICs irq request register */
uint16_t pic_get_irr(void) { return __pic_get_irq_reg(PIC_READ_IRR); }

/* Returns the combined value of the cascaded PICs in-service register */
uint16_t pic_get_isr(void) { return __pic_get_irq_reg(PIC_READ_ISR); }
