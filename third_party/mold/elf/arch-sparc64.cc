// clang-format off
// SPARC is a RISC ISA developed by Sun Microsystems.
//
// The byte order of the processor is big-endian. Anything larger than a
// byte is stored in the "reverse" order compared to little-endian
// processors such as x86-64.
//
// All instructions are 4 bytes long and aligned to 4 bytes boundaries.
//
// A notable feature of SPARC is that, unlike other RISC ISAs, it doesn't
// need range extension thunks. It is because the SPARC's CALL instruction
// contains a whopping 30 bits immediate. The processor scales it by 4 to
// extend it to 32 bits (this is doable because all instructions are
// aligned to 4 bytes boundaries, so the least significant two bits are
// always zero). That means CALL's reach is PC ± 2 GiB, elinating the
// need of range extension thunks. It comes with the cost that the CALL
// instruction alone takes 1/4th of the instruction encoding space,
// though.
//
// SPARC has 32 general purpose registers. CALL instruction saves a return
// address to %o7, which is an alias for %r15. Thread pointer is stored to
// %g7 which is %r7.
//
// SPARC does not have PC-relative load/store instructions. To access data
// in the position-independent manner, we usually first set the address of
// .got to, for example, %l7, with the following piece of code
//
//   sethi  %hi(. - _GLOBAL_OFFSET_TABLE_), %l7
//   add  %l7, %lo(. - _GLOBAL_OFFSET_TABLE_), %l7
//   call __sparc_get_pc_thunk.l7
//   nop
//
// where __sparc_get_pc_thunk.l7 is defined as
//
//   retl
//   add  %o7, %l7, %l7
//
// . SETHI and the following ADD materialize a 32 bits offset to .got.
// CALL instruction sets a return address to $o7, and the subsequent ADD
// adds it to the GOT offset to materialize the absolute address of .got.
//
// Note that we have a NOP after CALL and an ADD after RETL because of
// SPARC's delay branch slots. That is, the SPARC processor always
// executes one instruction after a branch even if the branch is taken.
// This may seem like an odd behavior, and indeed it is considered as such
// (that's a premature optimization for the early pipelined SPARC
// processors), but that's been a part of the ISA's spec so that's what it
// is.
//
// Note also that the .got address obtained this way is not shared between
// functions, so functions can use an arbitrary register to hold the .got
// address. That also means each function needs to execute the above piece
// of code to become position-independent.
//
// This scheme is very similar to i386. That may not be a coincidence
// because the i386 ELF psABI is created by Sun Microsystems too.
//
// https://github.com/rui314/psabi/blob/main/sparc.pdf

#include "third_party/mold/elf/mold.h"

