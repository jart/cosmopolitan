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
#include "libc/log/log.h"
#include "tool/build/lib/endian.h"
#include "tool/build/lib/memory.h"
#include "tool/build/lib/modrm.h"
#include "tool/build/lib/op101.h"
#include "tool/build/lib/throw.h"
#include "tool/build/lib/time.h"

static void StoreDescriptorTable(struct Machine *m, uint32_t rde,
                                 uint16_t limit, uint64_t base) {
  uint64_t l;
  l = ComputeAddress(m, rde);
  if (l + 10 <= m->real.n) {
    Write16(m->real.p + l, limit);
    if (Rexw(rde)) {
      Write64(m->real.p + l + 2, base);
      SetWriteAddr(m, l, 10);
    } else if (!Osz(rde)) {
      Write32(m->real.p + l + 2, base);
      SetWriteAddr(m, l, 6);
    } else {
      Write16(m->real.p + l + 2, base);
      SetWriteAddr(m, l, 4);
    }
  } else {
    ThrowSegmentationFault(m, l);
  }
}

static void LoadDescriptorTable(struct Machine *m, uint32_t rde,
                                uint16_t *out_limit, uint64_t *out_base) {
  uint16_t limit;
  uint64_t l, base;
  l = ComputeAddress(m, rde);
  if (l + 10 <= m->real.n) {
    limit = Read16(m->real.p + l);
    if (Rexw(rde)) {
      base = Read64(m->real.p + l + 2) & 0x00ffffff;
      SetReadAddr(m, l, 10);
    } else if (!Osz(rde)) {
      base = Read32(m->real.p + l + 2);
      SetReadAddr(m, l, 6);
    } else {
      base = Read16(m->real.p + l + 2);
      SetReadAddr(m, l, 4);
    }
    if (base + limit <= m->real.n) {
      *out_limit = limit;
      *out_base = base;
    } else {
      ThrowProtectionFault(m);
    }
  } else {
    ThrowSegmentationFault(m, l);
  }
}

static void SgdtMs(struct Machine *m, uint32_t rde) {
  StoreDescriptorTable(m, rde, m->gdt_limit, m->gdt_base);
}

static void LgdtMs(struct Machine *m, uint32_t rde) {
  LoadDescriptorTable(m, rde, &m->gdt_limit, &m->gdt_base);
  LOGF("set gdt %p lim %,d", m->gdt_base, m->gdt_limit);
}

static void SidtMs(struct Machine *m, uint32_t rde) {
  StoreDescriptorTable(m, rde, m->idt_limit, m->idt_base);
}

static void LidtMs(struct Machine *m, uint32_t rde) {
  LoadDescriptorTable(m, rde, &m->idt_limit, &m->idt_base);
  LOGF("set idt %p lim %,d", m->idt_base, m->idt_limit);
}

static void Monitor(struct Machine *m, uint32_t rde) {
}

static void Mwait(struct Machine *m, uint32_t rde) {
}

static void Swapgs(struct Machine *m, uint32_t rde) {
}

static void Vmcall(struct Machine *m, uint32_t rde) {
}

static void Vmlaunch(struct Machine *m, uint32_t rde) {
}

static void Vmresume(struct Machine *m, uint32_t rde) {
}

static void Vmxoff(struct Machine *m, uint32_t rde) {
}

static void InvlpgM(struct Machine *m, uint32_t rde) {
  ResetTlb(m);
}

static void Smsw(struct Machine *m, uint32_t rde, bool ismem) {
  if (ismem) {
    Write16(GetModrmRegisterWordPointerWrite2(m, rde), m->cr0);
  } else if (Rexw(rde)) {
    Write64(RegRexrReg(m, rde), m->cr0);
  } else if (!Osz(rde)) {
    Write64(RegRexrReg(m, rde), m->cr0 & 0xffffffff);
  } else {
    Write16(RegRexrReg(m, rde), m->cr0);
  }
}

static void Lmsw(struct Machine *m, uint32_t rde) {
  m->cr0 = Read16(GetModrmRegisterWordPointerRead2(m, rde));
}

void Op101(struct Machine *m, uint32_t rde) {
  bool ismem;
  ismem = !IsModrmRegister(rde);
  switch (ModrmReg(rde)) {
    case 0:
      if (ismem) {
        SgdtMs(m, rde);
      } else {
        switch (ModrmRm(rde)) {
          case 1:
            Vmcall(m, rde);
            break;
          case 2:
            Vmlaunch(m, rde);
            break;
          case 3:
            Vmresume(m, rde);
            break;
          case 4:
            Vmxoff(m, rde);
            break;
          default:
            OpUd(m, rde);
        }
      }
      break;
    case 1:
      if (ismem) {
        SidtMs(m, rde);
      } else {
        switch (ModrmRm(rde)) {
          case 0:
            Monitor(m, rde);
            break;
          case 1:
            Mwait(m, rde);
            break;
          default:
            OpUd(m, rde);
        }
      }
      break;
    case 2:
      if (ismem) {
        LgdtMs(m, rde);
      } else {
        OpUd(m, rde);
      }
      break;
    case 3:
      if (ismem) {
        LidtMs(m, rde);
      } else {
        OpUd(m, rde);
      }
      break;
    case 4:
      Smsw(m, rde, ismem);
      break;
    case 6:
      Lmsw(m, rde);
      break;
    case 7:
      if (ismem) {
        InvlpgM(m, rde);
      } else {
        switch (ModrmRm(rde)) {
          case 0:
            Swapgs(m, rde);
            break;
          case 1:
            OpRdtscp(m, rde);
            break;
          default:
            OpUd(m, rde);
        }
      }
      break;
    default:
      OpUd(m, rde);
  }
}
