// clang-format off
// This file contains helper functions for thread-local storage (TLS).
// TLS is probably the most obscure feature the linker has to support,
// so I'll explain it in detail in this comment.
//
// TLS is a per-thread storage. Thread-local variables (TLVs) are in a TLS
// so that each thread has its own set of thread-local variables. Taking
// an address of a TLV returns a unique value for each thread. For example,
// `&foo` for the following code returns different pointer values for
// different threads.
//
//   thread_local int foo;
//
// TLV is a relatively new feature. C for example didn't provide the
// official support for it through the keyword `thread_local` until C11.
// TLV needs a coordination between the compiler, the linker and the
// runtime to work correctly.
//
// An ELF exectuable or a shared library using TLV contains a "TLS template
// image" in the PT_TLS segment. For each newly created thread including the
// initial one, the runtime allocates a contiguous memory for an executable
// and its depending shared libraries and copies template images there. That
// per-thread memory is called the "TLS block". After allocating and
// initializing a TLS block, the runtime sets a register to refer to the TLS
// block, so that the thread-local variables are accessible relative to the
// register.
//
// The register referring to the per-thread storage is called the Thread
// Pointer (TP). TP is part of the thread's context. When the kernel
// scheduler switches threads, TP is saved and restored automatically just
// like other registers are.
//
// The TLS template image is read-only. It contains TLVs' initial values
// for new threads, and no one writes to it at runtime.
//
// Now, let's think about how to access a TLV. We need to know the TLV's
// address to access it which can be done in several different ways as
// follows:
//
//  1. If we are creating an executable, we know the exact size of the TLS
//     template image we are creating, and we know where the TP will be
//     set to after the template is copied to the TLS block. Therefore,
//     the TP-relative address of a TLV in the main executable is known at
//     link-time. That means, computing a TLV's address can be as easy as
//     `add %dst, %tp, <link-time constant>`.
//
//  2. If we are creating a shared library, we don't exactly know where
//     its TLS template image will be copied to in terms of the
//     TP-relative address, because we don't know how large is the main
//     executable's and other libraries' TLS template images are. Only the
//     runtime knows the exact TP-relative address.
//
//     We can solve the problem with an indirection. Specifically, for
//     each TLV whose TP-relative address is only known at process startup
//     time, we create a GOT entry to store its TP-relative address. We
//     also emit a dynamic relocation to let the runtime to fill the GOT
//     entry with a TP-relative address.
//
//     Computing a TLV address in this scheme needs at least two machine
//     instructions in most ISAs; first instruction loads a value from a
//     GOT entry, and the second one adds the loaded value to TP.
//
//  3. Now, think about libraries that you dynamically load with dlopen.
//     The TLS block for such library has to be allocated separately from
//     the initial TLS block, so we now have two or more discontiguous
//     TLS blocks. There's no easy formula to compute an address of a TLV
//     in a separate TLS block.
//
//     The address of a TLV in a separate TLS block can be obtained by
//     calling a libc-provided function, __tls_get_addr(). The function
//     takes two arguments; a module ID to identify the ELF file and the
//     TLV's offset within the ELF file's TLS template image. Accessing a
//     TLV is sometimes compiled to a function call! The module ID and the
//     offset are usually stored to GOT as two consecutive words.
//
// The last access method is the most generic, so the compiler emits such
// code by default. But that's the most expensive one, so the linker
// rewrites instructions if possible so that 3) is relaxed to 2) or even
// to 1).
//
// 1) is called the Local Exec access model. 2) is Initial Exec, and 3) is
// General Dynamic.
//
// There's another little trick that the compiler can use if it knows two
// TLVs are in the same ELF file (usually in the same file as the code is).
// In this case, we can call __tls_get_addr() only once with a module ID and
// the offset 0 to obtain the base address of the ELF file's TLS block. The
// base address obtained this way is sometimes called Dynamic Thread Pointer
// or DTP. We can then compute TLVs' addresses by adding their DTP-relative
// addresses to DTP. This access model is called the Local Dynamic.
//
//
// === TLS Descriptor access model ===
//
// As described above, there are arguably too many different TLS access
// models from the most generic one you can use in any ELF file to the most
// efficient one you can use only when building a main executable. Compiling
// source code with an appropriate TLS access model is bothersome. To solve
// the problem, a new TLS access model was proposed. That is called the TLS
// Descriptor (TLSDESC) model.
//
// For a TLV compiled with TLSDESC, we allocate two consecutive GOT slots
// and create a TLSDESC dynamic relocation for them. The dynamic linker
// sets a function pointer to the first GOT slot and its argument to the
// second slot.
//
// To access the TLV, we call the function pointer with the argument we
// read from the second GOT slot. The function returns the TLV's
// TP-relative address.
//
// The runtime chooses the best access method depending on the situation
// and sets a pointer to the most efficient code to the first GOT slot.
// For example, if a TLV's TP-relative address is known at process startup
// time, the runtime sets that address to the second GOT slot and set a
// function that just returns its argument to the first GOT slot.
//
// With TLSDECS, the compiler can always emit the same code for TLVs
// without sacrificing runtime performance.
//
// TLSDESC is better than the traditional, non-TLSDESC TLS access models.
// It's the default on ARM64, but on other targets, TLSDESC is
// unfortunately either optional or even not supported at all. So we still
// need to support both the traditional TLS models and the TLSDESC model.

