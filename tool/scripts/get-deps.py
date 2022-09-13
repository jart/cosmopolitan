#!/usr/bin/env python.com
#
# OVERVIEW
#
#   One-Off Makefile Rule Generator
#
# EXAMPLES
#
#     tool/scripts/get-deps.py examples/hello.c
#     asmexpr 'mov $0,%ecx' 'vmovd %ecx,%xmm1' 'vpbroadcastb %xmm1,%ymm1' 'mov $0x20202032489001ff,%rax' 'vmovq %rax,%xmm0' 'vpcmpgtb %ymm1,%ymm0,%ymm2'
#

import os
import re
import sys

def GetDeps(path):
  sys.stdout.write('o/$(MODE)/%s.o:' % (os.path.splitext(path)[0]))
  deps = set()
  def Dive(path):
    if path in deps:
      return
    deps.add(path)
    sys.stdout.write(' \\\n\t\t%s' % (path))
    with open(path) as f:
      code = f.read()
    for dep in re.findall(r'[.#]include "([^"]+)"', code):
      Dive(dep)
  Dive(path)
  sys.stdout.write('\n')

for arg in sys.argv[1:]:
  if os.path.isdir(arg):
    for dirpath, dirs, files in os.walk(arg):
      for filepath in files:
        GetDeps(os.path.join(dirpath, filepath))
  else:
    GetDeps(arg)
