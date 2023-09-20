#ifndef COSMOPOLITAN_LIBC_IRQ_ACPI_INTERNAL_H_
#define COSMOPOLITAN_LIBC_IRQ_ACPI_INTERNAL_H_
#include "libc/dce.h"
#include "libc/intrin/bits.h"
#include "libc/intrin/bswap.h"
#include "libc/intrin/kprintf.h"
#include "libc/log/color.internal.h"

/**
 * @internal
 * AcpiStatus values.
 */
#define kAcpiOk             0x0000
#define kAcpiExNotFound     0x0005
#define kAcpiExBadHeader    0x2002
#define kAcpiExBadChecksum  0x2003

/**
 * @internal
 * Flags for AcpiTableMadt::Flags.
 */
#define kAcpiMadtPcAtCompat 0x0001

/**
 * @internal
 * Flags for AcpiTableFadt::BootFlags.
 */
#define kAcpiFadtLegacyDevices 0x0001
#define kAcpiFadt8042          0x0002
#define kAcpiFadtNoVga         0x0004
#define kAcpiFadtNoMsi         0x0008
#define kAcpiFadtNoAspm        0x0010
#define kAcpiFadtNoCmosRtc     0x0020

/**
 * @internal
 * Values for AcpiSubtableHeader::Type under an AcpiTableMadt.
 */
#define kAcpiMadtLocalApic    0
#define kAcpiMadtIoApic       1
#define kAcpiMadtIntOverride  2

/**
 * @internal
 * ACPI Machine Language (AML) opcodes.
 */
#define kAmlZeroOp             0x00
#define kAmlOneOp              0x01
#define kAmlAliasOp            0x06
#define kAmlNameOp             0x08
#define kAmlByteOp             0x0A
#define kAmlWordOp             0x0B
#define kAmlDwordOp            0x0C
#define kAmlStringOp           0x0D
#define kAmlQwordOp            0x0E
#define kAmlScopeOp            0x10
#define kAmlBufferOp           0x11
#define kAmlPackageOp          0x12
#define kAmlVariablePackageOp  0x13
#define kAmlMethodOp           0x14
#define kAmlExternalOp         0x15
#define kAmlExtendedPrefix     0x5B
#define kAmlAddOp              0x72
#define kAmlConcatOp           0x73
#define kAmlSubtractOp         0x74
#define kAmlMultiplyOp         0x77
#define kAmlShiftLeftOp        0x79
#define kAmlShiftRightOp       0x7A
#define kAmlAndOp              0x7B
#define kAmlNandOp             0x7C
#define kAmlOrOp               0x7D
#define kAmlNorOp              0x7E
#define kAmlCreateDwordFieldOp 0x8A
#define kAmlCreateWordFieldOp  0x8B
#define kAmlCreateByteFieldOp  0x8C
#define kAmlCreateBitFieldOp   0x8D
#define kAmlCreateQwordFieldOp 0x8F
#define kAmlIfOp               0xA0
#define kAmlOnesOp             0xFF

/**
 * @internal
 * ACPI Machine Language (AML) opcodes prefixed with kAmlExtendedPrefix.
 */
#define kAmlXMutexOp         0x01
#define kAmlXRegionOp        0x80
#define kAmlXFieldOp         0x81
#define kAmlXDeviceOp        0x82
#define kAmlXProcessorOp     0x83
#define kAmlXPowerResourceOp 0x84
#define kAmlXThermalZoneOp   0x85
#define kAmlXIndexFieldOp    0x86

/**
 * @internal
 * ACPI Machine Language (AML) name prefix codes.
 */
#define kAmlNullName          0x00
#define kAmlDualNamePrefix    0x2E
#define kAmlMultiNamePrefix   0x2F
#define kAmlRootPrefix        0x5C
#define kAmlParentPrefix      0x5E	

#if !(__ASSEMBLER__ + __LINKER__ + 0)

