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
#include "libc/irq/acpi.internal.h"
#include "libc/str/str.h"

#ifdef __x86_64__

typedef struct {
  /* lowest-level NameSeg of the most recent NameString/NamePath */
  uint8_t name_seg[4];
} AmlParseState;

textstartup static void _AcpiCheckParseOverrun(const uint8_t *nxt,
                                               const uint8_t *aml_end) {
  if (nxt > aml_end) {
    ACPI_FATAL("AML: parse overrun @ %p > %p", nxt, aml_end);
  }
}

textstartup static void _AcpiCheckLeadNameChar(const uint8_t *aml, uint8_t c) {
  if ((c >= 'A' && c <= 'Z') || c == '_') return;
  ACPI_FATAL("AML: bad NameSeg %#x %#x %#x %#x @ %p",
             (unsigned)aml[0], (unsigned)aml[1],
             (unsigned)aml[2], (unsigned)aml[3], aml);
}

textstartup static void _AcpiCheckNameChar(const uint8_t *aml, uint8_t c) {
  if (c >= '0' && c <= '9') return;
  _AcpiCheckLeadNameChar(aml, c);
}

textstartup static const uint8_t *_AcpiParseNameSeg(AmlParseState *parse,
                                                    const uint8_t *aml,
                                                    const uint8_t *aml_end) {
  const uint8_t *nxt = aml + 4;
  uint8_t c0 = aml[0], c1 = aml[1], c2 = aml[2], c3 = aml[3];
  _AcpiCheckLeadNameChar(aml, c0);
  _AcpiCheckNameChar(aml, c1);
  _AcpiCheckNameChar(aml, c2);
  _AcpiCheckNameChar(aml, c3);
  parse->name_seg[0] = c0;
  parse->name_seg[1] = c1;
  parse->name_seg[2] = c2;
  parse->name_seg[3] = c3;
  return nxt;
}

textstartup static const uint8_t *_AcpiParseNameString(AmlParseState *parse,
                                                       const uint8_t *aml,
                                                       const uint8_t *aml_end)
{
  const uint8_t *nxt = aml + 1;
  unsigned seg_count;
  switch (aml[0]) {
    case kAmlNullName:
      parse->name_seg[0] = '_';
      parse->name_seg[1] = '_';
      parse->name_seg[2] = '_';
      parse->name_seg[3] = '_';
      return nxt;
    case kAmlDualNamePrefix:
      nxt = _AcpiParseNameSeg(parse, nxt, aml_end);
      nxt = _AcpiParseNameSeg(parse, nxt, aml_end);
      return nxt;
    case kAmlMultiNamePrefix:
      ++nxt;
      seg_count = aml[1];
      if (!seg_count) {
        ACPI_FATAL("AML: MultiNamePath with zero SegCount @ %p", aml);
      }
      while (seg_count-- != 0) {
        nxt = _AcpiParseNameSeg(parse, nxt, aml_end);
      }
      return nxt;
    case kAmlRootPrefix:
    case kAmlParentPrefix:
      return _AcpiParseNameString(parse, nxt, aml_end);
    default:
      return _AcpiParseNameSeg(parse, aml, aml_end);
  }
}

textstartup static const uint8_t *_AcpiParsePkgLength(const uint8_t *aml,
                                                      const uint8_t *aml_end,
                                                      uint32_t *pkg_length) {
  uint8_t lead = aml[0];
  uint32_t pl = 0;
  const uint8_t *nxt = aml + 1 + (lead >> 6);
  switch (lead >> 6) {
    default:
      pl  = (uint32_t)aml[3] << 20;  /* FALLTHRU */
    case 2:
      pl |= (uint32_t)aml[2] << 12;  /* FALLTHRU */
    case 1:
      pl |= (uint32_t)aml[1] <<  4;
      pl |= lead & 0x0F;
      break;
    case 0:
      pl  = lead;
  }
  if (pkg_length) *pkg_length = pl;
  return nxt;
}