#include "third_party/mold/elf/mold.h"

namespace mold::elf {

template <typename E>
static ElfPhdr<E> *get_tls_segment(Context<E> &ctx) {
  if (ctx.phdr)
    for (ElfPhdr<E> &phdr : ctx.phdr->phdrs)
      if (phdr.p_type == PT_TLS)
        return &phdr;
  return nullptr;
}

template <typename E>
u64 get_tls_begin(Context<E> &ctx) {
  if (ElfPhdr<E> *phdr = get_tls_segment(ctx))
    return phdr->p_vaddr;
  return 0;
}

// Returns the TP address which can be used for efficient TLV accesses in
// the main executable. TP at runtime refers to a per-process TLS block
// whose address is not known at link-time. So the address returned from
// this function is the TP if the TLS template image were a TLS block.
template <typename E>
u64 get_tp_addr(Context<E> &ctx) {
  ElfPhdr<E> *phdr = get_tls_segment(ctx);
  if (!phdr)
    return 0;

  // On x86, SPARC and s390x, TP (%gs on i386, %fs on x86-64, %g7 on SPARC
  // and %a0/%a1 on s390x) refers to past the end of the TLS block for
  // historical reasons. TLVs are accessed with negative offsets from TP.
  if constexpr (is_x86<E> || is_sparc<E> || is_s390x<E>)
    return align_to(phdr->p_vaddr + phdr->p_memsz, phdr->p_align);

  // On ARM, SH4 and Alpha, the runtime appends two words at the beginning
  // of TLV template image when copying TLVs to the TLS block, so we need
  // to offset it.
  if constexpr (is_arm<E> || is_sh4<E> || is_alpha<E>)
    return align_down(phdr->p_vaddr - sizeof(Word<E>) * 2, phdr->p_align);

  // On PPC and m68k, TP is 0x7000 (28 KiB) past the beginning of the TLV
  // block to maximize the addressable range for load/store instructions
  // with 16-bits signed immediates. It's not exactly 0x8000 (32 KiB) off
  // because there's a small implementation-defined piece of data before
  // the TLV block, and the runtime wants to access them efficiently too.
  if constexpr (is_ppc<E> || is_m68k<E>)
    return phdr->p_vaddr + 0x7000;

  // RISC-V just uses the beginning of the main executable's TLV block as
  // TP. RISC-V load/store instructions usually take 12-bits signed
  // immediates, so the beginning of TLV ± 2 KiB is accessible with a
  // single load/store instruction.
  assert(is_riscv<E>);
  return phdr->p_vaddr;
}

// Returns the address __tls_get_addr() would return if it's called
// with offset 0.
template <typename E>
u64 get_dtp_addr(Context<E> &ctx) {
  ElfPhdr<E> *phdr = get_tls_segment(ctx);
  if (!phdr)
    return 0;

  // On PPC64 and m68k, R_DTPOFF is resolved to the address 0x8000 (32
  // KiB) past the start of the TLS block. The bias maximizes the
  // accessible range for load/store instructions with 16-bits signed
  // immediates. That is, if the offset were right at the beginning of
  // the start of the TLS block, the half of addressible space (negative
  // immediates) would have been wasted.
  if constexpr (is_ppc<E> || is_m68k<E>)
    return phdr->p_vaddr + 0x8000;

  // On RISC-V, the bias is 0x800 as the load/store instructions in the
  // ISA usually have a 12-bit immediate.
  if constexpr (is_riscv<E>)
    return phdr->p_vaddr + 0x800;

  // On other targets, DTP simply refers to the beginning of the TLS block.
  return phdr->p_vaddr;
}

using E = MOLD_TARGET;

template u64 get_tls_begin<E>(Context<E> &);
template u64 get_tp_addr<E>(Context<E> &);
template u64 get_dtp_addr<E>(Context<E> &);

} // namespace mold::elf
