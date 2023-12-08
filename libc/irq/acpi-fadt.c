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
#include "libc/intrin/kprintf.h"
#include "libc/irq/acpi.internal.h"

#ifdef __x86_64__

textstartup static void _AcpiDsdtInit(uintptr_t dsdt_phy) {
  const AcpiTableDsdt *dsdt;
  size_t length;
  if (!dsdt_phy) {
    KWARNF("FADT: no DSDT");
    return;
  }
  dsdt = _AcpiMapTable(dsdt_phy);
  KINFOF("FADT: DSDT @ %p", dsdt);
  length = dsdt->Header.Length;
  if (length <= offsetof(AcpiTableDsdt, Aml)) {
    KWARNF("DSDT: no AML?");
    return;
  }
  /* TODO: parse AML to discover hardware configuration */
}

textstartup void _AcpiFadtInit(void) {
  if (IsMetal()) {
    const AcpiTableFadt *fadt;
    size_t length;
    uint16_t flags;
    uintptr_t dsdt_phy = 0;
    if (!_AcpiSuccess(_AcpiGetTable("FACP", 0, (void **)&fadt))) {
      KINFOF("no FADT found");
      return;
    }
    length = fadt->Header.Length;
    KINFOF("FADT @ %p,+%#zx", fadt, length);
    _Static_assert(offsetof(AcpiTableFadt, Dsdt) == 40);
    _Static_assert(offsetof(AcpiTableFadt, BootFlags) == 109);
    _Static_assert(offsetof(AcpiTableFadt, XDsdt) == 140);
    if (length >= offsetof(AcpiTableFadt, BootFlags) + sizeof(fadt->BootFlags))
    {
      _AcpiBootFlags = flags = fadt->BootFlags;
      KINFOF("FADT: boot flags %#x", (unsigned)flags);
    }
    if (length >= offsetof(AcpiTableFadt, XDsdt) + sizeof(fadt->XDsdt) &&
        fadt->XDsdt) {
      dsdt_phy = fadt->XDsdt;
    } else if (length >= offsetof(AcpiTableFadt, Dsdt) + sizeof(fadt->Dsdt)) {
      dsdt_phy = fadt->Dsdt;
    }
    _AcpiDsdtInit(dsdt_phy);
  }
}

#endif /* __x86_64__ */
