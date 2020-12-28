/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/log/check.h"
#include "third_party/xed/x86.h"
#include "tool/build/lib/address.h"
#include "tool/build/lib/endian.h"
#include "tool/build/lib/machine.h"
#include "tool/build/lib/memory.h"
#include "tool/build/lib/modrm.h"
#include "tool/build/lib/throw.h"

struct AddrSeg LoadEffectiveAddress(const struct Machine *m, uint32_t rde) {
  uint8_t *s = m->ds;
  uint64_t i = m->xedd->op.disp;
  DCHECK(!IsModrmRegister(rde));
  if (Eamode(rde) != XED_MODE_REAL) {
    if (!SibExists(rde)) {
      if (IsRipRelative(rde)) {
        if (Mode(rde) == XED_MODE_LONG) {
          i += m->ip;
        }
      } else {
        i += Read64(RegRexbRm(m, rde));
        if (RexbRm(rde) == 4 || RexbRm(rde) == 5) {
          s = m->ss;
        }
      }
    } else {
      if (SibHasBase(m->xedd, rde)) {
        i += Read64(RegRexbBase(m, rde));
        if (RexbBase(m, rde) == 4 || RexbBase(m, rde) == 5) {
          s = m->ss;
        }
      }
      if (SibHasIndex(m->xedd)) {
        i += Read64(RegRexxIndex(m)) << m->xedd->op.scale;
      }
    }
    if (Eamode(rde) == XED_MODE_LEGACY) {
      i &= 0xffffffff;
    }
  } else {
    switch (ModrmRm(rde)) {
      case 0:
        i += Read16(m->bx);
        i += Read16(m->si);
        break;
      case 1:
        i += Read16(m->bx);
        i += Read16(m->di);
        break;
      case 2:
        s = m->ss;
        i += Read16(m->bp);
        i += Read16(m->si);
        break;
      case 3:
        s = m->ss;
        i += Read16(m->bp);
        i += Read16(m->di);
        break;
      case 4:
        i += Read16(m->si);
        break;
      case 5:
        i += Read16(m->di);
        break;
      case 6:
        if (ModrmMod(rde)) {
          s = m->ss;
          i += Read16(m->bp);
        }
        break;
      case 7:
        i += Read16(m->bx);
        break;
      default:
        unreachable;
    }
    i &= 0xffff;
  }
  return (struct AddrSeg){i, s};
}

int64_t ComputeAddress(const struct Machine *m, uint32_t rde) {
  struct AddrSeg ea;
  ea = LoadEffectiveAddress(m, rde);
  return AddSegment(m, rde, ea.addr, ea.seg);
}

void *ComputeReserveAddressRead(struct Machine *m, uint32_t rde, size_t n) {
  int64_t v;
  v = ComputeAddress(m, rde);
  SetReadAddr(m, v, n);
  return ReserveAddress(m, v, n);
}

void *ComputeReserveAddressRead1(struct Machine *m, uint32_t rde) {
  return ComputeReserveAddressRead(m, rde, 1);
}

void *ComputeReserveAddressRead4(struct Machine *m, uint32_t rde) {
  return ComputeReserveAddressRead(m, rde, 4);
}

void *ComputeReserveAddressRead8(struct Machine *m, uint32_t rde) {
  return ComputeReserveAddressRead(m, rde, 8);
}

void *ComputeReserveAddressWrite(struct Machine *m, uint32_t rde, size_t n) {
  int64_t v;
  v = ComputeAddress(m, rde);
  SetWriteAddr(m, v, n);
  return ReserveAddress(m, v, n);
}

void *ComputeReserveAddressWrite1(struct Machine *m, uint32_t rde) {
  return ComputeReserveAddressWrite(m, rde, 1);
}

void *ComputeReserveAddressWrite4(struct Machine *m, uint32_t rde) {
  return ComputeReserveAddressWrite(m, rde, 4);
}

void *ComputeReserveAddressWrite8(struct Machine *m, uint32_t rde) {
  return ComputeReserveAddressWrite(m, rde, 8);
}

uint8_t *GetModrmRegisterMmPointerRead(struct Machine *m, uint32_t rde,
                                       size_t n) {
  if (IsModrmRegister(rde)) {
    return MmRm(m, rde);
  } else {
    return ComputeReserveAddressRead(m, rde, n);
  }
}

uint8_t *GetModrmRegisterMmPointerRead8(struct Machine *m, uint32_t rde) {
  return GetModrmRegisterMmPointerRead(m, rde, 8);
}

uint8_t *GetModrmRegisterMmPointerWrite(struct Machine *m, uint32_t rde,
                                        size_t n) {
  if (IsModrmRegister(rde)) {
    return MmRm(m, rde);
  } else {
    return ComputeReserveAddressWrite(m, rde, n);
  }
}

uint8_t *GetModrmRegisterMmPointerWrite8(struct Machine *m, uint32_t rde) {
  return GetModrmRegisterMmPointerWrite(m, rde, 8);
}

uint8_t *GetModrmRegisterBytePointerRead(struct Machine *m, uint32_t rde) {
  int64_t v;
  if (IsModrmRegister(rde)) {
    return ByteRexbRm(m, rde);
  } else {
    return ComputeReserveAddressRead1(m, rde);
  }
}

