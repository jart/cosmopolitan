/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/fmt/itoa.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/macros.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/cap.h"

static const struct thatispacked {
  unsigned char x;
  const char *s;
} kCapabilityName[] = {
    {CAP_CHOWN, "CHOWN"},                            //
    {CAP_DAC_OVERRIDE, "DAC_OVERRIDE"},              //
    {CAP_DAC_READ_SEARCH, "DAC_READ_SEARCH"},        //
    {CAP_FOWNER, "FOWNER"},                          //
    {CAP_FSETID, "FSETID"},                          //
    {CAP_KILL, "KILL"},                              //
    {CAP_SETGID, "SETGID"},                          //
    {CAP_SETUID, "SETUID"},                          //
    {CAP_SETPCAP, "SETPCAP"},                        //
    {CAP_LINUX_IMMUTABLE, "LINUX_IMMUTABLE"},        //
    {CAP_NET_BIND_SERVICE, "NET_BIND_SERVICE"},      //
    {CAP_NET_BROADCAST, "NET_BROADCAST"},            //
    {CAP_NET_ADMIN, "NET_ADMIN"},                    //
    {CAP_NET_RAW, "NET_RAW"},                        //
    {CAP_IPC_LOCK, "IPC_LOCK"},                      //
    {CAP_IPC_OWNER, "IPC_OWNER"},                    //
    {CAP_SYS_MODULE, "SYS_MODULE"},                  //
    {CAP_SYS_RAWIO, "SYS_RAWIO"},                    //
    {CAP_SYS_CHROOT, "SYS_CHROOT"},                  //
    {CAP_SYS_PTRACE, "SYS_PTRACE"},                  //
    {CAP_SYS_PACCT, "SYS_PACCT"},                    //
    {CAP_SYS_ADMIN, "SYS_ADMIN"},                    //
    {CAP_SYS_BOOT, "SYS_BOOT"},                      //
    {CAP_SYS_NICE, "SYS_NICE"},                      //
    {CAP_SYS_RESOURCE, "SYS_RESOURCE"},              //
    {CAP_SYS_TIME, "SYS_TIME"},                      //
    {CAP_SYS_TTY_CONFIG, "SYS_TTY_CONFIG"},          //
    {CAP_MKNOD, "MKNOD"},                            //
    {CAP_LEASE, "LEASE"},                            //
    {CAP_AUDIT_WRITE, "AUDIT_WRITE"},                //
    {CAP_AUDIT_CONTROL, "AUDIT_CONTROL"},            //
    {CAP_SETFCAP, "SETFCAP"},                        //
    {CAP_MAC_OVERRIDE, "MAC_OVERRIDE"},              //
    {CAP_MAC_ADMIN, "MAC_ADMIN"},                    //
    {CAP_SYSLOG, "SYSLOG"},                          //
    {CAP_WAKE_ALARM, "WAKE_ALARM"},                  //
    {CAP_BLOCK_SUSPEND, "BLOCK_SUSPEND"},            //
    {CAP_AUDIT_READ, "AUDIT_READ"},                  //
    {CAP_PERFMON, "PERFMON"},                        //
    {CAP_BPF, "BPF"},                                //
    {CAP_CHECKPOINT_RESTORE, "CHECKPOINT_RESTORE"},  //
};

const char *(DescribeCapability)(char buf[32], int x) {
  int i;
  for (i = 0; i < ARRAYLEN(kCapabilityName); ++i) {
    if (kCapabilityName[i].x == x) {
      stpcpy(stpcpy(buf, "CAP_"), kCapabilityName[i].s);
      return buf;
    }
  }
  FormatInt32(buf, x);
  return buf;
}
