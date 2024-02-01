/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "ape/relocations.h"
#include "ape/sections.internal.h"
#include "libc/dce.h"
#include "libc/intrin/newbie.h"
#include "libc/intrin/weaken.h"
#include "libc/macros.internal.h"
#include "libc/nt/efi.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/runtime/e820.internal.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/pc.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"

#pragma GCC diagnostic ignored "-Warray-bounds"
#pragma GCC diagnostic ignored "-Wstringop-overflow"

#ifdef __x86_64__

/* TODO: Why can't we change CR3? Could it really need PML5T? */
/* TODO: Why does QEMU in UEFI mode take ten seconds to boot? */

struct EfiArgs {
  char *Args[0x400 / sizeof(char *)];
  char ArgBlock[0xC00];
};

static EFI_GUID kEfiLoadedImageProtocol = LOADED_IMAGE_PROTOCOL;
static EFI_GUID kEfiGraphicsOutputProtocol = GRAPHICS_OUTPUT_PROTOCOL;
static const EFI_GUID kEfiAcpi20TableGuid = ACPI_20_TABLE_GUID;
static const EFI_GUID kEfiAcpi10TableGuid = ACPI_10_TABLE_GUID;

extern const char vga_console[];
extern void _EfiPostboot(struct mman *, uint64_t *, uintptr_t, char **);

static void EfiInitVga(struct mman *mm, EFI_SYSTEM_TABLE *SystemTable) {
  EFI_GRAPHICS_OUTPUT_PROTOCOL *GraphInfo;
  EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE *GraphMode;
  EFI_PIXEL_BITMASK *PixelInfo;
  unsigned vid_typ = PC_VIDEO_TEXT;
  size_t bytes_per_pix = 0;

  SystemTable->BootServices->LocateProtocol(&kEfiGraphicsOutputProtocol, NULL,
                                            &GraphInfo);
  GraphMode = GraphInfo->Mode;
  switch (GraphMode->Info->PixelFormat) {
    case PixelRedGreenBlueReserved8BitPerColor:
      vid_typ = PC_VIDEO_RGBX8888;
      bytes_per_pix = 4;
      break;
    case PixelBlueGreenRedReserved8BitPerColor:
      vid_typ = PC_VIDEO_BGRX8888;
      bytes_per_pix = 4;
      break;
    case PixelBitMask:
      PixelInfo = &GraphMode->Info->PixelInformation;
      switch (le32toh(PixelInfo->RedMask)) {
        case 0x00FF0000U:
          if (le32toh(PixelInfo->ReservedMask) >= 0x01000000U &&
              le32toh(PixelInfo->GreenMask) == 0x0000FF00U &&
              le32toh(PixelInfo->BlueMask) == 0x000000FFU) {
            vid_typ = PC_VIDEO_BGRX8888;
            bytes_per_pix = 4;
          }
          break;
        case 0x000000FFU:
          if (le32toh(PixelInfo->ReservedMask) >= 0x01000000U &&
              le32toh(PixelInfo->GreenMask) == 0x0000FF00U &&
              le32toh(PixelInfo->BlueMask) == 0x00FF0000U) {
            vid_typ = PC_VIDEO_RGBX8888;
            bytes_per_pix = 4;
          }
          break;
        case 0x0000F800U:
          if (le32toh(PixelInfo->ReservedMask) <= 0x0000FFFFU &&
              le32toh(PixelInfo->GreenMask) == 0x000007E0U &&
              le32toh(PixelInfo->BlueMask) == 0x0000001FU) {
            vid_typ = PC_VIDEO_BGR565;
            bytes_per_pix = 2;
          }
          break;
        case 0x00007C00U:
          if (le32toh(PixelInfo->ReservedMask) <= 0x0000FFFFU &&
              le32toh(PixelInfo->GreenMask) == 0x000003E0U &&
              le32toh(PixelInfo->BlueMask) == 0x0000001FU) {
            vid_typ = PC_VIDEO_BGR555;
            bytes_per_pix = 2;
          }
          break;
      }
    default:
      notpossible;
  }
  if (!bytes_per_pix) notpossible;
  mm->pc_video_type = vid_typ;
  mm->pc_video_stride = GraphMode->Info->PixelsPerScanLine * bytes_per_pix;
  mm->pc_video_width = GraphMode->Info->HorizontalResolution;
  mm->pc_video_height = GraphMode->Info->VerticalResolution;
  mm->pc_video_framebuffer = GraphMode->FrameBufferBase;
  mm->pc_video_framebuffer_size = GraphMode->FrameBufferSize;
  mm->pc_video_curs_info.y = mm->pc_video_curs_info.x = 0;
  SystemTable->BootServices->SetMem((void *)GraphMode->FrameBufferBase,
                                    GraphMode->FrameBufferSize, 0);
}

