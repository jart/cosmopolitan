#!/usr/bin/env python
# -*- coding: utf-8 -*-
lines = open('libc/sysv/syscalls.sh').read().split('\n')
for line in lines:
  if not line.startswith('scall'):
    continue
  fields = line.split()
  name = fields[1]
  if name.startswith("'"): name = name[1:]
  if name.endswith("'"): name = name[:-1]
  if name.startswith("__"): name = name[2:]
  if name.endswith("$sysv"): name = name[:-5]
  if name.endswith("$bsd"): name = name[:-4]
  if name.endswith("$freebsd"): name = name[:-8]
  numbers = int(fields[2][2:], 16)
  systemd = (numbers >> 000) & 0xffff
  xnu     = (numbers >> 020) & 0x0fff | ((numbers >> 28) & 0xf) << 20
  freebsd = (numbers >> 040) & 0xffff
  openbsd = (numbers >> 060) & 0xffff
  print "syscon\tnr\t__NR_%s\t\t\t\t0x%04x\t\t\t0x%07x\t\t0x%04x\t\t\t0x%04x\t\t\t-1" % (name, systemd, xnu, freebsd, openbsd)
