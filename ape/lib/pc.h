/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=8 sts=2 sw=2 fenc=utf-8                                :vi│
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
#ifndef APE_LIB_PC_H_
#define APE_LIB_PC_H_

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
#define FPU_IE 0b0000000000100000000000001
#define FPU_ZE 0b0000000000100000000000100
#define FPU_SF 0b0000000000000000001000000
#define FPU_C0 0b0000000000000000100000000
#define FPU_C1 0b0000000000000001000000000
#define FPU_C2 0b0000000000000010000000000
#define FPU_C3 0b0000000000100000000000000

#define CR0_PE (1u << 0)  /* protected mode enabled */
#define CR0_MP (1u << 1)  /* monitor coprocessor */
#define CR0_EM (1u << 2)  /* no x87 fpu present if set */
#define CR0_TS (1u << 3)  /* task switched x87 */
#define CR0_ET (1u << 4)  /* extension type 287 or 387 */
#define CR0_NE (1u << 5)  /* enable x87 error reporting */
#define CR0_WP (1u << 16) /* write protect read-only pages @pl0 */
#define CR0_AM (1u << 18) /* alignment mask */
#define CR0_NW (1u << 29) /* global write-through cache disable */
#define CR0_CD (1u << 30) /* global cache disable */
#define CR0_PG (1u << 31) /* paging enabled */

#define CR4_VME        (1u << 0)  /* virtual 8086 mode extension */
#define CR4_PVI        (1u << 1)  /* protected mode virtual interrupts */
#define CR4_TSD        (1u << 2)  /* time stamp disable (rdtsc) */
#define CR4_DE         (1u << 3)  /* debugging extensions */
#define CR4_PSE        (1u << 4)  /* page size extension */
#define CR4_PAE        (1u << 5)  /* physical address extension */
#define CR4_MCE        (1u << 6)  /* machine check exception */
#define CR4_PGE        (1u << 7)  /* page global enabled */
#define CR4_OSFXSR     (1u << 9)  /* enable SSE and fxsave/fxrestor */
#define CR4_OSXMMEXCPT (1u << 10) /* enable unmasked SSE exceptions */
#define CR4_LA57       (1u << 12) /* enable level-5 paging */
#define CR4_VMXE       (1u << 13) /* enable VMX operations */
#define CR4_SMXE       (1u << 14) /* enable SMX operations */
#define CR4_FSGSBASE   (1u << 16) /* enable *FSBASE and *GSBASE instructions */
#define CR4_PCIDE      (1u << 17) /* enable process-context identifiers */
#define CR4_OSXSAVE    (1u << 18) /* enable XSAVE */

#define XCR0_X87       (1u << 0)
#define XCR0_SSE       (1u << 1)
#define XCR0_AVX       (1u << 2)
#define XCR0_BNDREG    (1u << 3)
#define XCR0_BNDCSR    (1u << 4)
#define XCR0_OPMASK    (1u << 5)
#define XCR0_ZMM_HI256 (1u << 6)
#define XCR0_HI16_ZMM  (1u << 7)

#define EFER     0xC0000080 /* extended feature enable register */
#define EFER_SCE (1u << 0)  /* system call extensions */
#define EFER_LME (1u << 8)  /* long mode enable */
#define EFER_LMA (1u << 10) /* long mode active */
#define EFER_NXE (1u << 11) /* no-execute enable */

#define GDT_REAL_CODE   8
#define GDT_REAL_DATA   16
#define GDT_LEGACY_CODE 24
#define GDT_LEGACY_DATA 32
#define GDT_LONG_CODE   40
#define GDT_LONG_DATA   48

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
     │                   IsPage (if PDPTE/PDE) or PAT (if PT)┐│││││││
     │        (If this maps 2MB/1GB page and CR4.PGE) Global┐││││││││
     │      (If IsPage 2MB/1GB, see Intel V3A § 11.12) PAT  │││││││││
     │                                                  │   │││││││││
     │             ┌────────────────────────────────────┤   │││││││││
     │   Must Be 0┐│ Next Page Table Address (!IsPage)  │   │││││││││
     │            │├────────────────────────────────────┤   │││││││││
     │            ││ Physical Address 4KB               │   │││││││││
     │┌───┐┌─────┐│├───────────────────────────┐        │ign│││││││││
     ││PKE││ ign │││ Physical Address 2MB      │        │┌┴┐│││││││││
     ││   ││     ││├──────────────────┐        │        ││ ││││││││││
     ││   ││     │││ Phys. Addr. 1GB  │        │        ││ ││││││││││
     ││   ││     │││                  │        │        ││ ││││││││││
     6666555555555544444444443333333333222222222211111111110000000000
     3210987654321098765432109876543210987654321098765432109876543210*/
#define PAGE_V   /*                                    */ 0b000000001
#define PAGE_RW  /*                                    */ 0b000000010
#define PAGE_U   /*                                    */ 0b000000100
#define PAGE_4KB /*                                    */ 0b010000000
#define PAGE_2MB /*                                    */ 0b110000000
#define PAGE_1GB /*                                    */ 0b110000000
#define PAGE_TA  0b11111111111111111111111111111111111111000000000000
#define PAGE_PA2 0b11111111111111111111111111111000000000000000000000
#define PAGE_XD  0x8000000000000000

#if !(__ASSEMBLER__ + __LINKER__ + 0)
#include "ape/config.h"

struct thatispacked GlobalDescriptorTable {
  uint16_t size;
  uint64_t *entries;
};

/**
 * Memory hole map.
 * @see wiki.osdev.org/Detecting_Memory_(x86)
 * @since 2002
 */
struct SmapEntry {
  uint64_t addr;
  uint64_t size;
  enum {
    kMemoryUsable = 1,
    kMemoryUnusable = 2,
    kMemoryAcpiReclaimable = 3,
    kMemoryAcpiNvs = 4,
    kMemoryBad = 5
  } type;
  uint32_t __acpi3; /* is abstracted */
};

struct IdtDescriptor {
  uint16_t offset_1; /* offset bits 0..15 */
  uint16_t selector; /* a code segment selector in GDT or LDT */
  uint8_t ist;       /* bits 0..2 hold stack table offset, rest are zero */
  uint8_t type_attr; /* type and attributes */
  uint16_t offset_2; /* offset bits 16..31 */
  uint32_t offset_3; /* offset bits 32..63 */
  uint32_t zero;     /* reserved */
};

struct thatispacked PageTable {
  uint64_t p[512];
} forcealign(PAGESIZE);

extern struct PageTable g_pml4t;
extern struct GlobalDescriptorTable gdt;

extern const unsigned char kBiosDataArea[256];
extern const unsigned char kBiosDataAreaXlm[256];

extern struct SmapEntry e820map[XLM_E820_SIZE / sizeof(struct SmapEntry)];
extern struct SmapEntry e820map_xlm[XLM_E820_SIZE / sizeof(struct SmapEntry)];

extern uint64_t g_ptsp;
extern uint64_t g_ptsp_xlm;

void bootdr(char drive) wontreturn;

void smapsort(struct SmapEntry *);
uint64_t *__getpagetableentry(int64_t, unsigned, struct PageTable *,
                              uint64_t *);
void flattenhighmemory(struct SmapEntry *, struct PageTable *, uint64_t *);
void pageunmap(int64_t);

forceinline unsigned long eflags(void) {
  unsigned long res;
  asm("pushf\n\t"
      "pop\t%0"
      : "=rm"(res));
  return res;
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

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* APE_LIB_PC_H_ */