static void EfiInitAcpi(struct mman *mm, EFI_SYSTEM_TABLE *SystemTable) {
  void *rsdp1 = NULL, *rsdp2 = NULL;
  uintptr_t n = SystemTable->NumberOfTableEntries, i;
  EFI_CONFIGURATION_TABLE *tab;
  for (i = 0, tab = SystemTable->ConfigurationTable; i < n; ++i, ++tab) {
    EFI_GUID *guid = &tab->VendorGuid;
    if (memcmp(guid, &kEfiAcpi20TableGuid, sizeof(EFI_GUID)) == 0) {
      rsdp2 = tab->VendorTable;
    } else if (memcmp(guid, &kEfiAcpi20TableGuid, sizeof(EFI_GUID)) == 0) {
      rsdp1 = tab->VendorTable;
    }
  }
  if (rsdp2) {
    mm->pc_acpi_rsdp = (uintptr_t)rsdp2;
  } else {
    mm->pc_acpi_rsdp = (uintptr_t)rsdp1;
  }
}

/**
 * EFI Application Entrypoint.
 *
 * This entrypoint is mutually exclusive from WinMain since
 * Windows apps and EFI apps use the same PE binary format.
 * So if you want to trade away Windows so that you can use
 * UEFI instead of the normal BIOS boot process, do this:
 *
 *     __static_yoink("EfiMain");
 *     int main() { ... }
 *
 * You can use QEMU to test this, but please note that UEFI
 * goes thousands of times slower than the normal BIOS boot
 *
 *     qemu-system-x86_64 \
 *       -bios OVMF.fd    \
 *       -nographic       \
 *       -net none        \
 *       -drive format=raw,file=fat:rw:o/tool/viz
 *     FS0:
 *     deathstar.com
 *
 * @see libc/dce.h
 */
