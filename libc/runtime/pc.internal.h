/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=8 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
╠──────────────────────────────────────────────────────────────────────────────╣
│░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░█▀█░█▀█░▀█▀░█░█░█▀█░█░░░█░░░█░█░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░█▀█░█░▄░░█░░█░█░█▀█░█░░░█░░░▀█▀░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░▀░▀░▀▀▀░░▀░░▀▀▀░▀░▀░▀▀▀░▀▀▀░░▀░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░█▀█░█▀█░█▀█░▀█▀░█▀█░█▀█░█░░░█▀▀░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░█▀▀░█ █░██▀░░█░░█▀█░█▀█░█░░░█▀▀░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░▀░░░▀▀▀░▀░▀░░▀░░▀░▀░▀▀▀░▀▀▀░▀▀▀░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│
│░░░░░░░█▀▀░█░█░█▀▀░█▀█░█░█░▀█▀░█▀█░█▀█░█░░█▀▀░░░░░░░░░░░░░░░░░░░░░░░░▄▄░░░▐█░░│
│░░░░░░░█▀▀░▄▀▄░█▀▀░█░▄░█░█░░█░░█▀█░█▀█░█░░█▀▀░░░░░░░░░░░░▄▄▄░░░▄██▄░░█▀░░░█░▄░│
│░░░░░░░▀▀▀░▀░▀░▀▀▀░▀▀▀░▀▀▀░░▀░░▀░▀░▀▀▀░▀▀░▀▀▀░░░░░░░░░░▄██▀█▌░██▄▄░░▐█▀▄░▐█▀░░│
│░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░▐█▀▀▌░░░▄▀▌░▌░█░▌░░▌░▌░░│
╠──────────────────────────────────────────────────────▌▀▄─▐──▀▄─▐▄─▐▄▐▄─▐▄─▐▄─│
│ αcτµαlly pδrταblε εxεcµταblε § ibm personal computer                         │
╚─────────────────────────────────────────────────────────────────────────────*/
#ifndef COSMOPOLITAN_LIBC_RUNTIME_PC_H_
#define COSMOPOLITAN_LIBC_RUNTIME_PC_H_
#include "libc/runtime/e820.internal.h"
#include "libc/runtime/mman.internal.h"

#define BANE              -140737488355328
#define BOOTSIG           0xaa55 /* master boot record signature */
#define PC_BIOS_DATA_AREA 0x400

#define kInterruptFlag (1u << 9)

/* FPU Status Word (x87)
   @see Intel Manual V1 §8.1.3
   IE: Invalid Operation ────────────────┐
   DE: Denormalized Operand ────────────┐│
   ZE: Zero Divide ────────────────────┐││
   OE: Overflow Flag ─────────────────┐│││
   UE: Underflow Flag ───────────────┐││││
   PE: Precision Flag ──────────────┐│││││
   SF: Stack Fault ────────────────┐││││││
   ES: Exception Summary Status ──┐│││││││
   C0-3: Condition Codes ──┬────┐ ││││││││
   TOP of Stack Pointer ─────┐  │ ││││││││
   B: FPU Busy ───────────┐│ │  │ ││││││││
                          ││┌┴┐┌┼┐││││││││
                          │↓│ │↓↓↓││││││││*/
#define FPU_IE 0b0000000000000000000000001
#define FPU_DE 0b0000000000000000000000010
#define FPU_ZE 0b0000000000000000000000100
#define FPU_OE 0b0000000000000000000001000
#define FPU_UE 0b0000000000000000000010000
#define FPU_PE 0b0000000000000000000100000
#define FPU_SF 0b0000000000000000001000000
#define FPU_C0 0b0000000000000000100000000
#define FPU_C1 0b0000000000000001000000000
#define FPU_C2 0b0000000000000010000000000
#define FPU_C3 0b0000000000100000000000000

#define CR0_PE 0x01       /* protected mode enabled */
#define CR0_MP 0x02       /* monitor coprocessor */
#define CR0_EM 0x04       /* no x87 fpu present if set */
#define CR0_TS 0x08       /* task switched x87 */
#define CR0_ET 0x10       /* extension type 287 or 387 */
#define CR0_NE 0x20       /* enable x87 error reporting */
#define CR0_WP 0x00010000 /* write protect read-only pages @pl0 */
#define CR0_AM 0x00040000 /* alignment mask */
#define CR0_NW 0x20000000 /* global write-through cache disable */
#define CR0_CD 0x40000000 /* global cache disable */
#define CR0_PG 0x80000000 /* paging enabled */

