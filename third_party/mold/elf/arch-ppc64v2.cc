// clang-format off
// This file implements the PowerPC ELFv2 ABI which was standardized in
// 2014. Modern little-endian PowerPC systems are based on this ABI.
// The ABI is often referred to as "ppc64le". This shouldn't be confused
// with "ppc64" which refers to the original, big-endian PowerPC systems.
//
// PPC64 is a bit tricky to support because PC-relative load/store
// instructions hadn't been available until Power10 which debuted in 2021.
// Prior to Power10, it wasn't trivial for position-independent code (PIC)
// to load a value from, for example, .got, as we can't do that with [PC +
// the offset to the .got entry].
//
// In the following, I'll explain how PIC is supported on pre-Power10
// systems first and then explain what has changed with Power10.
//
//
// Position-independent code on Power9 or earlier:
//
// We can get the program counter on older PPC64 systems with the
// following four instructions
//
//   mflr  r1  // save the current link register to r1
//   bl    .+4 // branch to the next instruction as if it were a function
//   mflr  r0  // copy the return address to r0
//   mtlr  r1  // restore the original link register value
//
// , but it's too expensive to do if we do this for each load/store.
//
// As a workaround, most functions are compiled in such a way that r2 is
// assumed to always contain the address of .got + 0x8000. With this, we
// can for example load the first entry of .got with a single instruction
// `lw r0, -0x8000(r2)`. r2 is called the TOC pointer.
//
// There's only one .got for each ELF module. Therefore, if a callee is in
// the same ELF module, r2 doesn't have to be recomputed. Most function
// calls are usually within the same ELF module, so this mechanism is
// efficient.
//
// A function compiled for pre-Power10 usually has two entry points,
// global and local. The global entry point usually 8 bytes precedes
// the local entry point. In between is the following instructions:
//
//   addis r2, r12, .TOC.@ha
//   addi  r2, r2,  .TOC.@lo + 4;
//
// The global entry point assumes that the address of itself is in r12,
// and it computes its own TOC pointer from r12. It's easy to do so for
// the callee because the offset between its .got + 0x8000 and the
// function is known at link-time. The above code sequence then falls
// through to the local entry point that assumes r2 is .got + 0x8000.
//
// So, if a callee's TOC pointer is different from the current one
// (e.g. calling a function in another .so), we first load the callee's
// address to r12 (e.g. from .got.plt with a r2-relative load) and branch
// to that address. Then the callee computes its own TOC pointer using
// r12.
//
//
// Position-independent code on Power10:
//
// Power10 added 8-bytes-long instructions to the ISA. Some of them are
// PC-relative load/store instructions that take 34 bits offsets.
// Functions compiled with `-mcpu=power10` use these instructions for PIC.
// r2 does not have a special meaning in such fucntions.
//
// When a fucntion compiled for Power10 calls a function that uses the TOC
// pointer, we need to compute a correct value for TOC and set it to r2
// before transferring the control to the callee. Thunks are responsible
// for doing it.
//
// `_NOTOC` relocations such as `R_PPC64_REL24_NOTOC` indicate that the
// callee does not use TOC (i.e. compiled with `-mcpu=power10`). If a
// function using TOC is referenced via a `_NOTOC` relocation, that call
// is made through a range extension thunk.
//
//
// Note on section names: the PPC64 psABI uses a weird naming convention
// which calls .got.plt .plt. We ignored that part because it's just
// confusing. Since the runtime only cares about segments, we should be
// able to name sections whatever we want.
//
// https://github.com/rui314/psabi/blob/main/ppc64v2.pdf

#include "third_party/mold/elf/mold.h"

