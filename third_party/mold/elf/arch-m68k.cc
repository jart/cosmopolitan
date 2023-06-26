// clang-format off
// This file contains code for the Motorola 68000 series microprocessors,
// which is often abbreviated as m68k. Running a Unix-like system on a
// m68k-based machine today is probably a retro-computing hobby activity,
// but the processor was a popular choice to build Unix computers during
// '80s. Early Sun workstations for example used m68k. Macintosh until
// 1994 were based on m68k as well until they switched to PowerPC (and
// then to x86 and to ARM.)
//
// From the linker's point of view, it is not hard to support m68k. It's
// just a 32-bit big-endian CISC ISA. Compared to comtemporary i386,
// m68k's psABI is actually simpler because m68k has PC-relative memory
// access instructions and therefore can support position-independent
// code without too much hassle.
//
// https://github.com/rui314/psabi/blob/main/m68k.pdf

#include "third_party/mold/elf/mold.h"

namespace mold::elf {

using E = M68K;

template <>
void write_plt_header(Context<E> &ctx, u8 *buf) {
  static const u8 insn[] = {
    0x2f, 0x00,                         // move.l %d0, -(%sp)
    0x2f, 0x3b, 0x01, 0x70, 0, 0, 0, 0, // move.l (GOTPLT+4, %pc), -(%sp)
    0x4e, 0xfb, 0x01, 0x71, 0, 0, 0, 0, // jmp    ([GOTPLT+8, %pc])
  };

  memcpy(buf, insn, sizeof(insn));
  *(ub32 *)(buf + 6) = ctx.gotplt->shdr.sh_addr - ctx.plt->shdr.sh_addr;
  *(ub32 *)(buf + 14) = ctx.gotplt->shdr.sh_addr - ctx.plt->shdr.sh_addr - 4;
}

template <>
void write_plt_entry(Context<E> &ctx, u8 *buf, Symbol<E> &sym) {
  static const u8 insn[] = {
    0x20, 0x3c, 0, 0, 0, 0,             // move.l PLT_OFFSET, %d0
    0x4e, 0xfb, 0x01, 0x71, 0, 0, 0, 0, // jmp    ([GOTPLT_ENTRY, %pc])
  };

  memcpy(buf, insn, sizeof(insn));
  *(ub32 *)(buf + 2) = sym.get_plt_idx(ctx) * sizeof(ElfRel<E>);
  *(ub32 *)(buf + 10) = sym.get_gotplt_addr(ctx) - sym.get_plt_addr(ctx) - 8;
}

template <>
void write_pltgot_entry(Context<E> &ctx, u8 *buf, Symbol<E> &sym) {
  static const u8 insn[] = {
    0x4e, 0xfb, 0x01, 0x71, 0, 0, 0, 0, // jmp ([GOT_ENTRY, %pc])
  };

  memcpy(buf, insn, sizeof(insn));
  *(ub32 *)(buf + 4) = sym.get_got_addr(ctx) - sym.get_plt_addr(ctx) - 2;
}

template <>
void EhFrameSection<E>::apply_reloc(Context<E> &ctx, const ElfRel<E> &rel,
                                    u64 offset, u64 val) {
  u8 *loc = ctx.buf + this->shdr.sh_offset + offset;

  switch (rel.r_type) {
  case R_NONE:
    break;
  case R_68K_32:
    *(ub32 *)loc = val;
    break;
  case R_68K_PC32:
    *(ub32 *)loc = val - this->shdr.sh_addr - offset;
    break;
  default:
    Fatal(ctx) << "unsupported relocation in .eh_frame: " << rel;
  }
}

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

    auto write16 = [&](u64 val) {
      check(val, 0, 1 << 16);
      *(ub16 *)loc = val;
    };

    auto write16s = [&](u64 val) {
      check(val, -(1 << 15), 1 << 15);
      *(ub16 *)loc = val;
    };

    auto write8 = [&](u64 val) {
      check(val, 0, 1 << 8);
      *loc = val;
    };

    auto write8s = [&](u64 val) {
      check(val, -(1 << 7), 1 << 7);
      *loc = val;
    };

    u64 S = sym.get_addr(ctx);
    u64 A = rel.r_addend;
    u64 P = get_addr() + rel.r_offset;
    u64 G = sym.get_got_idx(ctx) * sizeof(Word<E>);
    u64 GOT = ctx.got->shdr.sh_addr;

