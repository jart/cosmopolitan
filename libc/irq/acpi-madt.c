/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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

#ifdef __x86_64__

textstartup void _AcpiMadtInit(void) {
  if (IsMetal()) {
    const AcpiTableMadt *madt;
    size_t length, num_io_apics = 0;
    const char *madt_end, *p;
    const AcpiSubtableHeader *h;
    const AcpiMadtIoApic **icp;
    uint32_t flags;
    if (!_AcpiSuccess(_AcpiGetTable("APIC", 0, (void **)&madt))) {
      KINFOF("no MADT found");
      return;
    }
    length = madt->Header.Length;
    if (length < sizeof(*madt)) {
      KDIEF("MADT has short length %#zx", length);
    }
    _AcpiMadtFlags = flags = madt->Flags;
    KINFOF("MADT @ %p, flags %#" PRIx32, madt, flags);
    madt_end = (char *)madt + length;
    p = madt->Subtable;
    while (p != madt_end) {
      h = (const AcpiSubtableHeader *)p;
      switch (h->Type) {
        case kAcpiMadtIoApic:
          ++num_io_apics;
      }
      p += h->Length;
    }
    KINFOF("MADT: %zu I/O APIC(s)", num_io_apics);
    icp = _AcpiOsAllocate(num_io_apics * sizeof(const AcpiMadtIoApic *));
    if (icp) {
      _AcpiIoApics = icp;
      p = madt->Subtable;
      while (p != madt_end) {
        h = (const AcpiSubtableHeader *)p;
        switch (h->Type) {
          case kAcpiMadtIoApic:
            *icp++ = (const AcpiMadtIoApic *)p;
        }
        p += h->Length;
      }
      _AcpiNumIoApics = num_io_apics;
    } else {
      KWARNF("MADT: no memory for I/O APICs");
    }
  }
}

#endif /* __x86_64__ */