namespace mold::elf {

using E = PPC64V2;

static u64 lo(u64 x)    { return x & 0xffff; }
static u64 hi(u64 x)    { return x >> 16; }
static u64 ha(u64 x)    { return (x + 0x8000) >> 16; }
static u64 high(u64 x)  { return (x >> 16) & 0xffff; }
static u64 higha(u64 x) { return ((x + 0x8000) >> 16) & 0xffff; }

static u64 prefix34(u64 x) {
  return bits(x, 33, 16) | (bits(x, 15, 0) << 32);
}

// .plt is used only for lazy symbol resolution on PPC64. All PLT
// calls are made via range extension thunks even if they are within
// reach. Thunks read addresses from .got.plt and jump there.
// Therefore, once PLT symbols are resolved and final addresses are
// written to .got.plt, thunks just skip .plt and directly jump to the
// resolved addresses.
template <>
void write_plt_header(Context<E> &ctx, u8 *buf) {
  static const ul32 insn[] = {
    // Get PC
    0x7c08'02a6, // mflr    r0
    0x429f'0005, // bcl     20, 31, 4 // obtain PC
    0x7d68'02a6, // mflr    r11
    0x7c08'03a6, // mtlr    r0

    // Compute the PLT entry index
    0xe80b'002c, // ld      r0, 44(r11)
    0x7d8b'6050, // subf    r12, r11, r12
    0x7d60'5a14, // add     r11, r0, r11
    0x380c'ffcc, // addi    r0, r12, -52
    0x7800'f082, // rldicl  r0, r0, 62, 2

    // Load .got.plt[0] and .got.plt[1] and branch to .got.plt[0]
    0xe98b'0000, // ld      r12, 0(r11)
    0x7d89'03a6, // mtctr   r12
    0xe96b'0008, // ld      r11, 8(r11)
    0x4e80'0420, // bctr

    // .quad .got.plt - .plt - 8
    0x0000'0000,
    0x0000'0000,
  };

  memcpy(buf, insn, sizeof(insn));
  *(ul64 *)(buf + 52) = ctx.gotplt->shdr.sh_addr - ctx.plt->shdr.sh_addr - 8;
}

template <>
void write_plt_entry(Context<E> &ctx, u8 *buf, Symbol<E> &sym) {
  // When the control is transferred to a PLT entry, the PLT entry's
  // address is already set to %r12 by the caller.
  i64 offset = ctx.plt->shdr.sh_addr - sym.get_plt_addr(ctx);
  *(ul32 *)buf = 0x4b00'0000 | (offset & 0x00ff'ffff);        // b plt0
}

// .plt.got is not necessary on PPC64 because range extension thunks
// directly read GOT entries and jump there.
template <>
void write_pltgot_entry(Context<E> &ctx, u8 *buf, Symbol<E> &sym) {}

template <>
void EhFrameSection<E>::apply_reloc(Context<E> &ctx, const ElfRel<E> &rel,
                                    u64 offset, u64 val) {
  u8 *loc = ctx.buf + this->shdr.sh_offset + offset;

  switch (rel.r_type) {
  case R_NONE:
    break;
  case R_PPC64_ADDR64:
    *(ul64 *)loc = val;
    break;
  case R_PPC64_REL32:
    *(ul32 *)loc = val - this->shdr.sh_addr - offset;
    break;
  case R_PPC64_REL64:
    *(ul64 *)loc = val - this->shdr.sh_addr - offset;
    break;
  default:
    Fatal(ctx) << "unsupported relocation in .eh_frame: " << rel;
  }
}

