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
#include "libc/intrin/bswap.h"
#include "libc/irq/acpi.internal.h"
#include "libc/str/str.h"

#ifdef __x86_64__

/*
 * Try to semi-correctly parse the ACPI Machine Language (AML) present in
 * the Differentiated System Description Table (DSDT), in order to get an
 * idea of what the system's hardware configuration is.
 *
 * (Parsing AML with anywhere close to 100% correctness — à la Intel's
 * ACPICA — requires us to keep track of a _lot_ of information at parse
 * time, & does not look quite necessary at the moment.  We will see.)
 *
 * The logic for partially parsing AML is largely based on that of the
 * SeaBIOS project, with some additions to handle AML features encountered
 * on actual firmwares.
 *
 * @see libc/irq/acpi.internal.h
 * @see src/fw/dsdt_parser.c in SeaBIOS, https://www.seabios.org/Download
 */

typedef struct {
  /* lowest-level NameSeg of the most recent NameString/NamePath */
  uint8_t name_seg[4];
  /* device structure for current Device Package, or NULL if none */
  AcpiDefDevice *dev;
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
  _AcpiCheckParseOverrun(nxt, aml_end);
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
      ACPI_FATAL("AML: weird TermArg type %#x @ %p", (unsigned)aml[0], aml);
  }
}

textstartup static const uint8_t *_AcpiParseTermArgInt(AmlParseState *parse,
                                                       const uint8_t *aml,
                                                       const uint8_t *aml_end,
                                                       uint64_t *value) {
  const uint8_t *nxt = aml + 1;
  uint64_t v = 0;
  _AcpiCheckParseOverrun(nxt, aml_end);
  switch (aml[0]) {
    case kAmlZeroOp:
      break;
    case kAmlOneOp:
      v = 1;
      break;
    case kAmlOnesOp:
      --v;
      break;
    case kAmlByteOp:
      v = *nxt++;
      break;
    case kAmlWordOp:
      v = READ16LE(nxt);
      nxt += 2;
      break;
    case kAmlDwordOp:
      v = READ32LE(nxt);
      nxt += 4;
      break;
    case kAmlQwordOp:
      v = READ64LE(nxt);
      nxt += 8;
      break;
    default:
      ACPI_FATAL("AML: weird TermArg => Integer type %#x @ %p",
                 (unsigned)aml[0], aml);
  }
  *value = v;
  return nxt;
}

static const uint8_t *_AcpiParseTermList(AmlParseState *,
                                         const uint8_t *, const uint8_t *);
static const uint8_t *_AcpiParseTermObj(AmlParseState *,
                                        const uint8_t *, const uint8_t *);

textstartup static const uint8_t *
_AcpiParseDataObjectHid(AmlParseState *state, const uint8_t *aml,
                        const uint8_t *aml_end, AcpiDeviceHid *hid) {
  const uint8_t *nxt = aml + 1, *end;
  uint64_t value;
  AcpiDeviceHid id;
  switch (aml[0]) {
    case kAmlStringOp:
      end = nxt;
      while (*end) ++end;
      *hid = id = _AcpiCompressHid(nxt, end - nxt);
      ACPI_INFO("AML:   dev _HID \"%!s\"%s", nxt, id ? "" : " (!)");
      return end + 1;
    case kAmlZeroOp:
    case kAmlOneOp:
    case kAmlOnesOp:
    case kAmlByteOp:
    case kAmlWordOp:
    case kAmlDwordOp:
    case kAmlQwordOp:
      nxt = _AcpiParseTermArgInt(state, aml, aml_end, &value);
      if (!IsTiny()) {
        char str[kAcpiDecompressHidMax];
        if (!_AcpiDecompressHid(value, str)) {
          ACPI_INFO("AML:   dev _HID %#" PRIx64 " (!)", value);
        } else {
          ACPI_INFO("AML:   dev _HID EISA %s", str);
        }
      }
      *hid = value;
      return nxt;
    default:
      ACPI_WARN("AML: weird _HID type %#x @ %p", (unsigned)aml[0], aml);
      *hid = 0;
      return _AcpiParseTermObj(state, aml, aml_end);
  }
}

