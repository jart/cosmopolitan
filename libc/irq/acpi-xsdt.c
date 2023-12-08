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
#include "libc/intrin/atomic.h"
#include "libc/serialize.h"
#include "libc/intrin/directmap.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/irq/acpi.internal.h"
#include "libc/log/color.internal.h"
#include "libc/macros.internal.h"
#include "libc/nt/efi.h"
#include "libc/runtime/pc.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"

#ifdef __x86_64__

textstartup static void *_AcpiOsMapRoMemory(uintptr_t phy, size_t n) {
  __invert_and_perm_ref_memory_area(__get_mm(), __get_pml4t(), phy, n, PAGE_XD);
  return (void *)(BANE + phy);
}

textstartup static void *_AcpiOsMapMemory(uintptr_t phy, size_t n) {
  __invert_and_perm_ref_memory_area(__get_mm(), __get_pml4t(), phy, n,
                                    PAGE_XD | PAGE_RW);
  return (void *)(BANE + phy);
}

textstartup void *_AcpiOsMapUncachedMemory(uintptr_t phy, size_t n) {
  __invert_and_perm_ref_memory_area(__get_mm(), __get_pml4t(), phy, n,
                                    PAGE_XD | PAGE_PCD | PAGE_RW);
  return (void *)(BANE + phy);
}

