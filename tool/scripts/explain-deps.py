#!/usr/bin/env python.com

import os
import re
import sys

def GetDeps(path):
  def Dive(path, depth, visited):
    sys.stdout.write('%s%s' % ('\t' * depth, path))
    if path in visited:
      sys.stdout.write(' cycle\n')
      return
    sys.stdout.write('\n')
    with open(path) as f:
      code = f.read()
    for dep in re.findall(r'[.#]include "([^"]+)"', code):
      Dive(dep, depth + 1, visited + [path])
  Dive(path, 0, [])
  sys.stdout.write('\n')

for arg in sys.argv[1:]:
  if os.path.isdir(arg):
    for dirpath, dirs, files in os.walk(arg):
      for filepath in files:
        GetDeps(os.path.join(dirpath, filepath))
  else:
    GetDeps(arg)