#define CR4_VME        0x01       /* virtual 8086 mode extension */
#define CR4_PVI        0x02       /* protected mode virtual interrupts */
#define CR4_TSD        0x04       /* time stamp disable (rdtsc) */
#define CR4_DE         0x08       /* debugging extensions */
#define CR4_PSE        0x10       /* page size extension */
#define CR4_PAE        0x20       /* physical address extension */
#define CR4_MCE        0x40       /* machine check exception */
#define CR4_PGE        0x80       /* page global enabled */
#define CR4_OSFXSR     0x0200     /* enable SSE and fxsave/fxrestor */
#define CR4_OSXMMEXCPT 0x0400     /* enable unmasked SSE exceptions */
#define CR4_LA57       0x1000     /* enable level-5 paging */
#define CR4_VMXE       0x2000     /* enable VMX operations */
#define CR4_SMXE       0x4000     /* enable SMX operations */
#define CR4_FSGSBASE   0x00010000 /* enable *FSBASE and *GSBASE instructions */
#define CR4_PCIDE      0x00020000 /* enable process-context identifiers */
#define CR4_OSXSAVE    0x00040000 /* enable XSAVE */

#define XCR0_X87       0x01
#define XCR0_SSE       0x02
#define XCR0_AVX       0x04
#define XCR0_BNDREG    0x08
#define XCR0_BNDCSR    0x10
#define XCR0_OPMASK    0x20
#define XCR0_ZMM_HI256 0x40
#define XCR0_HI16_ZMM  0x80

#define EFER     0xc0000080 /* extended feature enable register */
#define EFER_SCE 0x01       /* system call extensions */
#define EFER_LME 0x0100     /* long mode enable */
#define EFER_LMA 0x0400     /* long mode active */
#define EFER_NXE 0x0800     /* no-execute enable */

#define GDT_REAL_CODE   8
#define GDT_REAL_DATA   16
#define GDT_LEGACY_CODE 24
#define GDT_LEGACY_DATA 32
#define GDT_LONG_CODE   40
#define GDT_LONG_DATA   48
#define GDT_LONG_TSS    56

#define PIC1         0x20 /* IO base address for master PIC */
#define PIC2         0xA0 /* IO base address for slave PIC */
#define PIC1_CMD     PIC1
#define PIC1_DATA    (PIC1 + 1)
#define PIC2_CMD     PIC2
#define PIC2_DATA    (PIC2 + 1)
#define PIC_EOI      0x20 /* End-of-interrupt command code */
#define PIC_READ_IRR 0x0a /* OCW3 irq ready next CMD read */
#define PIC_READ_ISR 0x0b /* OCW3 irq service next CMD read */

/* Long Mode Paging
   @see Intel Manual V.3A §4.1 §4.5
                                          IsValid (ignored on CR3) V┐
     ┌XD:No Inst. Fetches (if NXE)   IsWritable (ignored on CR3) RW┐│
     │                                 Permit User-Mode Access - u┐││
     │                             Page-level Write-Through - PWT┐│││
     │                            Page-level Cache Disable - PCD┐││││
     │                          Set if has been read - Accessed┐│││││
     │                         Set if has been written - Dirty┐││││││
     │           IsPage 2MB/1GB (if PDPTE/PDE) or PAT (if PT)┐│││││││
     │                (If this maps page and CR4.PGE) Global┐││││││││
     │      (If IsPage 2MB/1GB, see Intel V3A § 11.12) PAT  │││││││││
     │                                                  │   │││││││││
     │            ┌─────────────────────────────────────┤   │││││││││
     │  Must Be 0┐│ Next Page Table Address (!IsPage)   │   │││││││││
     │           │├─────────────────────────────────────┤   │││││││││
     │           ││ Physical Address 4KB                │   │││││││││
     │┌──┐┌─────┐│├────────────────────────────┐        │ign│││││││││
     ││PK││ ign │││ Physical Address 2MB       │        │┌┴┐│││││││││
     ││  ││     ││├───────────────────┐        │        ││ ││││││││││
     ││  ││     │││ Phys. Addr. 1GB   │        │        ││ ││││││││││
     ││  ││     │││                   │        │        ││ ││││││││││
     6666555555555544444444443333333333222222222211111111110000000000
     3210987654321098765432109876543210987654321098765432109876543210*/