__msabi EFI_STATUS EfiMain(EFI_HANDLE ImageHandle,
                           EFI_SYSTEM_TABLE *SystemTable) {
  struct mman *mm;
  uint32_t DescVersion;
  uintptr_t i, j, MapSize;
  struct EfiArgs *ArgBlock;
  EFI_LOADED_IMAGE *ImgInfo;
  EFI_MEMORY_DESCRIPTOR *Map, *Desc;
  uint64_t Address;
  uintptr_t Args, MapKey, DescSize;
  uint64_t *pd, *pml4t, *pdt1, *pdt2, *pdpt1, *pdpt2;
  const char16_t *CmdLine;

  extern char os asm("__hostos");
  os = _HOSTMETAL;

  /*
   * Allocates and clears PC-compatible memory and copies image.  Marks the
   * pages as EfiRuntimeServicesData, so that we can simply free up all
   * EfiLoader... and EfiBootServices... pages later.  The first page at
   * address 0 is normally already allocated as EfiBootServicesData, so
   * handle it separately.
   */
  Address = 0;
  SystemTable->BootServices->AllocatePages(
      AllocateAddress, EfiRuntimeServicesData, 4096 / 4096, &Address);
  Address = 4096;
  SystemTable->BootServices->AllocatePages(
      AllocateAddress, EfiRuntimeServicesData, (IMAGE_BASE_REAL - 4096) / 4096,
      &Address);
  Address = 0x79000;
  SystemTable->BootServices->AllocatePages(
      AllocateAddress, EfiRuntimeServicesData,
      (0x7f000 - 0x79000 + sizeof(struct EfiArgs) + 4095) / 4096, &Address);
  Address = IMAGE_BASE_PHYSICAL;
  SystemTable->BootServices->AllocatePages(
      AllocateAddress, EfiRuntimeServicesData,
      ((_end - __executable_start) + 4095) / 4096, &Address);
  mm = __get_mm_phy();
  SystemTable->BootServices->SetMem(mm, sizeof(*mm), 0);
  SystemTable->BootServices->SetMem(
      (void *)0x79000, 0x7f000 - 0x79000 + sizeof(struct EfiArgs), 0);
  SystemTable->BootServices->CopyMem((void *)IMAGE_BASE_PHYSICAL,
                                     __executable_start,
                                     _end - __executable_start);

  /*
   * Converts UEFI shell arguments to argv.
   */
  ArgBlock = (struct EfiArgs *)0x7f000;
  SystemTable->BootServices->HandleProtocol(ImageHandle,
                                            &kEfiLoadedImageProtocol, &ImgInfo);
  CmdLine = (const char16_t *)ImgInfo->LoadOptions;
  if (!CmdLine || !CmdLine[0]) CmdLine = u"BOOTX64.EFI";
  Args = GetDosArgv(CmdLine, ArgBlock->ArgBlock, sizeof(ArgBlock->ArgBlock),
                    ArgBlock->Args, ARRAYLEN(ArgBlock->Args));

  /*
   * Gets information about our current video mode.  Clears the screen.
   * TODO: if needed, switch to a video mode that has a linear frame buffer
   * type we support.
   */
  if (_weaken(vga_console)) EfiInitVga(mm, SystemTable);

  /*
   * Gets a pointer to the ACPI RSDP.
   */
  EfiInitAcpi(mm, SystemTable);

  /*
   * Asks UEFI which parts of our RAM we're allowed to use.
   */
  Map = NULL;
  MapSize = 0;
  SystemTable->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescSize,
                                          &DescVersion);
  SystemTable->BootServices->AllocatePool(EfiLoaderData, MapSize, &Map);
  MapSize *= 2;
  SystemTable->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescSize,
                                          &DescVersion);
  for (j = i = 0, Desc = Map; i < MapSize / DescSize; ++i) {
    switch (Desc->Type) {
      case EfiLoaderCode:
      case EfiLoaderData:
      case EfiBootServicesCode:
      case EfiBootServicesData:
        if (Desc->PhysicalStart != 0) break;
        /* fallthrough */
      case EfiConventionalMemory:
        mm->e820[j].addr = Desc->PhysicalStart;
        mm->e820[j].size = Desc->NumberOfPages * 4096;
        mm->e820[j].type = kMemoryUsable;
        ++j;
    }
    Desc = (EFI_MEMORY_DESCRIPTOR *)((char *)Desc + DescSize);
  }
  SystemTable->BootServices->FreePool(Map);

  /*
   * Sets up page tables.
   */
  pd = (uint64_t *)0x79000;
  pdt1 = (uint64_t *)0x7b000;
  pdt2 = (uint64_t *)0x7a000;
  pdpt1 = (uint64_t *)0x7d000;
  pdpt2 = (uint64_t *)0x7c000;
  pml4t = (uint64_t *)0x7e000;
  for (i = 0; i < 512; ++i) {
    pd[i] = 0x1000 * i + PAGE_V + PAGE_RSRV + PAGE_RW;
  }
  pdt1[0] = (intptr_t)pd + PAGE_V + PAGE_RW;
  pdt2[0] = (intptr_t)pd + PAGE_V + PAGE_RW;
  pdpt1[0] = (intptr_t)pdt1 + PAGE_V + PAGE_RW;
  pdpt2[0] = (intptr_t)pdt2 + PAGE_V + PAGE_RW;
  pml4t[0] = (intptr_t)pdpt1 + PAGE_V + PAGE_RW;
  pml4t[256] = (intptr_t)pdpt2 + PAGE_V + PAGE_RW;

  /*
   * Asks UEFI to handover control?
   */
  SystemTable->BootServices->ExitBootServices(ImageHandle, MapKey);

  /*
   * Switches to copied image and launches program.
   */
  _EfiPostboot(mm, pml4t, Args, ArgBlock->Args);
  __builtin_unreachable();
}

#endif /* __x86_64__ */