textstartup static const uint8_t *_AcpiParseTermArg(AmlParseState *parse,
                                                    const uint8_t *aml,
                                                    const uint8_t *aml_end) {
  const uint8_t *nxt = aml + 1;
  switch (aml[0]) {
    case kAmlZeroOp:
    case kAmlOneOp:
    case kAmlOnesOp:
      return nxt;
    case kAmlByteOp:
      return nxt + 1;
    case kAmlWordOp:
      return nxt + 2;
    case kAmlDwordOp:
      return nxt + 4;
    case kAmlQwordOp:
      return nxt + 8;
    case 'A' ... 'Z':
    case '_':
    case kAmlDualNamePrefix:
    case kAmlMultiNamePrefix:
    case kAmlRootPrefix:
    case kAmlParentPrefix:
      /* FIXME? */
      nxt = _AcpiParseNameString(parse, aml, aml_end);
      ACPI_WARN("AML: assuming ...%c%c%c%c @ %p not method call",
                (int)parse->name_seg[0], (int)parse->name_seg[1],
                (int)parse->name_seg[2], (int)parse->name_seg[3], aml);
      return nxt;
    case kAmlAddOp:
    case kAmlConcatOp:
    case kAmlSubtractOp:
    case kAmlMultiplyOp:
    case kAmlShiftLeftOp:
    case kAmlShiftRightOp:
    case kAmlAndOp:
    case kAmlNandOp:
    case kAmlOrOp:
    case kAmlNorOp:
      nxt = _AcpiParseTermArg(parse, nxt, aml_end);
      nxt = _AcpiParseTermArg(parse, nxt, aml_end);
      nxt = _AcpiParseNameString(parse, nxt, aml_end);
      return nxt;
    default:
      ACPI_FATAL("AML: unknown TermArg type %#x @ %p", (unsigned)aml[0], aml);
  }
}

static const uint8_t *_AcpiParseTermList(AmlParseState *,
                                         const uint8_t *, const uint8_t *);

textstartup static const uint8_t *_AcpiParseTermObj(AmlParseState *parse,
                                                    const uint8_t *aml,
                                                    const uint8_t *aml_end) {
  const uint8_t *nxt = aml + 1, *scope_end;
  uint32_t pl;
  _AcpiCheckParseOverrun(nxt, aml_end);
  switch (aml[0]) {
    case kAmlZeroOp:
    case kAmlOneOp:
    case kAmlOnesOp:
      return nxt;
    case kAmlAliasOp:
      nxt = _AcpiParseNameString(parse, nxt, aml_end);
      nxt = _AcpiParseNameString(parse, nxt, aml_end);
      return nxt;
    case kAmlNameOp:
      nxt = _AcpiParseNameString(parse, nxt, aml_end);
      nxt = _AcpiParseTermObj(parse, nxt, aml_end);
      return nxt;
    case kAmlByteOp:
      return nxt + 1;
    case kAmlWordOp:
      return nxt + 2;
    case kAmlDwordOp:
      return nxt + 4;
    case kAmlStringOp:
      while (*nxt) ++nxt;
      return nxt + 1;
    case kAmlQwordOp:
      return nxt + 8;
    case kAmlScopeOp:
      nxt = _AcpiParsePkgLength(nxt, aml_end, &pl);
      scope_end = aml + 1 + pl;
      nxt = _AcpiParseNameString(parse, nxt, scope_end);
      nxt = _AcpiParseTermList(parse, nxt, scope_end);
      return nxt;
    case kAmlBufferOp:
    case kAmlPackageOp:
    case kAmlVariablePackageOp:
    case kAmlMethodOp:
      _AcpiParsePkgLength(nxt, aml_end, &pl);
      /* ignore */
      return nxt + pl;
    case kAmlExtendedPrefix:
      ++nxt;
      switch (aml[1]) {
        case kAmlXMutexOp:
          nxt = _AcpiParseNameString(parse, nxt, aml_end);
          ++nxt;
          return nxt;
        case kAmlXRegionOp:
          nxt = _AcpiParseNameString(parse, nxt, aml_end);
          ++nxt;
          nxt = _AcpiParseTermArg(parse, nxt, aml_end);
          nxt = _AcpiParseTermArg(parse, nxt, aml_end);
          return nxt;
        case kAmlXFieldOp:
        case kAmlXProcessorOp:
        case kAmlXPowerResourceOp:
        case kAmlXThermalZoneOp:
        case kAmlXIndexFieldOp:
          _AcpiParsePkgLength(nxt, aml_end, &pl);
          /* ignore */
          return nxt + pl;
        case kAmlXDeviceOp:
          ACPI_INFO("AML: DefDevice @ %p", aml);
          _AcpiParsePkgLength(nxt, aml_end, &pl);
          /* ignore */
          return nxt + pl;
        default:
          ACPI_FATAL("AML: unknown TermObj type %#x %#x @ %p",
                     (unsigned)kAmlExtendedPrefix, (unsigned)aml[1], aml);
      }
    case kAmlCreateByteFieldOp:
    case kAmlCreateWordFieldOp:
    case kAmlCreateDwordFieldOp:
    case kAmlCreateQwordFieldOp:
      nxt = _AcpiParseTermArg(parse, nxt, aml_end);
      nxt = _AcpiParseTermArg(parse, nxt, aml_end);
      nxt = _AcpiParseNameString(parse, nxt, aml_end);
      return nxt;
    case kAmlIfOp:
      ACPI_WARN("AML: skipping DefIfElse @ %p", aml);
      _AcpiParsePkgLength(nxt, aml_end, &pl);
      return nxt + pl;
    default:
      ACPI_FATAL("AML: unknown TermObj type %#x @ %p", (unsigned)aml[0], aml);
  }
}

