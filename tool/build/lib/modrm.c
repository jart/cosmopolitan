/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/log/check.h"
#include "third_party/xed/x86.h"
#include "tool/build/lib/address.h"
#include "tool/build/lib/endian.h"
#include "tool/build/lib/machine.h"
#include "tool/build/lib/memory.h"
#include "tool/build/lib/modrm.h"
#include "tool/build/lib/throw.h"

/**
 * Byte register offsets.
 *
 * for (i = 0; i < 2; ++i) {      // rex
 *   for (j = 0; j < 2; ++j) {    // rexb, or rexr
 *     for (k = 0; k < 8; ++k) {  // reg, rm, or srm
 *       kByteReg[i << 4 | j << 3 | k] =
 *           i ? (j << 3 | k) * 8 : (k & 0b11) * 8 + ((k & 0b100) >> 2);
 *     }
 *   }
 * }
 */
const uint8_t kByteReg[32] = {0x00, 0x08, 0x10, 0x18, 0x01, 0x09, 0x11, 0x19,
                              0x00, 0x08, 0x10, 0x18, 0x01, 0x09, 0x11, 0x19,
                              0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38,
                              0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78};

int64_t ComputeAddress(const struct Machine *m, uint32_t rde) {
  uint64_t i;
  uint8_t *s;
  DCHECK(!IsModrmRegister(rde));
  s = m->ds;
  i = m->xedd->op.disp;
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
  return AddSegment(m, rde, i, s);
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
