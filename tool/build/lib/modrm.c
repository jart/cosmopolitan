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
int64_t ComputeAddress(const struct Machine *m) {
  uint64_t i;
  DCHECK(m->xedd->op.has_modrm);
  DCHECK(!IsModrmRegister(m->xedd));
  i = m->xedd->op.disp;
  if (!SibExists(m->xedd)) {
    i += IsRipRelative(m->xedd) ? m->ip : Read64(RegRexbRm(m));
  } else {
    DCHECK(m->xedd->op.has_sib);
    if (SibHasBase(m->xedd)) i += Read64(RegRexbBase(m));
    if (SibHasIndex(m->xedd)) i += Read64(RegRexxIndex(m)) << m->xedd->op.scale;
  }
  i += GetSegment(m);
  if (Asz(m->xedd)) i &= 0xffffffff;
  return i;
}

void *ComputeReserveAddressRead(struct Machine *m, size_t n) {
  int64_t v;
  v = ComputeAddress(m);
  SetReadAddr(m, v, n);
  return ReserveAddress(m, v, n);
}

void *ComputeReserveAddressRead1(struct Machine *m) {
  return ComputeReserveAddressRead(m, 1);
}

void *ComputeReserveAddressRead8(struct Machine *m) {
  return ComputeReserveAddressRead(m, 8);
}

void *ComputeReserveAddressWrite(struct Machine *m, size_t n) {
  int64_t v;
  v = ComputeAddress(m);
  SetWriteAddr(m, v, n);
  return ReserveAddress(m, v, n);
}

void *ComputeReserveAddressWrite1(struct Machine *m) {
  return ComputeReserveAddressWrite(m, 1);
}

void *ComputeReserveAddressWrite4(struct Machine *m) {
  return ComputeReserveAddressWrite(m, 4);
}

void *ComputeReserveAddressWrite8(struct Machine *m) {
  return ComputeReserveAddressWrite(m, 8);
}

uint8_t *GetModrmRegisterMmPointerRead(struct Machine *m, size_t n) {
  if (IsModrmRegister(m->xedd)) {
    return MmRm(m);
  } else {
    return ComputeReserveAddressRead(m, n);
  }
}

uint8_t *GetModrmRegisterMmPointerRead8(struct Machine *m) {
  return GetModrmRegisterMmPointerRead(m, 8);
}

uint8_t *GetModrmRegisterMmPointerWrite(struct Machine *m, size_t n) {
  if (IsModrmRegister(m->xedd)) {
    return MmRm(m);
  } else {
    return ComputeReserveAddressWrite(m, n);
  }
}

uint8_t *GetModrmRegisterMmPointerWrite8(struct Machine *m) {
  return GetModrmRegisterMmPointerWrite(m, 8);
}

uint8_t *GetModrmRegisterBytePointerRead(struct Machine *m) {
  int64_t v;
  if (IsModrmRegister(m->xedd)) {
    return ByteRexbRm(m);
  } else {
    return ComputeReserveAddressRead1(m);
  }
}

uint8_t *GetModrmRegisterBytePointerWrite(struct Machine *m) {
  int64_t v;
  if (IsModrmRegister(m->xedd)) {
    return ByteRexbRm(m);
  } else {
    return ComputeReserveAddressWrite1(m);
  }
}

uint8_t *GetModrmRegisterWordPointerRead(struct Machine *m, size_t n) {
  if (IsModrmRegister(m->xedd)) {
    return RegRexbRm(m);
  } else {
    return ComputeReserveAddressRead(m, n);
  }
}

uint8_t *GetModrmRegisterWordPointerRead2(struct Machine *m) {
  return GetModrmRegisterWordPointerRead(m, 2);
}

uint8_t *GetModrmRegisterWordPointerRead4(struct Machine *m) {
  return GetModrmRegisterWordPointerRead(m, 4);
}

uint8_t *GetModrmRegisterWordPointerRead8(struct Machine *m) {
  return GetModrmRegisterWordPointerRead(m, 8);
}

uint8_t *GetModrmRegisterWordPointerReadOsz(struct Machine *m) {
  if (!Osz(m->xedd)) {
    return GetModrmRegisterWordPointerRead8(m);
  } else {
    return GetModrmRegisterWordPointerRead2(m);
  }
}

uint8_t *GetModrmRegisterWordPointerReadOszRexw(struct Machine *m) {
  if (Rexw(m->xedd)) {
    return GetModrmRegisterWordPointerRead8(m);
  } else if (!Osz(m->xedd)) {
    return GetModrmRegisterWordPointerRead4(m);
  } else {
    return GetModrmRegisterWordPointerRead2(m);
  }
}

uint8_t *GetModrmRegisterWordPointerWrite(struct Machine *m, size_t n) {
  if (IsModrmRegister(m->xedd)) {
    return RegRexbRm(m);
  } else {
    return ComputeReserveAddressWrite(m, n);
  }
}

uint8_t *GetModrmRegisterWordPointerWrite4(struct Machine *m) {
  return GetModrmRegisterWordPointerWrite(m, 4);
}

uint8_t *GetModrmRegisterWordPointerWrite8(struct Machine *m) {
  return GetModrmRegisterWordPointerWrite(m, 8);
}

uint8_t *GetModrmRegisterWordPointerWriteOszRexw(struct Machine *m) {
  if (Rexw(m->xedd)) {
    return GetModrmRegisterWordPointerWrite(m, 8);
  } else if (!Osz(m->xedd)) {
    return GetModrmRegisterWordPointerWrite(m, 4);
  } else {
    return GetModrmRegisterWordPointerWrite(m, 2);
  }
}

uint8_t *GetModrmRegisterWordPointerWriteOsz(struct Machine *m) {
  if (!Osz(m->xedd)) {
    return GetModrmRegisterWordPointerWrite(m, 8);
  } else {
    return GetModrmRegisterWordPointerWrite(m, 2);
  }
}

uint8_t *GetModrmRegisterXmmPointerRead(struct Machine *m, size_t n) {
  if (IsModrmRegister(m->xedd)) {
    return XmmRexbRm(m);
  } else {
    return ComputeReserveAddressRead(m, n);
  }
}

uint8_t *GetModrmRegisterXmmPointerRead4(struct Machine *m) {
  return GetModrmRegisterXmmPointerRead(m, 4);
}

uint8_t *GetModrmRegisterXmmPointerRead8(struct Machine *m) {
  return GetModrmRegisterXmmPointerRead(m, 8);
}

uint8_t *GetModrmRegisterXmmPointerRead16(struct Machine *m) {
  return GetModrmRegisterXmmPointerRead(m, 16);
}

uint8_t *GetModrmRegisterXmmPointerWrite(struct Machine *m, size_t n) {
  if (IsModrmRegister(m->xedd)) {
    return XmmRexbRm(m);
  } else {
    return ComputeReserveAddressWrite(m, n);
  }
}

uint8_t *GetModrmRegisterXmmPointerWrite4(struct Machine *m) {
  return GetModrmRegisterXmmPointerWrite(m, 4);
}

uint8_t *GetModrmRegisterXmmPointerWrite8(struct Machine *m) {
  return GetModrmRegisterXmmPointerWrite(m, 8);
}

uint8_t *GetModrmRegisterXmmPointerWrite16(struct Machine *m) {
  return GetModrmRegisterXmmPointerWrite(m, 16);
}