static u64 get_local_entry_offset(Context<E> &ctx, Symbol<E> &sym) {
  i64 val = sym.esym().ppc_local_entry;
  assert(val <= 7);
  if (val == 7)
    Fatal(ctx) << sym << ": local entry offset 7 is reserved";

  if (val == 0 || val == 1)
    return 0;
  return 1 << val;
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

    u64 S = sym.get_addr(ctx);
    u64 A = rel.r_addend;
    u64 P = get_addr() + rel.r_offset;
    u64 G = sym.get_got_idx(ctx) * sizeof(Word<E>);
    u64 GOT = ctx.got->shdr.sh_addr;
    u64 TOC = ctx.extra.TOC->value;

    auto r2save_thunk_addr = [&] { return get_thunk_addr(i); };
    auto no_r2save_thunk_addr = [&] { return get_thunk_addr(i) + 4; };

    switch (rel.r_type) {
    case R_PPC64_ADDR64:
      if (name() == ".toc")
        apply_toc_rel(ctx, sym, rel, loc, S, A, P, dynrel);
      else
        apply_dyn_absrel(ctx, sym, rel, loc, S, A, P, dynrel);
      break;
    case R_PPC64_TOC16_HA:
      *(ul16 *)loc = ha(S + A - TOC);
      break;
    case R_PPC64_TOC16_LO:
      *(ul16 *)loc = lo(S + A - TOC);
      break;
    case R_PPC64_TOC16_DS:
    case R_PPC64_TOC16_LO_DS:
      *(ul16 *)loc |= (S + A - TOC) & 0xfffc;
      break;
    case R_PPC64_REL24:
      if (sym.has_plt(ctx) || !sym.esym().preserves_r2()) {
        i64 val = r2save_thunk_addr() + A - P;
        *(ul32 *)loc |= bits(val, 25, 2) << 2;

        // The thunk saves %r2 to the caller's r2 save slot. We need to
        // restore it after function return. To do so, there's usually a
        // NOP as a placeholder after a BL. 0x6000'0000 is a NOP.
        if (*(ul32 *)(loc + 4) == 0x6000'0000)
          *(ul32 *)(loc + 4) = 0xe841'0018; // ld r2, 24(r1)
      } else {
        i64 val = S + get_local_entry_offset(ctx, sym) + A - P;
        if (sign_extend(val, 25) != val)
          val = no_r2save_thunk_addr() + A - P;
        *(ul32 *)loc |= bits(val, 25, 2) << 2;
      }
      break;
    case R_PPC64_REL24_NOTOC:
      if (sym.has_plt(ctx) || sym.esym().uses_toc()) {
        i64 val = no_r2save_thunk_addr() + A - P;
        *(ul32 *)loc |= bits(val, 25, 2) << 2;
      } else {
        i64 val = S + A - P;
        if (sign_extend(val, 25) != val)
          val = no_r2save_thunk_addr() + A - P;
        *(ul32 *)loc |= bits(val, 25, 2) << 2;
      }
      break;
    case R_PPC64_REL32:
      *(ul32 *)loc = S + A - P;
      break;
    case R_PPC64_REL64:
      *(ul64 *)loc = S + A - P;
      break;
    case R_PPC64_REL16_HA:
      *(ul16 *)loc = ha(S + A - P);
      break;
    case R_PPC64_REL16_LO:
      *(ul16 *)loc = lo(S + A - P);
      break;
    case R_PPC64_PLT16_HA:
      *(ul16 *)loc = ha(G + GOT - TOC);
      break;
    case R_PPC64_PLT16_HI:
      *(ul16 *)loc = hi(G + GOT - TOC);
      break;
    case R_PPC64_PLT16_LO:
      *(ul16 *)loc = lo(G + GOT - TOC);
      break;
    case R_PPC64_PLT16_LO_DS:
      *(ul16 *)loc |= (G + GOT - TOC) & 0xfffc;
      break;
    case R_PPC64_PLT_PCREL34:
    case R_PPC64_PLT_PCREL34_NOTOC:
    case R_PPC64_GOT_PCREL34:
      *(ul64 *)loc |= prefix34(G + GOT - P);
      break;
    case R_PPC64_PCREL34:
      *(ul64 *)loc |= prefix34(S + A - P);
      break;
    case R_PPC64_GOT_TPREL16_HA:
      *(ul16 *)loc = ha(sym.get_gottp_addr(ctx) - TOC);
      break;
    case R_PPC64_GOT_TPREL16_LO_DS:
      *(ul16 *)loc |= (sym.get_gottp_addr(ctx) - TOC) & 0xfffc;
      break;
    case R_PPC64_GOT_TPREL_PCREL34:
      *(ul64 *)loc |= prefix34(sym.get_gottp_addr(ctx) - P);
      break;
    case R_PPC64_GOT_TLSGD16_HA:
      *(ul16 *)loc = ha(sym.get_tlsgd_addr(ctx) - TOC);
      break;
    case R_PPC64_GOT_TLSGD16_LO:
      *(ul16 *)loc = lo(sym.get_tlsgd_addr(ctx) - TOC);
      break;
    case R_PPC64_GOT_TLSGD_PCREL34:
      *(ul64 *)loc |= prefix34(sym.get_tlsgd_addr(ctx) - P);
      break;
    case R_PPC64_GOT_TLSLD16_HA:
      *(ul16 *)loc = ha(ctx.got->get_tlsld_addr(ctx) - TOC);
      break;
    case R_PPC64_GOT_TLSLD16_LO:
      *(ul16 *)loc = lo(ctx.got->get_tlsld_addr(ctx) - TOC);
      break;
    case R_PPC64_GOT_TLSLD_PCREL34:
      *(ul64 *)loc |= prefix34(ctx.got->get_tlsld_addr(ctx) - P);
      break;
    case R_PPC64_DTPREL16_HA:
      *(ul16 *)loc = ha(S + A - ctx.dtp_addr);
      break;
    case R_PPC64_DTPREL16_LO:
      *(ul16 *)loc = lo(S + A - ctx.dtp_addr);
      break;
    case R_PPC64_DTPREL34:
      *(ul64 *)loc |= prefix34(S + A - ctx.dtp_addr);
      break;
    case R_PPC64_TPREL16_HA:
      *(ul16 *)loc = ha(S + A - ctx.tp_addr);
      break;
    case R_PPC64_TPREL16_LO:
      *(ul16 *)loc = lo(S + A - ctx.tp_addr);
      break;
    case R_PPC64_PLTSEQ:
    case R_PPC64_PLTSEQ_NOTOC:
    case R_PPC64_PLTCALL:
    case R_PPC64_PLTCALL_NOTOC:
    case R_PPC64_TLS:
    case R_PPC64_TLSGD:
    case R_PPC64_TLSLD:
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

    auto check = [&](i64 val, i64 lo, i64 hi) {
      if (val < lo || hi <= val)
        Error(ctx) << *this << ": relocation " << rel << " against "
                   << sym << " out of range: " << val << " is not in ["
                   << lo << ", " << hi << ")";
    };

    SectionFragment<E> *frag;
    i64 frag_addend;
    std::tie(frag, frag_addend) = get_fragment(ctx, rel);

    u64 S = frag ? frag->get_addr(ctx) : sym.get_addr(ctx);
    u64 A = frag ? frag_addend : (i64)rel.r_addend;

    switch (rel.r_type) {
    case R_PPC64_ADDR64:
      if (std::optional<u64> val = get_tombstone(sym, frag))
        *(ul64 *)loc = *val;
      else
        *(ul64 *)loc = S + A;
      break;
    case R_PPC64_ADDR32: {
      i64 val = S + A;
      check(val, 0, 1LL << 32);
      *(ul32 *)loc = val;
      break;
    }
    case R_PPC64_DTPREL64:
      *(ul64 *)loc = S + A - ctx.dtp_addr;
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

  // Scan relocations
  for (i64 i = 0; i < rels.size(); i++) {
    const ElfRel<E> &rel = rels[i];
    if (rel.r_type == R_NONE || record_undef_error(ctx, rel))
      continue;

    Symbol<E> &sym = *file.symbols[rel.r_sym];

    if (sym.is_ifunc())
      sym.flags |= NEEDS_GOT | NEEDS_PLT;

    switch (rel.r_type) {
    case R_PPC64_ADDR64:
      if (name() == ".toc")
        scan_toc_rel(ctx, sym, rel);
      else
        scan_dyn_absrel(ctx, sym, rel);
      break;
    case R_PPC64_GOT_TPREL16_HA:
    case R_PPC64_GOT_TPREL_PCREL34:
      sym.flags |= NEEDS_GOTTP;
      break;
    case R_PPC64_REL24:
      if (sym.is_imported)
        sym.flags |= NEEDS_PLT;
      break;
    case R_PPC64_REL24_NOTOC:
      if (sym.is_imported)
        sym.flags |= NEEDS_PLT;
      ctx.extra.is_power10 = true;
      break;
    case R_PPC64_PLT16_HA:
    case R_PPC64_PLT_PCREL34:
    case R_PPC64_PLT_PCREL34_NOTOC:
    case R_PPC64_GOT_PCREL34:
      sym.flags |= NEEDS_GOT;
      break;
    case R_PPC64_GOT_TLSGD16_HA:
    case R_PPC64_GOT_TLSGD_PCREL34:
      sym.flags |= NEEDS_TLSGD;
      break;
    case R_PPC64_GOT_TLSLD16_HA:
    case R_PPC64_GOT_TLSLD_PCREL34:
      ctx.needs_tlsld = true;
      break;
    case R_PPC64_TPREL16_HA:
    case R_PPC64_TPREL16_LO:
      check_tlsle(ctx, sym, rel);
      break;
    case R_PPC64_REL32:
    case R_PPC64_REL64:
    case R_PPC64_TOC16_HA:
    case R_PPC64_TOC16_LO:
    case R_PPC64_TOC16_LO_DS:
    case R_PPC64_TOC16_DS:
    case R_PPC64_REL16_HA:
    case R_PPC64_REL16_LO:
    case R_PPC64_PLT16_HI:
    case R_PPC64_PLT16_LO:
    case R_PPC64_PLT16_LO_DS:
    case R_PPC64_PCREL34:
    case R_PPC64_PLTSEQ:
    case R_PPC64_PLTSEQ_NOTOC:
    case R_PPC64_PLTCALL:
    case R_PPC64_PLTCALL_NOTOC:
    case R_PPC64_GOT_TPREL16_LO_DS:
    case R_PPC64_GOT_TLSGD16_LO:
    case R_PPC64_GOT_TLSLD16_LO:
    case R_PPC64_TLS:
    case R_PPC64_TLSGD:
    case R_PPC64_TLSLD:
    case R_PPC64_DTPREL16_HA:
    case R_PPC64_DTPREL16_LO:
    case R_PPC64_DTPREL34:
      break;
    default:
      Error(ctx) << *this << ": unknown relocation: " << rel;
    }
  }
}

template <>
void RangeExtensionThunk<E>::copy_buf(Context<E> &ctx) {
  u8 *buf = ctx.buf + output_section.shdr.sh_offset + offset;

  // If the destination is PLT, we read an address from .got.plt or .got
  // and jump there.
  static const ul32 plt_thunk[] = {
    0xf841'0018, // std   r2, 24(r1)
    0x3d82'0000, // addis r12, r2, foo@gotplt@toc@ha
    0xe98c'0000, // ld    r12, foo@gotplt@toc@lo(r12)
    0x7d89'03a6, // mtctr r12
    0x4e80'0420, // bctr
  };

  static const ul32 plt_thunk_power10[] = {
    0xf841'0018, // std   r2, 24(r1)
    0x0410'0000, // pld   r12, foo@gotplt@pcrel
    0xe580'0000,
    0x7d89'03a6, // mtctr r12
    0x4e80'0420, // bctr
  };

  // If the destination is a non-imported function, we directly jump
  // to its local entry point.
  static const ul32 local_thunk[] = {
    0xf841'0018, // std   r2, 24(r1)
    0x3d82'0000, // addis r12, r2,  foo@toc@ha
    0x398c'0000, // addi  r12, r12, foo@toc@lo
    0x7d89'03a6, // mtctr r12
    0x4e80'0420, // bctr
  };

  static const ul32 local_thunk_power10[] = {
    0xf841'0018, // std   r2, 24(r1)
    0x0610'0000, // pla   r12, foo@pcrel
    0x3980'0000,
    0x7d89'03a6, // mtctr r12
    0x4e80'0420, // bctr
  };

  static_assert(E::thunk_size == sizeof(plt_thunk));
  static_assert(E::thunk_size == sizeof(plt_thunk_power10));
  static_assert(E::thunk_size == sizeof(local_thunk));
  static_assert(E::thunk_size == sizeof(local_thunk_power10));

  for (i64 i = 0; i < symbols.size(); i++) {
    Symbol<E> &sym = *symbols[i];
    ul32 *loc = (ul32 *)(buf + i * E::thunk_size);

    if (sym.has_plt(ctx)) {
      u64 got = sym.has_got(ctx) ? sym.get_got_addr(ctx) : sym.get_gotplt_addr(ctx);

      if (ctx.extra.is_power10) {
        memcpy(loc, plt_thunk_power10, E::thunk_size);
        *(ul64 *)(loc + 1) |= prefix34(got - get_addr(i) - 4);
      } else {
        i64 val = got - ctx.extra.TOC->value;
        memcpy(loc, plt_thunk, E::thunk_size);
        loc[1] |= higha(val);
        loc[2] |= lo(val);
      }
    } else {
      if (ctx.extra.is_power10) {
        memcpy(loc, local_thunk_power10, E::thunk_size);
        *(ul64 *)(loc + 1) |= prefix34(sym.get_addr(ctx) - get_addr(i) - 4);
      } else {
        i64 val = sym.get_addr(ctx) - ctx.extra.TOC->value;
        memcpy(loc, local_thunk, E::thunk_size);
        loc[1] |= higha(val);
        loc[2] |= lo(val);
      }
    }
  }
}

} // namespace mold::elf