/**
 * @internal
 * @fileoverview Declarations for bare metal code to interact with ACPI
 *
 * @see UEFI Forum, Inc.  Advanced Configuration and Power Interface (ACPI)
 *      Specification, Version 6.5, 2022.  https://uefi.org/specifications
 * @see Intel Corporation.  ACPI Component Architecture: User Guide and
 *      Programmer Reference, Revision 6.2, 2017.  https://acpica.org
 */

COSMOPOLITAN_C_START_

/**
 * @internal
 * Structure of an ACPI Root System Description Pointer (RSDP) table.  This
 * points to either an RSDT, or an XSDT, or both.
 */
typedef struct {
  uint8_t Signature[8];
  uint8_t Checksum;
  uint8_t OemId[6];
  uint8_t Revision;
  uint32_t RsdtPhysicalAddress;
  uint32_t Length;
  uint64_t XsdtPhysicalAddress;
  uint8_t ExtendedChecksum;
  uint8_t Reserved[3];
} AcpiTableRsdp;

/**
 * @internal
 * Structure of a System Description Table Header, which appears at the
 * beginning of each ACPI system description table.
 */
typedef struct thatispacked {
  uint8_t Signature[4];
  uint32_t Length;
  uint8_t Revision;
  uint8_t Checksum;
  uint8_t OemId[6];
  uint8_t OemTableId[8];
  uint32_t OemRevision;
  uint8_t AslCompilerId[4];
  uint32_t AslCompilerRevision;
} AcpiTableHeader;

/**
 * @internal
 * ACPI Root System Description Table (RSDT) structure.
 */
typedef struct thatispacked {
  AcpiTableHeader Header;
  uint32_t TableOffsetEntry[];
} AcpiTableRsdt;

/**
 * @internal
 * ACPI Extended System Description Table (XSDT) structure.
 */
typedef struct thatispacked {
  AcpiTableHeader Header;
  uint64_t TableOffsetEntry[];
} AcpiTableXsdt;

typedef struct thatispacked {
  uint8_t SpaceId;
  uint8_t BitWidth;
  uint8_t BitOffset;
  uint8_t AccessWidth;
  uint64_t Address;
} AcpiGenericAddress;

typedef struct thatispacked {
  uint8_t Type;
  uint8_t Length;
} AcpiSubtableHeader;

typedef struct thatispacked {
  AcpiSubtableHeader Header;
  uint8_t Id;
  uint8_t Reserved;
  uint32_t Address;
  uint32_t GlobalIrqBase;
} AcpiMadtIoApic;

/**
 * @internal
 * ACPI Multiple APIC Description Table (MADT) structure.
 */
typedef struct thatispacked {
  AcpiTableHeader Header;
  uint32_t Address;  /* local APIC address */
  uint32_t Flags;    /* multiple APIC flags */
  char Subtable[];   /* ...interrupt controller structures... */
} AcpiTableMadt;

/**
 * @internal
 * Fixed ACPI Description Table (FADT) structure.
 */