textstartup static void *_AcpiOsAllocatePages(size_t n) {
  struct DirectMap dm = sys_mmap_metal(NULL, n, PROT_READ | PROT_WRITE,
                                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  void *addr = dm.addr;
  if (addr == (void *)-1) addr = NULL;
  return addr;
}

textstartup void *_AcpiOsAllocate(size_t n) {
  static _Atomic(char *) slack = NULL;
  char *addr = NULL;
  size_t align = __BIGGEST_ALIGNMENT__, use;
  if (n >= 4096) return _AcpiOsAllocatePages(n);
  n = ROUNDUP(n, align);
  for (;;) {
    addr = atomic_exchange(&slack, NULL);
    if (!addr) {
      addr = _AcpiOsAllocatePages(4096);
      if (!addr) return NULL;
    }
    use = (uintptr_t)addr % 4096 + n;
    if (use <= 4096) {
      if (use < 4096) atomic_store(&slack, addr + n);
      return addr;
    }
  }
}

textstartup static uint8_t _AcpiTbChecksum(const uint8_t *p, size_t n) {
  uint8_t c = 0;
  while (n-- != 0) c += *p++;
  return c;
}

textstartup static AcpiStatus _AcpiTbVerifyChecksum(const uint8_t *p,
                                                    size_t n) {
  uint8_t sum = _AcpiTbChecksum(p, n);
  if (!sum) return kAcpiOk;
  KWARNF("bad ACPI table cksum %#x != 0 @ %p,+%#zx", (unsigned)sum, p, n);
  return kAcpiExBadChecksum;
}

textstartup static AcpiStatus _AcpiRsdpVerifyChecksums(const uint8_t *p) {
  const AcpiTableRsdp *q = (const AcpiTableRsdp *)p;
  size_t length = offsetof(AcpiTableRsdp, Length);
  AcpiStatus sta = _AcpiTbVerifyChecksum(p, length);
  if (!_AcpiSuccess(sta)) return sta;
  if (q->Revision <= 1) return kAcpiOk;
  length = q->Length;
  if (length < offsetof(AcpiTableRsdp, Reserved)) {
    KWARNF("malformed ACPI 2+ RSDP, length %#zx < %#zx", length,
           offsetof(AcpiTableRsdp, Reserved));
    if (length < offsetof(AcpiTableRsdp, RsdtPhysicalAddress) +
                     sizeof(q->RsdtPhysicalAddress)) {
      return kAcpiExBadHeader;
    }
    return kAcpiOk;
  }
  return _AcpiTbVerifyChecksum(p, length);
}

textstartup static bool _AcpiTbIsValidRsdp(const uint8_t *p) {
  const AcpiTableRsdp *q = (const AcpiTableRsdp *)p;
  if (READ64LE(q->Signature) != READ64LE("RSD PTR ")) return false;
  KINFOF("\"RSD PTR \" @ %p, ACPI rev %u", q, (unsigned)q->Revision);
  if (!_AcpiSuccess(_AcpiRsdpVerifyChecksums(p))) return false;
  return true;
}

textstartup static const AcpiTableRsdp *_AcpiFindRsdp(void) {
  const size_t PARA_SZ = 0x10;
  size_t off;
  struct mman *mm = __get_mm();
  uint64_t rsdp_phy = mm->pc_acpi_rsdp;
  uint16_t ebda_para;
  const uint8_t *area;
  if (rsdp_phy) return _AcpiOsMapRoMemory(rsdp_phy, sizeof(AcpiTableRsdp));
  /*
   * "OSPM finds the Root System Description Pointer (RSDP) structure by
   *  searching physical memory ranges on 16-byte boundaries for a valid
   *  Root System Description Pointer structure signature and checksum match
   *  as follows:
   *    * The first 1 KB of the Extended BIOS Data Area (EBDA). For EISA or
   *      MCA systems, the EBDA can be found in the two-byte location 40:0Eh
   *      on the BIOS data area.
   *    * The BIOS read-only memory space between 0E0000h and 0FFFFFh."
   * — Advanced Configuration and Power Interface (ACPI) Specification,
   *   Release 6.5, §5.2.5.1
   */
  ebda_para = *(const uint16_t *)(BANE + PC_BIOS_DATA_AREA + 0xE);
  if (ebda_para) {
    area = _AcpiOsMapMemory((uint32_t)ebda_para * PARA_SZ, 1024);
    KINFOF("search EBDA @ %p for RSDP", area);
    off = 1024;
    while (off != 0) {
      off -= PARA_SZ;
      if (_AcpiTbIsValidRsdp(area + off)) {
        return (const AcpiTableRsdp *)(area + off);
      }
    }
  }
  area = _AcpiOsMapMemory(0xE0000, 0xFFFFF + 1 - 0xE0000);
  KINFOF("search ROM BIOS for RSDP");
  off = ROUNDUP(0xFFFFF + 1 - 0xE0000, PARA_SZ);
  while (off != 0) {
    off -= PARA_SZ;
    if (_AcpiTbIsValidRsdp(area + off)) {
      return (const AcpiTableRsdp *)(area + off);
    }
  }
  return NULL;
}

textstartup void *_AcpiMapTable(uintptr_t phy) {
  void *p = _AcpiOsMapRoMemory(phy, sizeof(AcpiTableHeader));
  AcpiTableHeader *hdr = p;
  size_t length = hdr->Length;
  if (length < sizeof(*hdr)) {
    KDIEF("ACPI table @ %p has short length %#zx", p, length);
  } else {
    p = _AcpiOsMapRoMemory(phy, length);
  }
  _AcpiTbVerifyChecksum((const uint8_t *)p, length);
  return p;
}

textstartup void _AcpiXsdtInit(void) {
  if (IsMetal()) {
    size_t nents, i;
    void **ents = NULL;
    const AcpiTableRsdp *rsdp = _AcpiFindRsdp();
    if (!rsdp) {
      KINFOF("no RSDP found");
      return;
    }
    KINFOF("RSDP @ %p", rsdp);
    if (rsdp->Revision <= 1 ||
        rsdp->Length < offsetof(AcpiTableRsdp, Reserved) ||
        !rsdp->XsdtPhysicalAddress) {
      const AcpiTableRsdt *rsdt = _AcpiMapTable(rsdp->RsdtPhysicalAddress);
      nents = (rsdt->Header.Length - sizeof(rsdt->Header)) / sizeof(uint32_t);
      KINFOF("RSDT @ %p, %#zx entries", rsdt, nents);
      ents = _AcpiOsAllocate(nents * sizeof(AcpiTableHeader *));
      if (ents) {
        for (i = 0; i < nents; ++i) {
          ents[i] = _AcpiMapTable(rsdt->TableOffsetEntry[i]);
        }
      }
    } else {
      const AcpiTableXsdt *xsdt = _AcpiMapTable(rsdp->XsdtPhysicalAddress);
      nents = (xsdt->Header.Length - sizeof(xsdt->Header)) / sizeof(uint64_t);
      KINFOF("XSDT @ %p, %#zx entries", xsdt, nents);
      ents = _AcpiOsAllocate(nents * sizeof(AcpiTableHeader *));
      if (ents) {
        for (i = 0; i < nents; ++i) {
          ents[i] = _AcpiMapTable(xsdt->TableOffsetEntry[i]);
        }
      }
    }
    if (!ents) {
      KWARNF("no memory for mapped RSDT/XSDT entries");
      nents = 0;
    }
    _AcpiXsdtEntries = ents;
    _AcpiXsdtNumEntries = nents;
  }
}

textstartup AcpiStatus _AcpiGetTableImpl(uint32_t sig, uint32_t inst,
                                         void **phdr) {
  void **p = _AcpiXsdtEntries;
  size_t n = _AcpiXsdtNumEntries;
  while (n-- != 0) {
    AcpiTableHeader *h = *p++;
    if (READ32LE(h->Signature) != sig) continue;
    if (inst-- != 0) continue;
    *phdr = h;
    return kAcpiOk;
  }
  return kAcpiExNotFound;
}

#endif /* __x86_64__ */
