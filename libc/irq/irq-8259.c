/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ This is free and unencumbered software released into the public domain.      │
│                                                                              │
│ Anyone is free to copy, modify, publish, use, compile, sell, or              │
│ distribute this software, either in source code form or as a compiled        │
│ binary, for any purpose, commercial or non-commercial, and by any            │
│ means.                                                                       │
│                                                                              │
│ In jurisdictions that recognize copyright laws, the author or authors        │
│ of this software dedicate any and all copyright interest in the              │
│ software to the public domain. We make this dedication for the benefit       │
│ of the public at large and to the detriment of our heirs and                 │
│ successors. We intend this dedication to be an overt act of                  │
│ relinquishment in perpetuity of all present and future rights to this        │
│ software under copyright law.                                                │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,              │
│ EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF           │
│ MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.       │
│ IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR            │
│ OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        │
│ ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR        │
│ OTHER DEALINGS IN THE SOFTWARE.                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/dce.h"
#include "libc/intrin/weaken.h"
#include "libc/inttypes.h"
#include "libc/irq/acpi.internal.h"
#include "libc/irq/apic.internal.h"
#include "libc/irq/irq.internal.h"
#include "libc/runtime/pc.internal.h"

#ifdef __x86_64__

textstartup void _Irq8259Init(void) {
  if (_weaken(_ApicDisableAll)) _weaken(_ApicDisableAll)();
  ACPI_INFO("starting 8259 IRQs");
  outb(PIC1_CMD, PIC_INIT | PIC_IC4);  /* ICW1 */
  outb(PIC2_CMD, PIC_INIT | PIC_IC4);
  outb(PIC1_DATA, IRQ0);               /* ICW2 */
  outb(PIC2_DATA, IRQ8);
  outb(PIC1_DATA, 1 << 2);             /* ICW3 — connect master IRQ 2 to */
  outb(PIC2_DATA, 1 << 1);             /* (slave) IRQ 9 */
  outb(PIC1_DATA, PIC_UPM);            /* ICW4 */
  outb(PIC2_DATA, PIC_UPM);
  /* Set IRQ masks. */
  outb(PIC1_DATA, ~(1 << 0));          /* OCW1 — allow IRQ 0 on PIC 1 */
  outb(PIC2_DATA, ~0);
  /* Send EOIs for good measure. */
  outb(PIC1_CMD, PIC_EOI);
  outb(PIC2_CMD, PIC_EOI);
  enable();
}

textstartup void _IrqHwInit(void) {
  if (!_weaken(_AcpiMadtFlags) ||
      (_AcpiMadtFlags & kAcpiMadtPcAtCompat) != 0) {
    _Irq8259Init();
  } else {
    /* TODO */
    ACPI_FATAL("machine does not support 8259 IRQ handling");
  }
}

#endif /* __x86_64__ */