typedef struct thatispacked {
  AcpiTableHeader Header;
  uint32_t Facs;
  uint32_t Dsdt;
  uint8_t Model;
  uint8_t PreferredProfile;
  uint16_t SciInterrupt;
  uint32_t SmiCommand;
  uint8_t AcpiEnable;
  uint8_t AcpiDisable;
  uint8_t S4BiosRequest;
  uint8_t PstateControl;
  uint32_t Pm1aEventBlock;
  uint32_t Pm1bEventBlock;
  uint32_t Pm1aControlBlock;
  uint32_t Pm1bControlBlock;
  uint32_t Pm2ControlBlock;
  uint32_t PmTimerBlock;
  uint32_t Gpe0Block;
  uint32_t Gpe1Block;
  uint8_t Pm1EventLength;
  uint8_t Pm1ControlLength;
  uint8_t Pm2ControlLength;
  uint8_t PmTimerLength;
  uint8_t Gpe0BlockLength;
  uint8_t Gpe1BlockLength;
  uint8_t Gpe1Base;
  uint8_t CstControl;
  uint16_t C2Latency;
  uint16_t C3Latency;
  uint16_t FlushSize;
  uint16_t FlushStride;
  uint8_t DutyOffset;
  uint8_t DutyWidth;
  uint8_t DayAlarm;
  uint8_t MonthAlarm;
  uint8_t Century;
  uint16_t BootFlags;
  uint8_t Reserved;
  uint32_t Flags;
  AcpiGenericAddress ResetRegister;
  uint8_t ResetValue;
  uint16_t ArmBootFlags;
  uint8_t MinorRevision;
  uint64_t XFacs;
  uint64_t XDsdt;
  AcpiGenericAddress XPm1aEventBlock;
  AcpiGenericAddress XPm1bEventBlock;
  AcpiGenericAddress XPm1aControlBlock;
  AcpiGenericAddress XPm1bControlBlock;
  AcpiGenericAddress XPm2ControlBlock;
  AcpiGenericAddress XPmTimerBlock;
  AcpiGenericAddress XGpe0Block;
  AcpiGenericAddress XGpe1Block;
  AcpiGenericAddress SleepControl;
  AcpiGenericAddress SleepStatus;
  uint64_t HypervisorId;
} AcpiTableFadt;

/**
 * @internal
 * ACPI Differentiated System Description Table (DSDT) structure.
 */
typedef struct thatispacked {
  AcpiTableHeader Header;
  uint8_t Aml[];
} AcpiTableDsdt;

/**
 * @internal
 * Private type for a device's hardware id.
 */
typedef uint64_t AcpiDeviceHid;

/**
 * @internal
 * Private information about a device defined by an ACPI Device Package.
 */
typedef struct AcpiDefDevice {
  struct AcpiDefDevice *next;
  AcpiDeviceHid hid;
  const uint8_t *crs;
  size_t crs_size;
} AcpiDefDevice;

typedef uint32_t AcpiStatus;

extern size_t _AcpiXsdtNumEntries, _AcpiNumIoApics;
extern void **_AcpiXsdtEntries;
extern uint16_t _AcpiBootFlags;
extern uint32_t _AcpiMadtFlags;
extern const AcpiMadtIoApic **_AcpiIoApics;
extern AcpiDefDevice *_AcpiDefDevices;

extern void *_AcpiOsMapUncachedMemory(uintptr_t, size_t);
extern void *_AcpiOsAllocate(size_t);
extern void *_AcpiMapTable(uintptr_t);

extern AcpiStatus _AcpiGetTableImpl(uint32_t, uint32_t, void **);

forceinline bool _AcpiSuccess(AcpiStatus __sta) {
  return __sta == kAcpiOk;
}

forceinline AcpiStatus _AcpiGetTable(const char __sig[4], uint32_t __inst,
                                     void **__phdr) {
  uint8_t __sig_copy[4] = { __sig[0], __sig[1], __sig[2], __sig[3] };
  return _AcpiGetTableImpl(READ32LE(__sig_copy), __inst, __phdr);
}

/**
 * @internal
 * Converts a hardware id for an EISA device from string form to numeric
 * form.  If the HID is not an EISA type id, return 0.
 *
 * TODO: when the need arises, find a reasonable way to "compress" ACPI &
 * PCI device ids as well.
 */
