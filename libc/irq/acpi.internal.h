#ifndef COSMOPOLITAN_LIBC_IRQ_ACPI_INTERNAL_H_
#define COSMOPOLITAN_LIBC_IRQ_ACPI_INTERNAL_H_
#include "libc/intrin/kprintf.h"

/**
 * @internal
 * @fileoverview Declarations for bare metal code to interact with ACPI
 *
 * @see UEFI Forum, Inc.  Advanced Configuration and Power Interface (ACPI)
 *      Specification, Version 6.5, 2022.  https://uefi.org/specifications
 * @see Intel Corporation.  ACPI Component Architecture: User Guide and
 *      Programmer Reference, Revision 6.2, 2017.  https://acpica.org
 */

/**
 * @internal
 * AcpiStatus values.
 */
#define kAcpiOk            0x0000
#define kAcpiExNotFound    0x0005
#define kAcpiExBadHeader   0x2002
#define kAcpiExBadChecksum 0x2003

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
#define kAcpiMadtLocalApic   0
#define kAcpiMadtIoApic      1
#define kAcpiMadtIntOverride 2

#if !(__ASSEMBLER__ + __LINKER__ + 0)

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
  uint32_t Address; /* local APIC address */
  uint32_t Flags;   /* multiple APIC flags */
  char Subtable[];  /* ...interrupt controller structures... */
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

typedef uint32_t AcpiStatus;

extern size_t _AcpiXsdtNumEntries, _AcpiNumIoApics;
extern void **_AcpiXsdtEntries;
extern uint16_t _AcpiBootFlags;
extern uint32_t _AcpiMadtFlags;
extern const AcpiMadtIoApic **_AcpiIoApics;

extern void *_AcpiOsMapUncachedMemory(uintptr_t, size_t);
extern void *_AcpiOsAllocate(size_t);
extern void *_AcpiMapTable(uintptr_t);

extern AcpiStatus _AcpiGetTableImpl(uint32_t, uint32_t, void **);

forceinline bool _AcpiSuccess(AcpiStatus __sta) {
  return __sta == kAcpiOk;
}

forceinline AcpiStatus _AcpiGetTable(const char __sig[4], uint32_t __inst,
                                     void **__phdr) {
  unsigned sig32le = (unsigned)(unsigned char)__sig[0] |
                     (unsigned)(unsigned char)__sig[1] << 8 |
                     (unsigned)(unsigned char)__sig[2] << 16 |
                     (unsigned)(unsigned char)__sig[3] << 24;
  return _AcpiGetTableImpl(sig32le, __inst, __phdr);
}

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_IRQ_ACPI_INTERNAL_H_ */
