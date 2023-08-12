// clang-format off
// Supporting x86-64 is straightforward. Unlike its predecessor, i386,
// x86-64 supports PC-relative addressing for position-independent code.
// Being CISC, its instructions are variable in size. Branch instructions
// take 4 bytes offsets, so we don't need range extension thunks.
//
// The psABI specifies %r11 as neither caller- nor callee-saved. It's
// intentionally left out so that we can use it as a scratch register in
// PLT.
//
// Thread Pointer (TP) is stored not to a general-purpose register but to
// FS segment register. Segment register is a 64-bits register which can
// be used as a base address for memory access. Each thread has a unique
// FS value, and they access their thread-local variables relative to FS
// as %fs:offset_from_tp.
//
// The value of a segment register itself is not generally readable from
// the user space. As a workaround, libc initializes %fs:0 (the first word
// referenced by FS) to the value of %fs itself. So we can obtain TP just
// by `mov %fs:0, %rax` if we need it.
//
// For historical reasons, TP points past the end of the TLS block on x86.
// This is contrary to other psABIs which usually use the beginning of the
// TLS block as TP (with some addend). As a result, offsets from TP to
// thread-local variables (TLVs) in the main executable are all negative.
//
// https://github.com/rui314/psabi/blob/main/x86-64.pdf
// https://github.com/rui314/psabi/blob/main/i386.pdf

#include "third_party/mold/elf/mold.h"