forceinline AcpiDeviceHid
_AcpiCompressHid(const uint8_t *__hid, size_t __len) {
  uint8_t __c;
  unsigned __a0, __a1, __a2, __d0, __d1, __d2, __d3;
  uint32_t __evalu;
  if (__len != 7) return 0;
  __c = __hid[0];
  if (__c < 'A' || __c > 'Z') return 0;
  __a0 = __c - 'A' + 1;
  __c = __hid[1];
  if (__c < 'A' || __c > 'Z') return 0;
  __a1 = __c - 'A' + 1;
  __c = __hid[2];
  if (__c < 'A' || __c > 'Z') return 0;
  __a2 = __c - 'A' + 1;
  __c = __hid[3];
  if (__c < '0' || __c > 'F' || (__c > '9' && __c < 'A')) return 0;
  __d0 = __c <= '9' ? __c - '0' : __c - 'A' + 0xA;
  __c = __hid[4];
  if (__c < '0' || __c > 'F' || (__c > '9' && __c < 'A')) return 0;
  __d1 = __c <= '9' ? __c - '0' : __c - 'A' + 0xA;
  __c = __hid[5];
  if (__c < '0' || __c > 'F' || (__c > '9' && __c < 'A')) return 0;
  __d2 = __c <= '9' ? __c - '0' : __c - 'A' + 0xA;
  __c = __hid[6];
  if (__c < '0' || __c > 'F' || (__c > '9' && __c < 'A')) return 0;
  __d3 = __c <= '9' ? __c - '0' : __c - 'A' + 0xA;
  __evalu = (uint32_t)__a0 << 26 | (uint32_t)__a1 << 21 | (uint32_t)__a2 << 16 |
          __d0 << 12 | __d1 << 8 | __d2 << 4 | __d3;
  return bswap_32(__evalu);
}

#define kAcpiDecompressHidMax 8

forceinline bool _AcpiDecompressHid(uintmax_t __hid,
                                    char __str[kAcpiDecompressHidMax]) {
  uint32_t __evalu;
  unsigned char __a0, __a1, __a2, __d0, __d1, __d2, __d3;
  if (__hid >= (1ul << 31)) return false;
  __evalu = bswap_32((uint32_t)__hid);
  __a0 = (__evalu >> 26) & 0x1F;
  if (!__a0 || __a0 > 26) return false;
  __a1 = (__evalu >> 21) & 0x1F;
  if (!__a1 || __a1 > 26) return false;
  __a2 = (__evalu >> 16) & 0x1F;
  if (!__a2 || __a2 > 26) return false;
  __d0 = (__evalu >> 12) & 0xF;
  __d1 = (__evalu >>  8) & 0xF;
  __d2 = (__evalu >>  4) & 0xF;
  __d3 = (__evalu >>  0) & 0xF;
  __str[0] = __a0 - 1 + 'A';
  __str[1] = __a1 - 1 + 'A';
  __str[2] = __a2 - 1 + 'A';
  __str[3] = __d0 <= 9 ? __d0 + '0' : __d0 - 0xA + 'A';
  __str[4] = __d1 <= 9 ? __d1 + '0' : __d1 - 0xA + 'A';
  __str[5] = __d2 <= 9 ? __d2 + '0' : __d2 - 0xA + 'A';
  __str[6] = __d3 <= 9 ? __d3 + '0' : __d3 - 0xA + 'A';
  __str[7] = 0;
  return true;
}

#define ACPI_INFO(FMT, ...)                                            \
        do {                                                           \
          if (!IsTiny()) {                                             \
            kprintf("%r%s%s:%d: " FMT "%s\n",                          \
                    SUBTLE, __FILE__, __LINE__, ##__VA_ARGS__, RESET); \
          }                                                            \
        } while (0)
#define ACPI_WARN(FMT, ...)                                            \
        do {                                                           \
          if (!IsTiny()) {                                             \
            kprintf("%r%swarn: %s:%d: " FMT "%s\n",                    \
                    BLUE1, __FILE__, __LINE__, ##__VA_ARGS__, RESET);  \
          }                                                            \
        } while (0)
#define ACPI_FATAL(FMT, ...)                                           \
        do {                                                           \
          kprintf("%r%sfatal: %s:%d: " FMT "%s\n",                     \
                  RED, __FILE__, __LINE__, ##__VA_ARGS__, RESET);      \
          for (;;) asm volatile("cli\n\thlt");                         \
        } while (0)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_IRQ_ACPI_INTERNAL_H_ */