namespace mold::elf {

using E = SPARC64;

// SPARC's PLT section is writable despite containing executable code.
// We don't need to write the PLT header entry because the dynamic loader
// will do that for us.
//
// We also don't need a .got.plt section to store the result of lazy PLT
// symbol resolution because the dynamic symbol resolver directly mutates
// instructions in PLT so that they jump to the right places next time.
// That's why each PLT entry contains lots of NOPs; they are a placeholder
// for the runtime to add more instructions.
//
// Self-modifying code is nowadays considered really bad from the security
// point of view, though.
template <>
void write_plt_header(Context<E> &ctx, u8 *buf) {
  memset(buf, 0, E::plt_hdr_size);
}

template <>
void write_plt_entry(Context<E> &ctx, u8 *buf, Symbol<E> &sym) {
  static ub32 insn[] = {
    0x0300'0000, // sethi (. - .PLT0), %g1
    0x3068'0000, // ba,a  %xcc, .PLT1
    0x0100'0000, // nop
    0x0100'0000, // nop
    0x0100'0000, // nop
    0x0100'0000, // nop
    0x0100'0000, // nop
    0x0100'0000, // nop
  };

  u64 plt0 = ctx.plt->shdr.sh_addr;
  u64 plt1 = ctx.plt->shdr.sh_addr + E::plt_size;
  u64 entry = sym.get_plt_addr(ctx);

  memcpy(buf, insn, sizeof(insn));
  *(ub32 *)buf |= bits(entry - plt0, 21, 0);
  *(ub32 *)(buf + 4) |= bits(plt1 - entry - 4, 20, 2);
}

template <>
void write_pltgot_entry(Context<E> &ctx, u8 *buf, Symbol<E> &sym) {
  static ub32 entry[] = {
    0x8a10'000f, // mov  %o7, %g5
    0x4000'0002, // call . + 8
    0xc25b'e014, // ldx  [ %o7 + 20 ], %g1
    0xc25b'c001, // ldx  [ %o7 + %g1 ], %g1
    0x81c0'4000, // jmp  %g1
    0x9e10'0005, // mov  %g5, %o7
    0x0000'0000, // .quad $plt_entry - $got_entry
    0x0000'0000,
  };

  memcpy(buf, entry, sizeof(entry));
  *(ub64 *)(buf + 24) = sym.get_got_addr(ctx) - sym.get_plt_addr(ctx) - 4;
}

template <>
void EhFrameSection<E>::apply_reloc(Context<E> &ctx, const ElfRel<E> &rel,
                                    u64 offset, u64 val) {
  u8 *loc = ctx.buf + this->shdr.sh_offset + offset;

  switch (rel.r_type) {
  case R_NONE:
    break;
  case R_SPARC_64:
  case R_SPARC_UA64:
    *(ub64 *)loc = val;
    break;
  case R_SPARC_DISP32:
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

    u64 S = sym.get_addr(ctx);
    u64 A = rel.r_addend;
    u64 P = (get_addr() + rel.r_offset);
    u64 G = (sym.get_got_idx(ctx) * sizeof(Word<E>));
    u64 GOT = ctx.got->shdr.sh_addr;

    switch (rel.r_type) {
    case R_SPARC_64:
      apply_dyn_absrel(ctx, sym, rel, loc, S, A, P, dynrel);
      break;
    case R_SPARC_5:
      check(S + A, 0, 1 << 5);
      *(ub32 *)loc |= bits(S + A, 4, 0);
      break;
    case R_SPARC_6:
      check(S + A, 0, 1 << 6);
      *(ub32 *)loc |= bits(S + A, 5, 0);
      break;
    case R_SPARC_7:
      check(S + A, 0, 1 << 7);
      *(ub32 *)loc |= bits(S + A, 6, 0);
      break;
    case R_SPARC_8:
      check(S + A, 0, 1 << 8);
      *(u8 *)loc = S + A;
      break;
    case R_SPARC_10:
      check(S + A, 0, 1 << 10);
      *(ub32 *)loc |= bits(S + A, 9, 0);
      break;
    case R_SPARC_LO10:
    case R_SPARC_LOPLT10:
      *(ub32 *)loc |= bits(S + A, 9, 0);
      break;
    case R_SPARC_11:
      check(S + A, 0, 1 << 11);
      *(ub32 *)loc |= bits(S + A, 10, 0);
      break;
    case R_SPARC_13:
      check(S + A, 0, 1 << 13);
      *(ub32 *)loc |= bits(S + A, 12, 0);
      break;
    case R_SPARC_16:
    case R_SPARC_UA16:
      check(S + A, 0, 1 << 16);
      *(ub16 *)loc = S + A;
      break;
    case R_SPARC_22:
      check(S + A, 0, 1 << 22);
      *(ub32 *)loc |= bits(S + A, 21, 0);
      break;
    case R_SPARC_32:
    case R_SPARC_UA32:
    case R_SPARC_PLT32:
      check(S + A, 0, 1LL << 32);
      *(ub32 *)loc = S + A;
      break;
    case R_SPARC_PLT64:
    case R_SPARC_UA64:
    case R_SPARC_REGISTER:
      *(ub64 *)loc = S + A;
      break;
    case R_SPARC_DISP8:
      check(S + A - P, -(1 << 7), 1 << 7);
      *(u8 *)loc = S + A - P;
      break;
    case R_SPARC_DISP16:
      check(S + A - P, -(1 << 15), 1 << 15);
      *(ub16 *)loc = S + A - P;
      break;
    case R_SPARC_DISP32:
    case R_SPARC_PCPLT32:
      check(S + A - P, -(1LL << 31), 1LL << 31);
      *(ub32 *)loc = S + A - P;
      break;
    case R_SPARC_DISP64:
      *(ub64 *)loc = S + A - P;
      break;
    case R_SPARC_WDISP16: {
      i64 val = S + A - P;
      check(val, -(1 << 16), 1 << 16);
      *(ub16 *)loc |= (bit(val, 16) << 21) | bits(val, 15, 2);
      break;
    }
    case R_SPARC_WDISP19:
      check(S + A - P, -(1 << 20), 1 << 20);
      *(ub32 *)loc |= bits(S + A - P, 20, 2);
      break;
    case R_SPARC_WDISP22:
      check(S + A - P, -(1 << 23), 1 << 23);
      *(ub32 *)loc |= bits(S + A - P, 23, 2);
      break;
    case R_SPARC_WDISP30:
    case R_SPARC_WPLT30:
      check(S + A - P, -(1LL << 31), 1LL << 31);
      *(ub32 *)loc |= bits(S + A - P, 31, 2);
      break;
    case R_SPARC_HI22:
    case R_SPARC_HIPLT22:
    case R_SPARC_LM22:
      *(ub32 *)loc |= bits(S + A, 31, 10);
      break;
    case R_SPARC_GOT10:
      *(ub32 *)loc |= bits(G, 9, 0);
      break;
    case R_SPARC_GOT13:
      check(G, 0, 1 << 12);
      *(ub32 *)loc |= bits(G, 12, 0);
      break;
    case R_SPARC_GOT22:
      *(ub32 *)loc |= bits(G, 31, 10);
      break;
    case R_SPARC_GOTDATA_HIX22: {
      i64 val = S + A - GOT;
      *(ub32 *)loc |= bits(val < 0 ? ~val : val, 31, 10);
      break;
    }
    case R_SPARC_GOTDATA_LOX10: {
      i64 val = S + A - GOT;
      *(ub32 *)loc |= bits(val, 9, 0) | (val < 0 ? 0b1'1100'0000'0000 : 0);
      break;
    }
    case R_SPARC_GOTDATA_OP_HIX22:
      // We always have to relax a GOT load to a load immediate if a
      // symbol is local, because R_SPARC_GOTDATA_OP cannot represent
      // an addend for a local symbol.
      if (sym.is_imported || sym.is_ifunc()) {
        *(ub32 *)loc |= bits(G, 31, 10);
      } else if (sym.is_absolute()) {
        i64 val = S + A;
        *(ub32 *)loc |= bits(val < 0 ? ~val : val, 31, 10);
      } else {
        i64 val = S + A - GOT;
        *(ub32 *)loc |= bits(val < 0 ? ~val : val, 31, 10);
      }
      break;
    case R_SPARC_GOTDATA_OP_LOX10: {
      if (sym.is_imported || sym.is_ifunc()) {
        *(ub32 *)loc |= bits(G, 9, 0);
      } else if (sym.is_absolute()) {
        i64 val = S + A;
        *(ub32 *)loc |= bits(val, 9, 0) | (val < 0 ? 0b1'1100'0000'0000 : 0);
      } else {
        i64 val = S + A - GOT;
        *(ub32 *)loc |= bits(val, 9, 0) | (val < 0 ? 0b1'1100'0000'0000 : 0);
      }
      break;
    }
    case R_SPARC_GOTDATA_OP:
      if (sym.is_imported || sym.is_ifunc())
        break;

      if (sym.is_absolute()) {
        // ldx [ %g2 + %g1 ], %g1  →  nop
        *(ub32 *)loc = 0x0100'0000;
      } else {
        // ldx [ %g2 + %g1 ], %g1  →  add %g2, %g1, %g1
        *(ub32 *)loc &= 0b00'11111'000000'11111'1'11111111'11111;
        *(ub32 *)loc |= 0b10'00000'000000'00000'0'00000000'00000;
      }
      break;
    case R_SPARC_PC10:
    case R_SPARC_PCPLT10:
      *(ub32 *)loc |= bits(S + A - P, 9, 0);
      break;
    case R_SPARC_PC22:
    case R_SPARC_PCPLT22:
    case R_SPARC_PC_LM22:
      *(ub32 *)loc |= bits(S + A - P, 31, 10);
      break;
    case R_SPARC_OLO10:
      *(ub32 *)loc |= bits(bits(S + A, 9, 0) + rel.r_type_data, 12, 0);
      break;
    case R_SPARC_HH22:
      *(ub32 *)loc |= bits(S + A, 63, 42);
      break;
    case R_SPARC_HM10:
      *(ub32 *)loc |= bits(S + A, 41, 32);
      break;
    case R_SPARC_PC_HH22:
      *(ub32 *)loc |= bits(S + A - P, 63, 42);
      break;
    case R_SPARC_PC_HM10:
      *(ub32 *)loc |= bits(S + A - P, 41, 32);
      break;
    case R_SPARC_HIX22:
      *(ub32 *)loc |= bits(~(S + A), 31, 10);
      break;
    case R_SPARC_LOX10:
      *(ub32 *)loc |= bits(S + A, 9, 0) | 0b1'1100'0000'0000;
      break;
    case R_SPARC_H44:
      *(ub32 *)loc |= bits(S + A, 43, 22);
      break;
    case R_SPARC_M44:
      *(ub32 *)loc |= bits(S + A, 21, 12);
      break;
    case R_SPARC_L44:
      *(ub32 *)loc |= bits(S + A, 11, 0);
      break;
    case R_SPARC_TLS_GD_HI22:
      *(ub32 *)loc |= bits(sym.get_tlsgd_addr(ctx) + A - GOT, 31, 10);
      break;
    case R_SPARC_TLS_GD_LO10:
      *(ub32 *)loc |= bits(sym.get_tlsgd_addr(ctx) + A - GOT, 9, 0);
      break;
    case R_SPARC_TLS_GD_CALL:
    case R_SPARC_TLS_LDM_CALL: {
      u64 addr;
      if (ctx.arg.is_static)
        addr = ctx.extra.tls_get_addr_sec->shdr.sh_addr;
      else
        addr = ctx.extra.tls_get_addr_sym->get_addr(ctx);

      *(ub32 *)loc |= bits(addr + A - P, 31, 2);
      break;
    }
    case R_SPARC_TLS_LDM_HI22:
      *(ub32 *)loc |= bits(ctx.got->get_tlsld_addr(ctx) + A - GOT, 31, 10);
      break;
    case R_SPARC_TLS_LDM_LO10:
      *(ub32 *)loc |= bits(ctx.got->get_tlsld_addr(ctx) + A - GOT, 9, 0);
      break;
    case R_SPARC_TLS_LDO_HIX22:
      *(ub32 *)loc |= bits(S + A - ctx.dtp_addr, 31, 10);
      break;
    case R_SPARC_TLS_LDO_LOX10:
      *(ub32 *)loc |= bits(S + A - ctx.dtp_addr, 9, 0);
      break;
    case R_SPARC_TLS_IE_HI22:
      *(ub32 *)loc |= bits(sym.get_gottp_addr(ctx) + A - GOT, 31, 10);
      break;
    case R_SPARC_TLS_IE_LO10:
      *(ub32 *)loc |= bits(sym.get_gottp_addr(ctx) + A - GOT, 9, 0);
      break;
    case R_SPARC_TLS_LE_HIX22:
      *(ub32 *)loc |= bits(~(S + A - ctx.tp_addr), 31, 10);
      break;
    case R_SPARC_TLS_LE_LOX10:
      *(ub32 *)loc |= bits(S + A - ctx.tp_addr, 9, 0) | 0b1'1100'0000'0000;
      break;
    case R_SPARC_SIZE32:
      *(ub32 *)loc = sym.esym().st_size + A;
      break;
    case R_SPARC_TLS_GD_ADD:
    case R_SPARC_TLS_LDM_ADD:
    case R_SPARC_TLS_LDO_ADD:
    case R_SPARC_TLS_IE_LD:
    case R_SPARC_TLS_IE_LDX:
    case R_SPARC_TLS_IE_ADD:
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
    case R_SPARC_64:
    case R_SPARC_UA64:
      if (std::optional<u64> val = get_tombstone(sym, frag))
        *(ub64 *)loc = *val;
      else
        *(ub64 *)loc = S + A;
      break;
    case R_SPARC_32:
    case R_SPARC_UA32: {
      i64 val = S + A;
      check(val, 0, 1LL << 32);
      *(ub32 *)loc = val;
      break;
    }
    case R_SPARC_TLS_DTPOFF32:
      *(ub32 *)loc = S + A - ctx.dtp_addr;
      break;
    case R_SPARC_TLS_DTPOFF64:
      *(ub64 *)loc = S + A - ctx.dtp_addr;
      break;
    default:
      Fatal(ctx) << *this << ": apply_reloc_nonalloc: " << rel;
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
    case R_SPARC_64:
      scan_dyn_absrel(ctx, sym, rel);
      break;
    case R_SPARC_8:
    case R_SPARC_5:
    case R_SPARC_6:
    case R_SPARC_7:
    case R_SPARC_10:
    case R_SPARC_11:
    case R_SPARC_13:
    case R_SPARC_16:
    case R_SPARC_22:
    case R_SPARC_32:
    case R_SPARC_REGISTER:
    case R_SPARC_UA16:
    case R_SPARC_UA32:
    case R_SPARC_UA64:
    case R_SPARC_PC_HM10:
    case R_SPARC_OLO10:
    case R_SPARC_LOX10:
    case R_SPARC_HM10:
    case R_SPARC_M44:
    case R_SPARC_HIX22:
    case R_SPARC_LO10:
    case R_SPARC_L44:
    case R_SPARC_LM22:
    case R_SPARC_HI22:
    case R_SPARC_H44:
    case R_SPARC_HH22:
      scan_absrel(ctx, sym, rel);
      break;
    case R_SPARC_PLT32:
    case R_SPARC_WPLT30:
    case R_SPARC_WDISP30:
    case R_SPARC_HIPLT22:
    case R_SPARC_LOPLT10:
    case R_SPARC_PCPLT32:
    case R_SPARC_PCPLT22:
    case R_SPARC_PCPLT10:
    case R_SPARC_PLT64:
      if (sym.is_imported)
        sym.flags |= NEEDS_PLT;
      break;
    case R_SPARC_GOT13:
    case R_SPARC_GOT10:
    case R_SPARC_GOT22:
    case R_SPARC_GOTDATA_HIX22:
      sym.flags |= NEEDS_GOT;
      break;
    case R_SPARC_GOTDATA_OP_HIX22:
      if (sym.is_imported)
        sym.flags |= NEEDS_GOT;
      break;
    case R_SPARC_DISP16:
    case R_SPARC_DISP32:
    case R_SPARC_DISP64:
    case R_SPARC_DISP8:
    case R_SPARC_PC10:
    case R_SPARC_PC22:
    case R_SPARC_PC_LM22:
    case R_SPARC_WDISP16:
    case R_SPARC_WDISP19:
    case R_SPARC_WDISP22:
    case R_SPARC_PC_HH22:
      scan_pcrel(ctx, sym, rel);
      break;
    case R_SPARC_TLS_GD_HI22:
      sym.flags |= NEEDS_TLSGD;
      break;
    case R_SPARC_TLS_LDM_HI22:
      ctx.needs_tlsld = true;
      break;
    case R_SPARC_TLS_IE_HI22:
      sym.flags |= NEEDS_GOTTP;
      break;
    case R_SPARC_TLS_GD_CALL:
    case R_SPARC_TLS_LDM_CALL:
      if (!ctx.arg.is_static && ctx.extra.tls_get_addr_sym->is_imported)
        ctx.extra.tls_get_addr_sym->flags |= NEEDS_PLT;
      break;
    case R_SPARC_TLS_LE_HIX22:
    case R_SPARC_TLS_LE_LOX10:
      check_tlsle(ctx, sym, rel);
      break;
    case R_SPARC_GOTDATA_OP_LOX10:
    case R_SPARC_GOTDATA_OP:
    case R_SPARC_GOTDATA_LOX10:
    case R_SPARC_TLS_GD_LO10:
    case R_SPARC_TLS_GD_ADD:
    case R_SPARC_TLS_LDM_LO10:
    case R_SPARC_TLS_LDM_ADD:
    case R_SPARC_TLS_LDO_HIX22:
    case R_SPARC_TLS_LDO_LOX10:
    case R_SPARC_TLS_LDO_ADD:
    case R_SPARC_TLS_IE_ADD:
    case R_SPARC_TLS_IE_LD:
    case R_SPARC_TLS_IE_LDX:
    case R_SPARC_TLS_IE_LO10:
    case R_SPARC_SIZE32:
      break;
    default:
      Fatal(ctx) << *this << ": scan_relocations: " << rel;
    }
  }
}

// __tls_get_addr is not defined by libc.a, so we can't use that function
// in statically-linked executables. This section provides a replacement.
void SparcTlsGetAddrSection::copy_buf(Context<E> &ctx) {
  ub32 *buf = (ub32 *)(ctx.buf + this->shdr.sh_offset);

  static const ub32 insn[] = {
    0x0300'0000, // sethi  %hi(TP_SIZE), %g1
    0x8210'6000, // or   %g1, %lo(TP_SIZE), %g1
    0x8221'c001, // sub  %g7, %g1, %g1
    0xd05a'2008, // ldx  [ %o0 + 8 ], %o0
    0x81c3'e008, // retl
    0x9000'4008, // add  %g1, %o0, %o0
  };

  assert(this->shdr.sh_size == sizeof(insn));
  memcpy(buf, insn, sizeof(insn));

  buf[0] |= bits(ctx.tp_addr - ctx.tls_begin, 31, 10);
  buf[1] |= bits(ctx.tp_addr - ctx.tls_begin, 9, 0);
}

} // namespace mold::elf
