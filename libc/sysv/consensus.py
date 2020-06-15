#!/usr/bin/env python
import sys

lineno = 0

def atoi(s):
  try:
    if s == '0':
      return 0
    elif s.startswith('0x'):
      return int(s[2:], 16)
    elif s.startswith('0b'):
      return int(s[2:], 2)
    elif s.startswith('0'):
      return int(s[1:], 8)
    return int(s)
  except ValueError:
    sys.stderr.write('error: %s on line %d\n' % (s, lineno))
    sys.exit(1)

for line in open('consts.sh'):
  f = line.split()
  lineno = lineno + 1
  if len(f) >= 8 and f[0] == 'syscon':
    linux = atoi(f[3])
    xnu = atoi(f[4])
    freebsd = atoi(f[5])
    openbsd = atoi(f[6])
    windows = atoi(f[7])
    if linux == xnu and xnu == freebsd and freebsd == openbsd and openbsd == windows:
      sys.stdout.write('%s\t%s\n' % (f[1], f[2]))