uint8_t *GetModrmRegisterBytePointerWrite(struct Machine *m, uint32_t rde) {
  int64_t v;
  if (IsModrmRegister(rde)) {
    return ByteRexbRm(m, rde);
  } else {
    return ComputeReserveAddressWrite1(m, rde);
  }
}

uint8_t *GetModrmRegisterWordPointerRead(struct Machine *m, uint32_t rde,
                                         size_t n) {
  if (IsModrmRegister(rde)) {
    return RegRexbRm(m, rde);
  } else {
    return ComputeReserveAddressRead(m, rde, n);
  }
}

uint8_t *GetModrmRegisterWordPointerRead2(struct Machine *m, uint32_t rde) {
  return GetModrmRegisterWordPointerRead(m, rde, 2);
}

uint8_t *GetModrmRegisterWordPointerRead4(struct Machine *m, uint32_t rde) {
  return GetModrmRegisterWordPointerRead(m, rde, 4);
}

uint8_t *GetModrmRegisterWordPointerRead8(struct Machine *m, uint32_t rde) {
  return GetModrmRegisterWordPointerRead(m, rde, 8);
}

uint8_t *GetModrmRegisterWordPointerReadOsz(struct Machine *m, uint32_t rde) {
  if (!Osz(rde)) {
    return GetModrmRegisterWordPointerRead8(m, rde);
  } else {
    return GetModrmRegisterWordPointerRead2(m, rde);
  }
}

uint8_t *GetModrmRegisterWordPointerReadOszRexw(struct Machine *m,
                                                uint32_t rde) {
  if (Rexw(rde)) {
    return GetModrmRegisterWordPointerRead8(m, rde);
  } else if (!Osz(rde)) {
    return GetModrmRegisterWordPointerRead4(m, rde);
  } else {
    return GetModrmRegisterWordPointerRead2(m, rde);
  }
}

uint8_t *GetModrmRegisterWordPointerWrite(struct Machine *m, uint32_t rde,
                                          size_t n) {
  if (IsModrmRegister(rde)) {
    return RegRexbRm(m, rde);
  } else {
    return ComputeReserveAddressWrite(m, rde, n);
  }
}

uint8_t *GetModrmRegisterWordPointerWrite2(struct Machine *m, uint32_t rde) {
  return GetModrmRegisterWordPointerWrite(m, rde, 2);
}

uint8_t *GetModrmRegisterWordPointerWrite4(struct Machine *m, uint32_t rde) {
  return GetModrmRegisterWordPointerWrite(m, rde, 4);
}

uint8_t *GetModrmRegisterWordPointerWrite8(struct Machine *m, uint32_t rde) {
  return GetModrmRegisterWordPointerWrite(m, rde, 8);
}

uint8_t *GetModrmRegisterWordPointerWriteOszRexw(struct Machine *m,
                                                 uint32_t rde) {
  if (Rexw(rde)) {
    return GetModrmRegisterWordPointerWrite(m, rde, 8);
  } else if (!Osz(rde)) {
    return GetModrmRegisterWordPointerWrite(m, rde, 4);
  } else {
    return GetModrmRegisterWordPointerWrite(m, rde, 2);
  }
}

uint8_t *GetModrmRegisterWordPointerWriteOsz(struct Machine *m, uint32_t rde) {
  if (!Osz(rde)) {
    return GetModrmRegisterWordPointerWrite(m, rde, 8);
  } else {
    return GetModrmRegisterWordPointerWrite(m, rde, 2);
  }
}

uint8_t *GetModrmRegisterXmmPointerRead(struct Machine *m, uint32_t rde,
                                        size_t n) {
  if (IsModrmRegister(rde)) {
    return XmmRexbRm(m, rde);
  } else {
    return ComputeReserveAddressRead(m, rde, n);
  }
}

uint8_t *GetModrmRegisterXmmPointerRead4(struct Machine *m, uint32_t rde) {
  return GetModrmRegisterXmmPointerRead(m, rde, 4);
}

uint8_t *GetModrmRegisterXmmPointerRead8(struct Machine *m, uint32_t rde) {
  return GetModrmRegisterXmmPointerRead(m, rde, 8);
}

uint8_t *GetModrmRegisterXmmPointerRead16(struct Machine *m, uint32_t rde) {
  return GetModrmRegisterXmmPointerRead(m, rde, 16);
}

uint8_t *GetModrmRegisterXmmPointerWrite(struct Machine *m, uint32_t rde,
                                         size_t n) {
  if (IsModrmRegister(rde)) {
    return XmmRexbRm(m, rde);
  } else {
    return ComputeReserveAddressWrite(m, rde, n);
  }
}

uint8_t *GetModrmRegisterXmmPointerWrite4(struct Machine *m, uint32_t rde) {
  return GetModrmRegisterXmmPointerWrite(m, rde, 4);
}

uint8_t *GetModrmRegisterXmmPointerWrite8(struct Machine *m, uint32_t rde) {
  return GetModrmRegisterXmmPointerWrite(m, rde, 8);
}

uint8_t *GetModrmRegisterXmmPointerWrite16(struct Machine *m, uint32_t rde) {
  return GetModrmRegisterXmmPointerWrite(m, rde, 16);
}
