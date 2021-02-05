import sys

H = '/home/jart/vendor/netbsd/sys/sys/ioctl.h'
G = 'errno'

MAGNUMS = {}

def ParseInt(x):
  s = 1
  if x.startswith('-'):
    x = x[1:]
    s = -1
  if x.startswith('0x'):
    x = int(x[2:], 16)
  elif x.startswith('0b'):
    x = int(x[2:], 2)
  elif x.startswith('0'):
    x = int(x, 8)
  else:
    x = int(x, 10)
  return x * s

def FormatInt(x):
  if x == -1:
    return "-1"
  if -128 <= x < 128:
    return "%d" % (x)
  else:
    return "%#x" % (x)

for line in open(H):
  if line.startswith('#define'):
    a = line.split()
    if len(a) >= 3:
      try:
        MAGNUMS[a[1]] = ParseInt(a[2])
      except ValueError:
        pass

GROUP_COLUMN = 2
NAME_COLUMN = 4
SYSTEMD_COLUMN = 6
XNU_COLUMN = 8
FREEBSD_COLUMN = 10
OPENBSD_COLUMN = 12
NETBSD_COLUMN = 14

print """\
#	The Fifth Bell System, Community Edition
#	> catalogue of carnage
#
#	group	name					GNU/Systemd		XNU's Not UNIX		FreeBSD			OpenBSD			NetBSD			XENIX			Commentary"""

for line in open('libc/sysv/consts.sh'):
  if not line.startswith('syscon'): continue
  fields = [""]
  i = 0
  t = 0
  for i in range(len(line)):
    if t == 0:
      if line[i] == '\t':
        fields.append("")
        t = 1
    elif t == 1:
      if line[i] != '\t':
        fields.append("")
        t = 0
    fields[-1] = fields[-1] + line[i]
  k = fields[NAME_COLUMN]
  v = ParseInt(fields[NETBSD_COLUMN])
  if k in MAGNUMS and MAGNUMS[k] != v:
    fields[NETBSD_COLUMN] = FormatInt(MAGNUMS[k])
  # if fields[GROUP_COLUMN] == G:
    sys.stdout.write("".join(fields))