    switch (rel.r_type) {
    case R_68K_32:
      apply_dyn_absrel(ctx, sym, rel, loc, S, A, P, dynrel);
      break;
    case R_68K_16:
      write16(S + A);
      break;
    case R_68K_8:
      write8(S + A);
      break;
    case R_68K_PC32:
    case R_68K_PLT32:
      *(ub32 *)loc = S + A - P;
      break;
    case R_68K_PC16:
    case R_68K_PLT16:
      write16s(S + A - P);
      break;
    case R_68K_PC8:
    case R_68K_PLT8:
      write8s(S + A - P);
      break;
    case R_68K_GOTPCREL32:
      *(ub32 *)loc = GOT + A - P;
      break;
    case R_68K_GOTPCREL16:
      write16s(GOT + A - P);
      break;
    case R_68K_GOTPCREL8:
      write8s(GOT + A - P);
      break;
    case R_68K_GOTOFF32:
      *(ub32 *)loc = G + A;
      break;
    case R_68K_GOTOFF16:
      write16(G + A);
      break;
    case R_68K_GOTOFF8:
      write8(G + A);
      break;
    case R_68K_TLS_GD32:
      *(ub32 *)loc = sym.get_tlsgd_addr(ctx) + A - GOT;
      break;
    case R_68K_TLS_GD16:
      write16(sym.get_tlsgd_addr(ctx) + A - GOT);
      break;
    case R_68K_TLS_GD8:
      write8(sym.get_tlsgd_addr(ctx) + A - GOT);
      break;
    case R_68K_TLS_LDM32:
      *(ub32 *)loc = ctx.got->get_tlsld_addr(ctx) + A - GOT;
      break;
    case R_68K_TLS_LDM16:
      write16(ctx.got->get_tlsld_addr(ctx) + A - GOT);
      break;
    case R_68K_TLS_LDM8:
      write8(ctx.got->get_tlsld_addr(ctx) + A - GOT);
      break;
    case R_68K_TLS_LDO32:
      *(ub32 *)loc = S + A - ctx.dtp_addr;
      break;
    case R_68K_TLS_LDO16:
      write16s(S + A - ctx.dtp_addr);
      break;
    case R_68K_TLS_LDO8:
      write8s(S + A - ctx.dtp_addr);
      break;
    case R_68K_TLS_IE32:
      *(ub32 *)loc = sym.get_gottp_addr(ctx) + A - GOT;
      break;
    case R_68K_TLS_IE16:
      write16(sym.get_gottp_addr(ctx) + A - GOT);
      break;
    case R_68K_TLS_IE8:
      write8(sym.get_gottp_addr(ctx) + A - GOT);
      break;
    case R_68K_TLS_LE32:
      *(ub32 *)loc = S + A - ctx.tp_addr;
      break;
    case R_68K_TLS_LE16:
      write16(S + A - ctx.tp_addr);
      break;
    case R_68K_TLS_LE8:
      write8(S + A - ctx.tp_addr);
      break;
    default:
      unreachable();
    }
  }
}

template <>
void InputSection<E>::apply_reloc_nonalloc(Context<E> &ctx, u8 *base) {
  std::span<const ElfRel<E>> rels = get_rels(ctx);

  for (i64 i = 0; i < rels.size(); i++) {
    const ElfRel<E> &rel = rels[i];
    if (rel.r_type == R_NONE || record_undef_error(ctx, rel))
      continue;

    Symbol<E> &sym = *file.symbols[rel.r_sym];
    u8 *loc = base + rel.r_offset;

    SectionFragment<E> *frag;
    i64 frag_addend;
    std::tie(frag, frag_addend) = get_fragment(ctx, rel);

    u64 S = frag ? frag->get_addr(ctx) : sym.get_addr(ctx);
    u64 A = frag ? frag_addend : (i64)rel.r_addend;

    switch (rel.r_type) {
    case R_68K_32:
      if (std::optional<u64> val = get_tombstone(sym, frag))
        *(ub32 *)loc = *val;
      else
        *(ub32 *)loc = S + A;
      break;
    default:
      Fatal(ctx) << *this << ": invalid relocation for non-allocated sections: "
                 << rel;
    }
  }
}

template <>
void InputSection<E>::scan_relocations(Context<E> &ctx) {
  assert(shdr().sh_flags & SHF_ALLOC);

  this->reldyn_offset = file.num_dynrel * sizeof(ElfRel<E>);
  std::span<const ElfRel<E>> rels = get_rels(ctx);

  for (i64 i = 0; i < rels.size(); i++) {
    const ElfRel<E> &rel = rels[i];
    if (rel.r_type == R_NONE || record_undef_error(ctx, rel))
      continue;

    Symbol<E> &sym = *file.symbols[rel.r_sym];

    if (sym.is_ifunc())
      Error(ctx) << sym << ": GNU ifunc symbol is not supported on m68k";

    switch (rel.r_type) {
    case R_68K_32:
      scan_dyn_absrel(ctx, sym, rel);
      break;
    case R_68K_16:
    case R_68K_8:
      scan_absrel(ctx, sym, rel);
      break;
    case R_68K_PC32:
    case R_68K_PC16:
    case R_68K_PC8:
      scan_pcrel(ctx, sym, rel);
      break;
    case R_68K_GOTPCREL32:
    case R_68K_GOTPCREL16:
    case R_68K_GOTPCREL8:
    case R_68K_GOTOFF32:
    case R_68K_GOTOFF16:
    case R_68K_GOTOFF8:
      sym.flags |= NEEDS_GOT;
      break;
    case R_68K_PLT32:
    case R_68K_PLT16:
    case R_68K_PLT8:
      if (sym.is_imported)
        sym.flags |= NEEDS_PLT;
      break;
    case R_68K_TLS_GD32:
    case R_68K_TLS_GD16:
    case R_68K_TLS_GD8:
      sym.flags |= NEEDS_TLSGD;
      break;
    case R_68K_TLS_LDM32:
    case R_68K_TLS_LDM16:
    case R_68K_TLS_LDM8:
      ctx.needs_tlsld = true;
      break;
    case R_68K_TLS_IE32:
    case R_68K_TLS_IE16:
    case R_68K_TLS_IE8:
      sym.flags |= NEEDS_GOTTP;
      break;
    case R_68K_TLS_LE32:
    case R_68K_TLS_LE16:
    case R_68K_TLS_LE8:
      check_tlsle(ctx, sym, rel);
      break;
    case R_68K_TLS_LDO32:
    case R_68K_TLS_LDO16:
    case R_68K_TLS_LDO8:
      break;
    default:
      Error(ctx) << *this << ": unknown relocation: " << rel;
    }
  }
}

} // namespace mold::elf
