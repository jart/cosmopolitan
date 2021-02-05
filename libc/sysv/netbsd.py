#!/usr/bin/env python
# -*- coding: utf-8 -*-
import re
import sys
NETBSD = {}
MAXBIT = 0

def bsr(x):
  r = -1
  while x:
    r += 1
    x >>= 1
  return r

def binify(x):
  return "%016x" % (x)

lines = open('libc/sysv/netbsd.txt').read().split('\n')
for line in lines:
  if 'STD' not in line: continue
  m = re.search(r'^(\d+).*?([_0-9A-Za-z]+)\(', line)
  if not m: continue
  NETBSD[m.group(2)] = int(m.group(1))

lines = open('libc/sysv/syscalls.sh').read().split('\n')
for line in lines:
  if line.startswith('scall'):
    fields = line.split()
    name = fields[1]
    if name.startswith("'"): name = name[1:]
    if name.endswith("'"): name = name[:-1]
    if name.startswith("__"): name = name[2:]
    if name.startswith("sys_"): name = name[4:]
    if name.endswith("_bsd"): name = name[:-4]
    if name.endswith("_freebsd"): name = name[:-8]
    numbas  = fields[2][2:]
    numbers = int(numbas, 16)
    systemd = (numbers >> 000) & 0xffff
    xnu     = (numbers >> 020) & 0x0fff
    xnukind = (numbers >> 034) & 0xf
    freebsd = (numbers >> 040) & 0xffff
    openbsd = (numbers >> 060) & 0xffff
    netbsd  = NETBSD.get(name, 0xffff)
    com = ""
    if netbsd != 0xffff:
      if netbsd == systemd: com += " SYSTEMD"
      if netbsd == xnu:     com += " xnu"
      if netbsd == freebsd: com += " freebsd"
      if netbsd == openbsd: com += " openbsd"
    getbit = lambda x: 0 if x == 0xffff else bsr(x)
    maxbit = max([getbit(systemd), getbit(xnu), getbit(freebsd), getbit(openbsd), getbit(netbsd)])
    MAXBIT = max(maxbit, MAXBIT)
    # print "%-30s %04x %04x %04x %04x %04x  %3d %3d %s" % (name, systemd, xnu, freebsd, openbsd, netbsd, maxbit, MAXBIT, com)
    systemd &= 0b111111111111
    xnu     &= 0b111111111111
    freebsd &= 0b111111111111
    openbsd &= 0b111111111111
    netbsd  &= 0b111111111111
    numba = netbsd << (4*13) | openbsd << (4*10) | freebsd << (4*7) | xnukind << (4*6) | xnu << (4*3) | systemd
    assert numbas in line
    line = line.replace(numbas, binify(numba))
  print line
