/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "ape/sections.internal.h"
#include "libc/dce.h"
#include "libc/intrin/bits.h"
#include "libc/macros.internal.h"
#include "libc/nt/efi.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/runtime/e820.internal.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/pc.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"

/* TODO: Why can't we change CR3? Could it really need PML5T? */
/* TODO: Why does QEMU in UEFI mode take ten seconds to boot? */

struct EfiArgs {
  char *Args[0x400 / sizeof(char *)];
  char ArgBlock[0xC00];
};

static const EFI_GUID kEfiLoadedImageProtocol = LOADED_IMAGE_PROTOCOL;

/**
 * EFI Application Entrypoint.
 *
 * This entrypoint is mutually exclusive from WinMain since
 * Windows apps and EFI apps use the same PE binary format.
 * So if you want to trade away Windows so that you can use
 * UEFI instead of the normal BIOS boot process, do this:
 *
 *     STATIC_YOINK("EfiMain");
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
__msabi noasan EFI_STATUS EfiMain(EFI_HANDLE ImageHandle,
                                  EFI_SYSTEM_TABLE *SystemTable) {
  int type, x87cw;
  struct mman *mm;
  uint32_t DescVersion;
  uintptr_t i, j, MapSize;
  struct EfiArgs *ArgBlock;
  EFI_LOADED_IMAGE *ImgInfo;
  EFI_MEMORY_DESCRIPTOR *Map;
  uintptr_t Args, MapKey, DescSize;
  uint64_t p, pe, cr4, *m, *pd, *sp, *pml4t, *pdt1, *pdt2, *pdpt1, *pdpt2;
  extern char __os asm("__hostos");

  __os = _HOSTMETAL;

  /*
   * Allocates and clears PC-compatible memory and copies image.
   */
  SystemTable->BootServices->AllocatePages(
      AllocateAddress, EfiConventionalMemory,
      MAX(2 * 1024 * 1024, 1024 * 1024 + (_end - _base)) / 4096, 0);
  SystemTable->BootServices->SetMem(0, 0x80000, 0);
  SystemTable->BootServices->CopyMem((void *)(1024 * 1024), _base,
                                     _end - _base);

  /*
   * Converts UEFI shell arguments to argv.
   */
  ArgBlock = (struct EfiArgs *)0x7e000;
  SystemTable->BootServices->HandleProtocol(ImageHandle,
                                            &kEfiLoadedImageProtocol, &ImgInfo);
  Args = GetDosArgv(ImgInfo->LoadOptions, ArgBlock->ArgBlock,
                    sizeof(ArgBlock->ArgBlock), ArgBlock->Args,
                    ARRAYLEN(ArgBlock->Args));

  /*
   * Asks UEFI which parts of our RAM we're allowed to use.
   */
  Map = NULL;
  MapSize = 0;
  SystemTable->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescSize,
                                          &DescVersion);
  SystemTable->BootServices->AllocatePool(EfiLoaderData, MapSize, Map);
  SystemTable->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescSize,
                                          &DescVersion);
  asm("xor\t%0,%0" : "=r"(mm)); /* gcc assumes null isn't mapped */
  for (j = i = 0; i < MapSize / sizeof(EFI_MEMORY_DESCRIPTOR); ++i) {
    if (Map[i].Type != EfiConventionalMemory) continue;
    mm->e820[j].addr = Map[i].PhysicalStart;
    mm->e820[j].size = Map[i].NumberOfPages * 4096;
    mm->e820[j].type = kMemoryUsable;
    ++j;
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
  __map_phdrs(mm, pml4t, 1024 * 1024);

  /*
   * Asks UEFI to handover control?
   */
  SystemTable->BootServices->ExitBootServices(ImageHandle, MapKey);

  /*
   * Launches program.
   */
  asm volatile("fldcw\t%3\n\t"
               "mov\t%4,%%cr3\n\t"
               ".weak\tape_stack_vaddr\n\t"
               ".weak\tape_stack_memsz\n\t"
               "movabs\t$ape_stack_vaddr,%%rsp\n\t"
               "add\t$ape_stack_memsz,%%rsp\n\t"
               "push\t$0\n\t"   /* auxv[1][1] */
               "push\t$0\n\t"   /* auxv[1][0] */
               "push\t(%1)\n\t" /* auxv[0][1] */
               "push\t$31\n\t"  /* auxv[0][0] AT_EXECFN */
               "push\t$0\n\t"   /* envp[0] */
               "sub\t%2,%%rsp\n\t"
               "mov\t%%rsp,%%rdi\n\t"
               "rep movsb\n\t" /* argv */
               "push\t%0\n\t"  /* argc */
               "xor\t%%edi,%%edi\n\t"
               "xor\t%%eax,%%eax\n\t"
               "xor\t%%ebx,%%ebx\n\t"
               "xor\t%%ecx,%%ecx\n\t"
               "xor\t%%edx,%%edx\n\t"
               "xor\t%%edi,%%edi\n\t"
               "xor\t%%esi,%%esi\n\t"
               "xor\t%%ebp,%%ebp\n\t"
               "xor\t%%r8d,%%r8d\n\t"
               "xor\t%%r9d,%%r9d\n\t"
               "xor\t%%r10d,%%r10d\n\t"
               "xor\t%%r11d,%%r11d\n\t"
               "xor\t%%r12d,%%r12d\n\t"
               "xor\t%%r13d,%%r13d\n\t"
               "xor\t%%r14d,%%r14d\n\t"
               "xor\t%%r15d,%%r15d\n\t"
               ".weak\t_start\n\t"
               "jmp\t_start"
               : /* no outputs */
               : "a"(Args), "S"(ArgBlock->Args), "c"((Args + 1) * 8),
                 "m"(x87cw), "r"(pml4t)
               : "memory");

  unreachable;
}