#define PAGE_V    /*                                */ 0b000000000001
#define PAGE_RW   /*                                */ 0b000000000010
#define PAGE_U    /*                                */ 0b000000000100
#define PAGE_PCD  /*                                */ 0b000000010000
#define PAGE_PS   /*                                */ 0b000010000000
#define PAGE_G    /*                                */ 0b000100000000
#define PAGE_IGN1 /*                                */ 0b111000000000
#define PAGE_RSRV /* blinkenlights/libc reservation */ 0b001000000000
#define PAGE_GROD /* blinkenlights MAP_GROWSDOWN    */ 0b010000000000
#define PAGE_TA   0x00007ffffffff000
#define PAGE_PA2  0x00007fffffe00000
#define PAGE_IGN2 0x07f0000000000000
#define PAGE_REFC PAGE_IGN2          /* libc reference counting */
#define PAGE_1REF 0x0010000000000000 /* libc reference counting */
#define PAGE_XD   0x8000000000000000

#if !(__ASSEMBLER__ + __LINKER__ + 0)

#define invlpg(p) asm volatile("invlpg\t(%0)" : : "r"(p) : "memory")

struct IdtDescriptor {
  uint16_t offset_1; /* offset bits 0..15 */
  uint16_t selector; /* a code segment selector in GDT or LDT */
  uint8_t ist;       /* bits 0..2 hold stack table offset, rest are zero */
  uint8_t type_attr; /* type and attributes */
  uint16_t offset_2; /* offset bits 16..31 */
  uint32_t offset_3; /* offset bits 32..63 */
  uint32_t zero;     /* reserved */
};

uint64_t *__get_virtual(struct mman *, uint64_t *, int64_t, bool);
uint64_t __clear_page(uint64_t);
uint64_t __new_page(struct mman *);
uint64_t *__invert_memory_area(struct mman *, uint64_t *, uint64_t, uint64_t,
                               uint64_t);
void __map_phdrs(struct mman *, uint64_t *, uint64_t, uint64_t);
void __reclaim_boot_pages(struct mman *, uint64_t, uint64_t);
void __ref_page(struct mman *, uint64_t *, uint64_t);
void __ref_pages(struct mman *, uint64_t *, uint64_t, uint64_t);
void __unref_page(struct mman *, uint64_t *, uint64_t);

/**
 * Identity maps an area of physical memory to its negative address and
 * marks it as permanently referenced and unreclaimable (so that it will
 * never be added to the free list).  This is useful for special-purpose
 * physical memory regions, such as video frame buffers and memory-mapped
 * I/O devices.
 */
forceinline void __invert_and_perm_ref_memory_area(struct mman *mm,
                                                  uint64_t *pml4t, uint64_t ps,
                                                  uint64_t size,
                                                  uint64_t pte_flags) {
  __invert_memory_area(mm, pml4t, ps, size, pte_flags | PAGE_REFC);
}

forceinline unsigned char inb(unsigned short port) {
  unsigned char al;
  asm volatile("inb\t%1,%0" : "=a"(al) : "dN"(port));
  return al;
}

forceinline void outb(unsigned short port, unsigned char byte) {
  asm volatile("outb\t%0,%1"
               : /* no inputs */
               : "a"(byte), "dN"(port));
}

#define __clear_page(page)                               \
  ({                                                     \
    long di, cx;                                         \
    uintptr_t Page = (uintptr_t)(page);                  \
    asm("rep stosb"                                      \
        : "=D"(di), "=c"(cx), "=m"(*(char(*)[4096])Page) \
        : "0"(Page), "1"(4096), "a"(0));                 \
    Page;                                                \
  })

#define __get_pml4t()                 \
  ({                                  \
    intptr_t cr3;                     \
    asm("mov\t%%cr3,%0" : "=r"(cr3)); \
    (uint64_t *)(BANE + cr3);         \
  })

#define __get_mm()     ((struct mman *)(BANE + 0x0500))
#define __get_mm_phy() ((struct mman *)0x0500)

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_PC_H_ */