namespace mold::elf {

using E = X86_64;

// This is a security-enhanced version of the regular PLT. The PLT
// header and each PLT entry starts with endbr64 for the Intel's
// control-flow enforcement security mechanism.
//
// Note that our IBT-enabled PLT instruction sequence is different
// from the one used in GNU ld. GNU's IBTPLT implementation uses two
// separate sections (.plt and .plt.sec) in which one PLT entry takes
// 32 bytes in total. Our IBTPLT consists of just .plt and each entry
// is 16 bytes long.
//
// Our PLT entry clobbers %r11, but that's fine because the resolver
// function (_dl_runtime_resolve) clobbers %r11 anyway.
template <>
void write_plt_header(Context<E> &ctx, u8 *buf) {
  static const u8 insn[] = {
    0xf3, 0x0f, 0x1e, 0xfa, // endbr64
    0x41, 0x53,             // push %r11
    0xff, 0x35, 0, 0, 0, 0, // push GOTPLT+8(%rip)
    0xff, 0x25, 0, 0, 0, 0, // jmp *GOTPLT+16(%rip)
    0xcc, 0xcc, 0xcc, 0xcc, // (padding)
    0xcc, 0xcc, 0xcc, 0xcc, // (padding)
    0xcc, 0xcc, 0xcc, 0xcc, // (padding)
    0xcc, 0xcc,             // (padding)
  };

  memcpy(buf, insn, sizeof(insn));
  *(ul32 *)(buf + 8) = ctx.gotplt->shdr.sh_addr - ctx.plt->shdr.sh_addr - 4;
  *(ul32 *)(buf + 14) = ctx.gotplt->shdr.sh_addr - ctx.plt->shdr.sh_addr - 2;
}

template <>
void write_plt_entry(Context<E> &ctx, u8 *buf, Symbol<E> &sym) {
  static const u8 insn[] = {
    0xf3, 0x0f, 0x1e, 0xfa, // endbr64
    0x41, 0xbb, 0, 0, 0, 0, // mov $index_in_relplt, %r11d
    0xff, 0x25, 0, 0, 0, 0, // jmp *foo@GOTPLT
  };

  memcpy(buf, insn, sizeof(insn));
  *(ul32 *)(buf + 6) = sym.get_plt_idx(ctx);
  *(ul32 *)(buf + 12) = sym.get_gotplt_addr(ctx) - sym.get_plt_addr(ctx) - 16;
}

template <>
void write_pltgot_entry(Context<E> &ctx, u8 *buf, Symbol<E> &sym) {
  static const u8 insn[] = {
    0xf3, 0x0f, 0x1e, 0xfa, // endbr64
    0xff, 0x25, 0, 0, 0, 0, // jmp *foo@GOT
    0xcc, 0xcc, 0xcc, 0xcc, // (padding)
    0xcc, 0xcc,             // (padding)
  };

  memcpy(buf, insn, sizeof(insn));
  *(ul32 *)(buf + 6) = sym.get_got_addr(ctx) - sym.get_plt_addr(ctx) - 10;
}

template <>
void EhFrameSection<E>::apply_reloc(Context<E> &ctx, const ElfRel<E> &rel,
                                    u64 offset, u64 val) {
  u8 *loc = ctx.buf + this->shdr.sh_offset + offset;

  switch (rel.r_type) {
  case R_NONE:
    break;
  case R_X86_64_32:
    *(ul32 *)loc = val;
    break;
  case R_X86_64_64:
    *(ul64 *)loc = val;
    break;
  case R_X86_64_PC32:
    *(ul32 *)loc = val - this->shdr.sh_addr - offset;
    break;
  case R_X86_64_PC64:
    *(ul64 *)loc = val - this->shdr.sh_addr - offset;
    break;
  default:
    Fatal(ctx) << "unsupported relocation in .eh_frame: " << rel;
  }
}

static u32 relax_gotpcrelx(u8 *loc) {
  switch ((loc[0] << 8) | loc[1]) {
  case 0xff15: return 0x90e8; // call *0(%rip) -> call 0
  case 0xff25: return 0x90e9; // jmp  *0(%rip) -> jmp  0
  }
  return 0;
}

static u32 relax_rex_gotpcrelx(u8 *loc) {
  switch ((loc[0] << 16) | (loc[1] << 8) | loc[2]) {
  case 0x488b05: return 0x488d05; // mov 0(%rip), %rax -> lea 0(%rip), %rax
  case 0x488b0d: return 0x488d0d; // mov 0(%rip), %rcx -> lea 0(%rip), %rcx
  case 0x488b15: return 0x488d15; // mov 0(%rip), %rdx -> lea 0(%rip), %rdx
  case 0x488b1d: return 0x488d1d; // mov 0(%rip), %rbx -> lea 0(%rip), %rbx
  case 0x488b25: return 0x488d25; // mov 0(%rip), %rsp -> lea 0(%rip), %rsp
  case 0x488b2d: return 0x488d2d; // mov 0(%rip), %rbp -> lea 0(%rip), %rbp
  case 0x488b35: return 0x488d35; // mov 0(%rip), %rsi -> lea 0(%rip), %rsi
  case 0x488b3d: return 0x488d3d; // mov 0(%rip), %rdi -> lea 0(%rip), %rdi
  case 0x4c8b05: return 0x4c8d05; // mov 0(%rip), %r8  -> lea 0(%rip), %r8
  case 0x4c8b0d: return 0x4c8d0d; // mov 0(%rip), %r9  -> lea 0(%rip), %r9
  case 0x4c8b15: return 0x4c8d15; // mov 0(%rip), %r10 -> lea 0(%rip), %r10
  case 0x4c8b1d: return 0x4c8d1d; // mov 0(%rip), %r11 -> lea 0(%rip), %r11
  case 0x4c8b25: return 0x4c8d25; // mov 0(%rip), %r12 -> lea 0(%rip), %r12
  case 0x4c8b2d: return 0x4c8d2d; // mov 0(%rip), %r13 -> lea 0(%rip), %r13
  case 0x4c8b35: return 0x4c8d35; // mov 0(%rip), %r14 -> lea 0(%rip), %r14
  case 0x4c8b3d: return 0x4c8d3d; // mov 0(%rip), %r15 -> lea 0(%rip), %r15
  }
  return 0;
}

static u32 relax_gottpoff(u8 *loc) {
  switch ((loc[0] << 16) | (loc[1] << 8) | loc[2]) {
  case 0x488b05: return 0x48c7c0; // mov 0(%rip), %rax -> mov $0, %rax
  case 0x488b0d: return 0x48c7c1; // mov 0(%rip), %rcx -> mov $0, %rcx
  case 0x488b15: return 0x48c7c2; // mov 0(%rip), %rdx -> mov $0, %rdx
  case 0x488b1d: return 0x48c7c3; // mov 0(%rip), %rbx -> mov $0, %rbx
  case 0x488b25: return 0x48c7c4; // mov 0(%rip), %rsp -> mov $0, %rsp
  case 0x488b2d: return 0x48c7c5; // mov 0(%rip), %rbp -> mov $0, %rbp
  case 0x488b35: return 0x48c7c6; // mov 0(%rip), %rsi -> mov $0, %rsi
  case 0x488b3d: return 0x48c7c7; // mov 0(%rip), %rdi -> mov $0, %rdi
  case 0x4c8b05: return 0x49c7c0; // mov 0(%rip), %r8  -> mov $0, %r8
  case 0x4c8b0d: return 0x49c7c1; // mov 0(%rip), %r9  -> mov $0, %r9
  case 0x4c8b15: return 0x49c7c2; // mov 0(%rip), %r10 -> mov $0, %r10
  case 0x4c8b1d: return 0x49c7c3; // mov 0(%rip), %r11 -> mov $0, %r11
  case 0x4c8b25: return 0x49c7c4; // mov 0(%rip), %r12 -> mov $0, %r12
  case 0x4c8b2d: return 0x49c7c5; // mov 0(%rip), %r13 -> mov $0, %r13
  case 0x4c8b35: return 0x49c7c6; // mov 0(%rip), %r14 -> mov $0, %r14
  case 0x4c8b3d: return 0x49c7c7; // mov 0(%rip), %r15 -> mov $0, %r15
  }
  return 0;
}

static u32 relax_gotpc32_tlsdesc(u8 *loc) {
  switch ((loc[0] << 16) | (loc[1] << 8) | loc[2]) {
  case 0x488d05: return 0x48c7c0; // lea 0(%rip), %rax -> mov $0, %rax
  case 0x488d0d: return 0x48c7c1; // lea 0(%rip), %rcx -> mov $0, %rcx
  case 0x488d15: return 0x48c7c2; // lea 0(%rip), %rdx -> mov $0, %rdx
  case 0x488d1d: return 0x48c7c3; // lea 0(%rip), %rbx -> mov $0, %rbx
  case 0x488d25: return 0x48c7c4; // lea 0(%rip), %rsp -> mov $0, %rsp
  case 0x488d2d: return 0x48c7c5; // lea 0(%rip), %rbp -> mov $0, %rbp
  case 0x488d35: return 0x48c7c6; // lea 0(%rip), %rsi -> mov $0, %rsi
  case 0x488d3d: return 0x48c7c7; // lea 0(%rip), %rdi -> mov $0, %rdi
  case 0x4c8d05: return 0x49c7c0; // lea 0(%rip), %r8  -> mov $0, %r8
  case 0x4c8d0d: return 0x49c7c1; // lea 0(%rip), %r9  -> mov $0, %r9
  case 0x4c8d15: return 0x49c7c2; // lea 0(%rip), %r10 -> mov $0, %r10
  case 0x4c8d1d: return 0x49c7c3; // lea 0(%rip), %r11 -> mov $0, %r11
  case 0x4c8d25: return 0x49c7c4; // lea 0(%rip), %r12 -> mov $0, %r12
  case 0x4c8d2d: return 0x49c7c5; // lea 0(%rip), %r13 -> mov $0, %r13
  case 0x4c8d35: return 0x49c7c6; // lea 0(%rip), %r14 -> mov $0, %r14
  case 0x4c8d3d: return 0x49c7c7; // lea 0(%rip), %r15 -> mov $0, %r15
  }
  return 0;
}

// Rewrite a function call to __tls_get_addr to a cheaper instruction
// sequence. We can do this when we know the thread-local variable's TP-
// relative address at link-time.
static void relax_gd_to_le(u8 *loc, ElfRel<E> rel, u64 val) {
  switch (rel.r_type) {
  case R_X86_64_PLT32:
  case R_X86_64_PC32:
  case R_X86_64_GOTPCREL:
  case R_X86_64_GOTPCRELX: {
    // The original instructions are the following:
    //
    //  66 48 8d 3d 00 00 00 00    lea  foo@tlsgd(%rip), %rdi
    //  66 66 48 e8 00 00 00 00    call __tls_get_addr
    //
    // or
    //
    //  66 48 8d 3d 00 00 00 00    lea foo@tlsgd(%rip), %rdi
    //  66 48 ff 15 00 00 00 00    call *__tls_get_addr@GOT(%rip)
    static const u8 insn[] = {
      0x64, 0x48, 0x8b, 0x04, 0x25, 0, 0, 0, 0, // mov %fs:0, %rax
      0x48, 0x81, 0xc0, 0, 0, 0, 0,             // add $tp_offset, %rax
    };
    memcpy(loc - 4, insn, sizeof(insn));
    *(ul32 *)(loc + 8) = val;
    break;
  }
  case R_X86_64_PLTOFF64: {
    // The original instructions are the following:
    //
    //  48 8d 3d 00 00 00 00           lea    foo@tlsgd(%rip), %rdi
    //  48 b8 00 00 00 00 00 00 00 00  movabs __tls_get_addr, %rax
    //  48 01 d8                       add    %rbx, %rax
    //  ff d0                          call   *%rax
    static const u8 insn[] = {
      0x64, 0x48, 0x8b, 0x04, 0x25, 0, 0, 0, 0, // mov %fs:0, %rax
      0x48, 0x81, 0xc0, 0, 0, 0, 0,             // add $tp_offset, %rax
      0x66, 0x0f, 0x1f, 0x44, 0x00, 0x00,       // nop
    };
    memcpy(loc - 3, insn, sizeof(insn));
    *(ul32 *)(loc + 9) = val;
    break;
  }
  default:
    unreachable();
  }
}

static void relax_gd_to_ie(u8 *loc, ElfRel<E> rel, u64 val) {
  switch (rel.r_type) {
  case R_X86_64_PLT32:
  case R_X86_64_PC32:
  case R_X86_64_GOTPCREL:
  case R_X86_64_GOTPCRELX: {
    static const u8 insn[] = {
      0x64, 0x48, 0x8b, 0x04, 0x25, 0, 0, 0, 0, // mov %fs:0, %rax
      0x48, 0x03, 0x05, 0, 0, 0, 0,             // add foo@gottpoff(%rip), %rax
    };
    memcpy(loc - 4, insn, sizeof(insn));
    *(ul32 *)(loc + 8) = val - 12;
    break;
  }
  case R_X86_64_PLTOFF64: {
    static const u8 insn[] = {
      0x64, 0x48, 0x8b, 0x04, 0x25, 0, 0, 0, 0, // mov %fs:0, %rax
      0x48, 0x03, 0x05, 0, 0, 0, 0,             // add foo@gottpoff(%rip), %rax
      0x66, 0x0f, 0x1f, 0x44, 0x00, 0x00,       // nop
    };
    memcpy(loc - 3, insn, sizeof(insn));
    *(ul32 *)(loc + 9) = val - 13;
    break;
  }
  default:
    unreachable();
  }
}

// Rewrite a function call to __tls_get_addr to a cheaper instruction
// sequence. The difference from relax_gd_to_le is that we are
// materializing a Dynamic Thread Pointer for the current ELF module
// instead of an address for a particular thread-local variable.
static void relax_ld_to_le(u8 *loc, ElfRel<E> rel, u64 val) {
  switch (rel.r_type) {
  case R_X86_64_PLT32:
  case R_X86_64_PC32: {
    // The original instructions are the following:
    //
    //  48 8d 3d 00 00 00 00    lea    foo@tlsld(%rip), %rdi
    //  e8 00 00 00 00          call   __tls_get_addr
    static const u8 insn[] = {
      0x31, 0xc0,                   // xor %eax, %eax
      0x64, 0x48, 0x8b, 0x00,       // mov %fs:(%rax), %rax
      0x48, 0x2d, 0, 0, 0, 0,       // sub $tls_size, %rax
    };
    memcpy(loc - 3, insn, sizeof(insn));
    *(ul32 *)(loc + 5) = val;
    break;
  }
  case R_X86_64_GOTPCREL:
  case R_X86_64_GOTPCRELX: {
    // The original instructions are the following:
    //
    //  48 8d 3d 00 00 00 00    lea    foo@tlsld(%rip), %rdi
    //  ff 15 00 00 00 00       call   *__tls_get_addr@GOT(%rip)
    static const u8 insn[] = {
      0x31, 0xc0,                   // xor %eax, %eax
      0x64, 0x48, 0x8b, 0x00,       // mov %fs:(%rax), %rax
      0x48, 0x2d, 0, 0, 0, 0,       // sub $tls_size, %rax
      0x90,                         // nop
    };
    memcpy(loc - 3, insn, sizeof(insn));
    *(ul32 *)(loc + 5) = val;
    break;
  }
  case R_X86_64_PLTOFF64: {
    // The original instructions are the following:
    //
    //  48 8d 3d 00 00 00 00           lea    foo@tlsld(%rip), %rdi
    //  48 b8 00 00 00 00 00 00 00 00  movabs __tls_get_addr@GOTOFF, %rax
    //  48 01 d8                       add    %rbx, %rax
    //  ff d0                          call   *%rax
    static const u8 insn[] = {
      0x31, 0xc0,                   // xor %eax, %eax
      0x64, 0x48, 0x8b, 0x00,       // mov %fs:(%rax), %rax
      0x48, 0x2d, 0, 0, 0, 0,       // sub $tls_size, %rax
      0x0f, 0x1f, 0x44, 0x00, 0x00, // nop
      0x0f, 0x1f, 0x44, 0x00, 0x00, // nop
    };
    memcpy(loc - 3, insn, sizeof(insn));
    *(ul32 *)(loc + 5) = val;
    break;
  }
  default:
    unreachable();
  }
}

// Apply relocations to SHF_ALLOC sections (i.e. sections that are
// mapped to memory at runtime) based on the result of
// scan_relocations().
template <>
void InputSection<E>::apply_reloc_alloc(Context<E> &ctx, u8 *base) {
  std::span<const ElfRel<E>> rels = get_rels(ctx);

  ElfRel<E> *dynrel = nullptr;
  if (ctx.reldyn)
    dynrel = (ElfRel<E> *)(ctx.buf + ctx.reldyn->shdr.sh_offset +
                           file.reldyn_offset + this->reldyn_offset);

  for (i64 i = 0; i < rels.size(); i++) {
    const ElfRel<E> &rel = rels[i];
    if (rel.r_type == R_NONE)
      continue;

    Symbol<E> &sym = *file.symbols[rel.r_sym];
    u8 *loc = base + rel.r_offset;

    auto check = [&](i64 val, i64 lo, i64 hi) {
      if (val < lo || hi <= val)
        Error(ctx) << *this << ": relocation " << rel << " against "
                   << sym << " out of range: " << val << " is not in ["
                   << lo << ", " << hi << ")";
    };

    auto write32 = [&](u64 val) {
      check(val, 0, 1LL << 32);
      *(ul32 *)loc = val;
    };

    auto write32s = [&](u64 val) {
      check(val, -(1LL << 31), 1LL << 31);
      *(ul32 *)loc = val;
    };

    u64 S = sym.get_addr(ctx);
    u64 A = rel.r_addend;
    u64 P = get_addr() + rel.r_offset;
    u64 G = sym.get_got_addr(ctx) - ctx.gotplt->shdr.sh_addr;
    u64 GOTPLT = ctx.gotplt->shdr.sh_addr;

    switch (rel.r_type) {
    case R_X86_64_8:
      check(S + A, 0, 1 << 8);
      *loc = S + A;
      break;
    case R_X86_64_16:
      check(S + A, 0, 1 << 16);
      *(ul16 *)loc = S + A;
      break;
    case R_X86_64_32:
      write32(S + A);
      break;
    case R_X86_64_32S:
      write32s(S + A);
      break;
    case R_X86_64_64:
      apply_dyn_absrel(ctx, sym, rel, loc, S, A, P, dynrel);
      break;
    case R_X86_64_PC8:
      check(S + A - P, -(1 << 7), 1 << 7);
      *loc = S + A - P;
      break;
    case R_X86_64_PC16:
      check(S + A - P, -(1 << 15), 1 << 15);
      *(ul16 *)loc = S + A - P;
      break;
    case R_X86_64_PC32:
    case R_X86_64_PLT32:
      write32s(S + A - P);
      break;
    case R_X86_64_PC64:
      *(ul64 *)loc = S + A - P;
      break;
    case R_X86_64_GOT32:
      write32s(G + A);
      break;
    case R_X86_64_GOT64:
      *(ul64 *)loc = G + A;
      break;
    case R_X86_64_GOTOFF64:
    case R_X86_64_PLTOFF64:
      *(ul64 *)loc = S + A - GOTPLT;
      break;
    case R_X86_64_GOTPC32:
      write32s(GOTPLT + A - P);
      break;
    case R_X86_64_GOTPC64:
      *(ul64 *)loc = GOTPLT + A - P;
      break;
    case R_X86_64_GOTPCREL:
      write32s(G + GOTPLT + A - P);
      break;
    case R_X86_64_GOTPCREL64:
      *(ul64 *)loc = G + GOTPLT + A - P;
      break;
    case R_X86_64_GOTPCRELX:
      // We always want to relax GOTPCRELX relocs even if --no-relax
      // was given because some static PIE runtime code depends on these
      // relaxations.
      if (!sym.is_imported && !sym.is_ifunc() && sym.is_relative()) {
        u32 insn = relax_gotpcrelx(loc - 2);
        i64 val = S + A - P;
        if (insn && (i32)val == val) {
          loc[-2] = insn >> 8;
          loc[-1] = insn;
          *(ul32 *)loc = val;
          break;
        }
      }
      write32s(G + GOTPLT + A - P);
      break;
    case R_X86_64_REX_GOTPCRELX:
      if (!sym.is_imported && !sym.is_ifunc() && sym.is_relative()) {
        u32 insn = relax_rex_gotpcrelx(loc - 3);
        i64 val = S + A - P;
        if (insn && (i32)val == val) {
          loc[-3] = insn >> 16;
          loc[-2] = insn >> 8;
          loc[-1] = insn;
          *(ul32 *)loc = val;
          break;
        }
      }
      write32s(G + GOTPLT + A - P);
      break;
    case R_X86_64_TLSGD:
      if (sym.has_tlsgd(ctx)) {
        write32s(sym.get_tlsgd_addr(ctx) + A - P);
      } else if (sym.has_gottp(ctx)) {
        relax_gd_to_ie(loc, rels[i + 1], sym.get_gottp_addr(ctx) - P);
        i++;
      } else {
        relax_gd_to_le(loc, rels[i + 1], S - ctx.tp_addr);
        i++;
      }
      break;
    case R_X86_64_TLSLD:
      if (ctx.got->has_tlsld(ctx)) {
        write32s(ctx.got->get_tlsld_addr(ctx) + A - P);
      } else {
        relax_ld_to_le(loc, rels[i + 1], ctx.tp_addr - ctx.tls_begin);
        i++;
      }
      break;
    case R_X86_64_DTPOFF32:
      write32s(S + A - ctx.dtp_addr);
      break;
    case R_X86_64_DTPOFF64:
      *(ul64 *)loc = S + A - ctx.dtp_addr;
      break;
    case R_X86_64_TPOFF32:
      write32s(S + A - ctx.tp_addr);
      break;
    case R_X86_64_TPOFF64:
      *(ul64 *)loc = S + A - ctx.tp_addr;
      break;
    case R_X86_64_GOTTPOFF:
      if (sym.has_gottp(ctx)) {
        write32s(sym.get_gottp_addr(ctx) + A - P);
      } else {
        u32 insn = relax_gottpoff(loc - 3);
        loc[-3] = insn >> 16;
        loc[-2] = insn >> 8;
        loc[-1] = insn;
        write32s(S - ctx.tp_addr);
        assert(A == -4);
      }
      break;
    case R_X86_64_GOTPC32_TLSDESC:
      if (sym.has_tlsdesc(ctx)) {
        write32s(sym.get_tlsdesc_addr(ctx) + A - P);
      } else {
        u32 insn = relax_gotpc32_tlsdesc(loc - 3);
        loc[-3] = insn >> 16;
        loc[-2] = insn >> 8;
        loc[-1] = insn;
        write32s(S - ctx.tp_addr);
        assert(A == -4);
      }
      break;
    case R_X86_64_SIZE32:
      write32(sym.esym().st_size + A);
      break;
    case R_X86_64_SIZE64:
      *(ul64 *)loc = sym.esym().st_size + A;
      break;
    case R_X86_64_TLSDESC_CALL:
      if (!sym.has_tlsdesc(ctx)) {
        // call *(%rax) -> nop
        loc[0] = 0x66;
        loc[1] = 0x90;
      }
      break;
    default:
      unreachable();
    }
  }
}

// This function is responsible for applying relocations against
// non-SHF_ALLOC sections (i.e. sections that are not mapped to memory
// at runtime).
//
// Relocations against non-SHF_ALLOC sections are much easier to
// handle than that against SHF_ALLOC sections. It is because, since
// they are not mapped to memory, they don't contain any variable or
// function and never need PLT or GOT. Non-SHF_ALLOC sections are
// mostly debug info sections.
//
// Relocations against non-SHF_ALLOC sections are not scanned by
// scan_relocations.
template <>
void InputSection<E>::apply_reloc_nonalloc(Context<E> &ctx, u8 *base) {
  std::span<const ElfRel<E>> rels = get_rels(ctx);

  for (i64 i = 0; i < rels.size(); i++) {
    const ElfRel<E> &rel = rels[i];
    if (rel.r_type == R_NONE || record_undef_error(ctx, rel))
      continue;

    Symbol<E> &sym = *file.symbols[rel.r_sym];
    u8 *loc = base + rel.r_offset;

    auto check = [&](i64 val, i64 lo, i64 hi) {
      if (val < lo || hi <= val)
        Error(ctx) << *this << ": relocation " << rel << " against "
                   << sym << " out of range: " << val << " is not in ["
                   << lo << ", " << hi << ")";
    };

    auto write32 = [&](u64 val) {
      check(val, 0, 1LL << 32);
      *(ul32 *)loc = val;
    };

    auto write32s = [&](u64 val) {
      check(val, -(1LL << 31), 1LL << 31);
      *(ul32 *)loc = val;
    };

    SectionFragment<E> *frag;
    i64 frag_addend;
    std::tie(frag, frag_addend) = get_fragment(ctx, rel);

    u64 S = frag ? frag->get_addr(ctx) : sym.get_addr(ctx);
    u64 A = frag ? frag_addend : (i64)rel.r_addend;

    switch (rel.r_type) {
    case R_X86_64_8:
      check(S + A, 0, 1 << 8);
      *loc = S + A;
      break;
    case R_X86_64_16:
      check(S + A, 0, 1 << 16);
      *(ul16 *)loc = S + A;
      break;
    case R_X86_64_32:
      write32(S + A);
      break;
    case R_X86_64_32S:
      write32s(S + A);
      break;
    case R_X86_64_64:
      if (std::optional<u64> val = get_tombstone(sym, frag))
        *(ul64 *)loc = *val;
      else
        *(ul64 *)loc = S + A;
      break;
    case R_X86_64_DTPOFF32:
      if (std::optional<u64> val = get_tombstone(sym, frag))
        *(ul32 *)loc = *val;
      else
        write32s(S + A - ctx.dtp_addr);
      break;
    case R_X86_64_DTPOFF64:
      if (std::optional<u64> val = get_tombstone(sym, frag))
        *(ul64 *)loc = *val;
      else
        *(ul64 *)loc = S + A - ctx.dtp_addr;
      break;
    case R_X86_64_GOTOFF64:
      *(ul64 *)loc = S + A - ctx.gotplt->shdr.sh_addr;
      break;
    case R_X86_64_GOTPC64:
      // PC-relative relocation doesn't make sense for non-memory-allocated
      // section, but GCC 6.3.0 seems to create this reloc for
      // _GLOBAL_OFFSET_TABLE_.
      *(ul64 *)loc = ctx.gotplt->shdr.sh_addr + A;
      break;
    case R_X86_64_SIZE32:
      write32(sym.esym().st_size + A);
      break;
    case R_X86_64_SIZE64:
      *(ul64 *)loc = sym.esym().st_size + A;
      break;
    default:
      Fatal(ctx) << *this << ": invalid relocation for non-allocated sections: "
                 << rel;
      break;
    }
  }
}

// Linker has to create data structures in an output file to apply
// some type of relocations. For example, if a relocation refers a GOT
// or a PLT entry of a symbol, linker has to create an entry in .got
// or in .plt for that symbol. In order to fix the file layout, we
// need to scan relocations.
template <>
void InputSection<E>::scan_relocations(Context<E> &ctx) {
  assert(shdr().sh_flags & SHF_ALLOC);

  this->reldyn_offset = file.num_dynrel * sizeof(ElfRel<E>);
  std::span<const ElfRel<E>> rels = get_rels(ctx);

  // Scan relocations
  for (i64 i = 0; i < rels.size(); i++) {
    const ElfRel<E> &rel = rels[i];
    if (rel.r_type == R_NONE || record_undef_error(ctx, rel))
      continue;

    Symbol<E> &sym = *file.symbols[rel.r_sym];
    u8 *loc = (u8 *)(contents.data() + rel.r_offset);

    if (sym.is_ifunc())
      sym.flags |= NEEDS_GOT | NEEDS_PLT;

    switch (rel.r_type) {
    case R_X86_64_8:
    case R_X86_64_16:
    case R_X86_64_32:
    case R_X86_64_32S:
      scan_absrel(ctx, sym, rel);
      break;
    case R_X86_64_64:
      scan_dyn_absrel(ctx, sym, rel);
      break;
    case R_X86_64_PC8:
    case R_X86_64_PC16:
    case R_X86_64_PC32:
    case R_X86_64_PC64:
      scan_pcrel(ctx, sym, rel);
      break;
    case R_X86_64_GOT32:
    case R_X86_64_GOT64:
    case R_X86_64_GOTPC32:
    case R_X86_64_GOTPC64:
    case R_X86_64_GOTPCREL:
    case R_X86_64_GOTPCREL64:
    case R_X86_64_GOTPCRELX:
    case R_X86_64_REX_GOTPCRELX:
      sym.flags |= NEEDS_GOT;
      break;
    case R_X86_64_PLT32:
    case R_X86_64_PLTOFF64:
      if (sym.is_imported)
        sym.flags |= NEEDS_PLT;
      break;
    case R_X86_64_TLSGD:
      if (rel.r_addend != -4)
        Fatal(ctx) << *this << ": bad r_addend for R_X86_64_TLSGD";

      if (i + 1 == rels.size())
        Fatal(ctx) << *this << ": TLSGD reloc must be followed by PLT or GOTPCREL";

      if (u32 ty = rels[i + 1].r_type;
          ty != R_X86_64_PLT32 && ty != R_X86_64_PC32 &&
          ty != R_X86_64_PLTOFF64 && ty != R_X86_64_GOTPCREL &&
          ty != R_X86_64_GOTPCRELX)
        Fatal(ctx) << *this << ": TLSGD reloc must be followed by PLT or GOTPCREL";

      if (ctx.arg.is_static ||
          (ctx.arg.relax && !sym.is_imported && !ctx.arg.shared)) {
        // We always relax if -static because libc.a doesn't contain
        // __tls_get_addr().
        i++;
      } else if (ctx.arg.relax && !sym.is_imported && ctx.arg.shared &&
                 !ctx.arg.z_dlopen) {
        sym.flags |= NEEDS_GOTTP;
        i++;
      } else {
        sym.flags |= NEEDS_TLSGD;
      }
      break;
    case R_X86_64_TLSLD:
      if (rel.r_addend != -4)
        Fatal(ctx) << *this << ": bad r_addend for R_X86_64_TLSLD";

      if (i + 1 == rels.size())
        Fatal(ctx) << *this << ": TLSLD reloc must be followed by PLT or GOTPCREL";

      if (u32 ty = rels[i + 1].r_type;
          ty != R_X86_64_PLT32 && ty != R_X86_64_PC32 &&
          ty != R_X86_64_PLTOFF64 && ty != R_X86_64_GOTPCREL &&
          ty != R_X86_64_GOTPCRELX)
        Fatal(ctx) << *this << ": TLSLD reloc must be followed by PLT or GOTPCREL";

      // We always relax if -static because libc.a doesn't contain
      // __tls_get_addr().
      if (ctx.arg.is_static || (ctx.arg.relax && !ctx.arg.shared))
        i++;
      else
        ctx.needs_tlsld = true;
      break;
    case R_X86_64_GOTTPOFF: {
      if (rel.r_addend != -4)
        Fatal(ctx) << *this << ": bad r_addend for R_X86_64_GOTTPOFF";

      bool do_relax = ctx.arg.relax && !ctx.arg.shared &&
                      !sym.is_imported && relax_gottpoff(loc - 3);
      if (!do_relax)
        sym.flags |= NEEDS_GOTTP;
      break;
    }
    case R_X86_64_GOTPC32_TLSDESC: {
      if (rel.r_addend != -4)
        Fatal(ctx) << *this << ": bad r_addend for R_X86_64_GOTPC32_TLSDESC";

      if (relax_gotpc32_tlsdesc(loc - 3) == 0)
        Fatal(ctx) << *this << ": GOTPC32_TLSDESC relocation is used"
                   << " against an invalid code sequence";

      if (!relax_tlsdesc(ctx, sym))
        sym.flags |= NEEDS_TLSDESC;
      break;
    }
    case R_X86_64_TPOFF32:
    case R_X86_64_TPOFF64:
      check_tlsle(ctx, sym, rel);
      break;
    case R_X86_64_GOTOFF64:
    case R_X86_64_DTPOFF32:
    case R_X86_64_DTPOFF64:
    case R_X86_64_SIZE32:
    case R_X86_64_SIZE64:
    case R_X86_64_TLSDESC_CALL:
      break;
    default:
      Error(ctx) << *this << ": unknown relocation: " << rel;
    }
  }
}

} // namespace mold::elf