textstartup static const uint8_t *_AcpiParseTermList(AmlParseState *parse,
                                                     const uint8_t *aml,
                                                     const uint8_t *aml_end) {
  const uint8_t *nxt = aml;
  while (nxt != aml_end) {
    nxt = _AcpiParseTermObj(parse, nxt, aml_end);
  }
  return nxt;
}

textstartup static void _AcpiDsdtInit(uintptr_t dsdt_phy) {
  const AcpiTableDsdt *dsdt;
  size_t length;
  const uint8_t *aml, *aml_end;
  AmlParseState parse;
  if (!dsdt_phy) {
    ACPI_WARN("FADT: no DSDT");
    return;
  }
  dsdt = _AcpiMapTable(dsdt_phy);
  ACPI_INFO("FADT: DSDT @ %p", dsdt);
  length = dsdt->Header.Length;
  if (length <= offsetof(AcpiTableDsdt, Aml)) {
    ACPI_WARN("DSDT: no AML?");
    return;
  }
  length -= offsetof(AcpiTableDsdt, Aml);
  aml = dsdt->Aml;
  aml_end = aml + length;
  ACPI_INFO("AML @ %p,+%#zx", aml, length);
  memset(&parse, 0, sizeof(parse));
  _AcpiParseTermList(&parse, aml, aml_end);
}

textstartup void _AcpiFadtInit(void) {
  if (IsMetal()) {
    const AcpiTableFadt *fadt;
    size_t length;
    uint16_t flags;
    uintptr_t dsdt_phy = 0;
    if (!_AcpiSuccess(_AcpiGetTable("FACP", 0, (void **)&fadt))) {
      ACPI_INFO("no FADT found");
      return;
    }
    length = fadt->Header.Length;
    ACPI_INFO("FADT @ %p,+%#zx", fadt, length);
    _Static_assert(offsetof(AcpiTableFadt, Dsdt) == 40);
    _Static_assert(offsetof(AcpiTableFadt, BootFlags) == 109);
    _Static_assert(offsetof(AcpiTableFadt, XDsdt) == 140);
    if (length >= offsetof(AcpiTableFadt, BootFlags) + sizeof(fadt->BootFlags))
    {
      _AcpiBootFlags = flags = fadt->BootFlags;
      ACPI_INFO("FADT: boot flags %#x", (unsigned)flags);
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
