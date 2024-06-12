#!/usr/bin/env python

import os
import re
import sys

def GetDeps(path):
  visited = set()
  def Dive(path, inside, depth, that_isnt=None):
    included = path

    if not os.path.exists(path):
      if inside:
        samedir = os.path.join(os.path.dirname(inside), path)
      if inside and os.path.exists(samedir) and samedir != that_isnt:
        path = samedir
      elif os.path.exists('third_party/libcxx/' + path) and 'third_party/libcxx/' + path != that_isnt:
        path = 'third_party/libcxx/' + path
      elif os.path.exists('libc/isystem/' + path):
        path = 'libc/isystem/' + path
      else:
        # sys.stderr.write('not found: %s\n' % (path))
        return

    sys.stdout.write('\t' * depth)
    sys.stdout.write(path)
    sys.stdout.write('\n')

    if path in visited:
      return
    visited.add(path)

    with open(path) as f:
      code = f.read()
    for dep in re.findall(r'[.#]\s*include\s+[<"]([^">]+)[">]', code):
      Dive(dep, path, depth + 1)
    for dep in re.findall(r'[.#]\s*include_next\s+[<"]([^">]+)[">]', code):
      Dive(dep, path, depth + 1, path)
  Dive(path, None, 0)

once = False
for arg in sys.argv[1:]:
  if os.path.isdir(arg):
    for dirpath, dirs, files in os.walk(arg):
      for filepath in files:
        if not once:
          sys.stdout.write('\n')
          once = True
        GetDeps(os.path.join(dirpath, filepath))
  else:
    GetDeps(arg)