textstartup static const uint8_t *
_AcpiParseDataObjectCrs(AmlParseState *parse, const uint8_t *aml,
                        const uint8_t *aml_end, const uint8_t **crs,
                        size_t *crs_size) {
  const uint8_t *nxt = aml + 1, *initer_end;
  uint8_t *buf;
  uint32_t pl;
  uint64_t buf_size;
  size_t initer_size;
  switch (aml[0]) {
    case kAmlBufferOp:
      nxt = _AcpiParsePkgLength(nxt, aml_end, &pl);
      initer_end = aml + 1 + pl;
      nxt = _AcpiParseTermArgInt(parse, nxt, initer_end, &buf_size);
      _AcpiCheckParseOverrun(nxt, initer_end);
      /*
       * The ACPI 6.5 spec § 19.6.10 says about Buffer objects,
       * "If the BufferSize is larger than the length of the Initializer,
       *  the BufferSize is used as the final buffer size.  At runtime, the
       *  AML interpreter will automatically pad zeros to the Initializer to
       *  match the BufferSize: ...
       * "If the BufferSize is smaller than the length of the Initializer,
       *  the length of the Initializer is used as the buffer size: ..."
       */
      initer_size = initer_end - nxt;
      ACPI_INFO("AML:   dev _CRS @ %p,+%#zx", nxt, initer_size);
      if (buf_size <= initer_size) {
        buf = (uint8_t *)nxt;
        buf_size = initer_size;
      } else {
        buf = _AcpiOsAllocate(buf_size);
        if (!buf) ACPI_FATAL("AML: no memory for _CRS");
        memcpy(buf, nxt, initer_size);
        memset(buf + initer_size, 0, buf_size - initer_size);
        ACPI_INFO("AML:   -> %p,+%#zx", buf, (size_t)buf_size);
      }
      *crs = buf;
      *crs_size = buf_size;
      return initer_end;
    default:
      ACPI_WARN("AML: weird _CRS type %#x @ %p", (unsigned)aml[0], aml);
      *crs = NULL;
      *crs_size = 0;
      return _AcpiParseTermObj(parse, aml, aml_end);
  }
}

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
      if (parse->dev && READ32LE(parse->name_seg) == READ32LE("_HID")) {
        nxt = _AcpiParseDataObjectHid(parse, nxt, aml_end, &parse->dev->hid);
      } else if (parse->dev && READ32LE(parse->name_seg) == READ32LE("_CRS")) {
        nxt = _AcpiParseDataObjectCrs(parse, nxt, aml_end,
                                      &parse->dev->crs, &parse->dev->crs_size);
      } else {
        nxt = _AcpiParseTermObj(parse, nxt, aml_end);
      }
      /* TODO: handle _STA? */
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
          {
            AmlParseState subparse;
            AcpiDefDevice tmp_dev, *dev;
            nxt = _AcpiParsePkgLength(nxt, aml_end, &pl);
            scope_end = aml + 2 + pl;
            nxt = _AcpiParseNameString(parse, nxt, scope_end);
            ACPI_INFO("AML: DefDevice %c%c%c%c @ %p",
                      (int)parse->name_seg[0], (int)parse->name_seg[1],
                      (int)parse->name_seg[2], (int)parse->name_seg[3], aml);
            memset(&tmp_dev, 0, sizeof(tmp_dev));
            subparse = *parse;
            subparse.dev = &tmp_dev;
            nxt = _AcpiParseTermList(&subparse, nxt, scope_end);
            if (tmp_dev.hid && tmp_dev.crs && tmp_dev.crs_size) {
              dev = _AcpiOsAllocate(sizeof(*dev));
              if (!dev) {
                ACPI_WARN("AML: no memory for %c%c%c%c dev info",
                          (int)parse->name_seg[0], (int)parse->name_seg[1],
                          (int)parse->name_seg[2], (int)parse->name_seg[3]);
              } else {
                *dev = tmp_dev;
                dev->next = _AcpiDefDevices;
                _AcpiDefDevices = dev;
                ACPI_INFO("AML: -> %p", dev);
             }
            }
          }
          return nxt;
        default:
          ACPI_FATAL("AML: weird TermObj type %#x %#x @ %p",
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
      ACPI_FATAL("AML: weird TermObj type %#x @ %p", (unsigned)aml[0], aml);
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
    ACPI_INFO("FADT %.8!s @ %p,+%#zx", fadt->Header.OemTableId, fadt, length);
    _Static_assert(offsetof(AcpiTableFadt, Dsdt) == 40);
    _Static_assert(offsetof(AcpiTableFadt, BootFlags) == 109);
    _Static_assert(offsetof(AcpiTableFadt, XDsdt) == 140);
    if (length >= offsetof(AcpiTableFadt, BootFlags) + sizeof(fadt->BootFlags))
    {
      flags = fadt->BootFlags;
      if (READ64LE(fadt->Header.OemTableId) == READ64LE("BXPC    ")) {
        /* Work around incomplete AML tables under QEMU. */
        ACPI_WARN("FADT: boot flags %#x -> %#x (QEMU workaround)",
                  (unsigned)flags, (unsigned)(flags | kAcpiFadtLegacyDevices));
        flags |= kAcpiFadtLegacyDevices;
      } else {
        ACPI_INFO("FADT: boot flags %#x", (unsigned)flags);
      }
      _AcpiBootFlags = flags;
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
