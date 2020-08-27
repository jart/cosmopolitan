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
#include "tool/build/lib/endian.h"
#include "tool/build/lib/machine.h"
#include "tool/build/lib/memory.h"
#include "tool/build/lib/modrm.h"

/**
 * Computes virtual address based on modrm and sib bytes.
 */
int64_t ComputeAddress(const struct Machine *m, uint32_t rde) {
  uint64_t i;
  DCHECK(m->xedd->op.has_modrm);
  DCHECK(!IsModrmRegister(rde));
  i = m->xedd->op.disp;
  if (!SibExists(rde)) {
    if (IsRipRelative(rde)) {
      i += m->ip;
    } else {
      i += Read64(RegRexbRm(m, rde));
    }
  } else {
    DCHECK(m->xedd->op.has_sib);
    if (SibHasBase(rde)) {
      i += Read64(RegRexbBase(m, rde));
    }
    if (SibHasIndex(rde)) {
      i += Read64(RegRexxIndex(m, rde)) << m->xedd->op.scale;
    }
  }
  i += GetSegment(m);
  if (Asz(rde)) i &= 0xffffffff;
  return i;
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
