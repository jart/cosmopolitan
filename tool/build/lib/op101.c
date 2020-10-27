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
#include "tool/build/lib/endian.h"
#include "tool/build/lib/modrm.h"
#include "tool/build/lib/op101.h"
#include "tool/build/lib/throw.h"
#include "tool/build/lib/time.h"

static void SgdtMs(struct Machine *m, uint32_t rde) {
}

static void LgdtMs(struct Machine *m, uint32_t rde) {
}

static void SidtMs(struct Machine *m, uint32_t rde) {
}

static void LidtMs(struct Machine *m, uint32_t rde) {
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
