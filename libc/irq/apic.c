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
#include "libc/inttypes.h"
#include "libc/irq/acpi.internal.h"
#include "libc/irq/apic.internal.h"

#ifdef __x86_64__

/**
 * @internal
 * Disables all I/O APIC interrupts.
 */
textstartup void _ApicDisableAll(void) {
  const AcpiMadtIoApic **icp = _AcpiIoApics, *ic;
  HwIoApic *hwic;
  size_t n = _AcpiNumIoApics;
  unsigned intin;
  uint32_t conf;
  while (n-- != 0) {
    ic = *icp++;
    hwic = _AcpiOsMapUncachedMemory((uintptr_t)ic->Address, sizeof(HwIoApic));
    ACPI_INFO("stopping I/O APIC @ %p", hwic);
    for (intin = 0; intin < 24; ++intin) {
      hwic->rIoRegSel = IoApicRedTblLo(intin);
      conf = hwic->rIoWin;
      hwic->rIoWin = conf | kIoApicRedTblMasked;
    }
  }
}

#endif /* __x86_64__ */
