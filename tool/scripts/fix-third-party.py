#!/usr/bin/env python.com
#
# fix-third-party.py
# normalizes third party codebases to cosmopolitan style
#
# this script does the bulk of the work when it comes to fixing things
# like #include lines so they follow the local convention. this script
# won't get you all the way, but it'll get you pretty far w/ vendoring
#
# for example:
#
#     cd bash-5.1.16
#     mkdir ~/cosmo/third_party/bash
#     cp -R *.c *.h builtins lib COPYING include/* ~/cosmo/third_party/bash
#     cd ~/cosmo
#     tool/scripts/fix-third-party.py third_party/bash
#

import os
import re
import sys

LIBC_ISYSTEM = 'libc/isystem/'
EXTENSIONS = ('.c', '.cc', '.cpp', '.h', '.hh', '.hpp', '.inc', '.tab', 'h.in')

isystem = {}
for dirpath, dirs, files in os.walk(LIBC_ISYSTEM):
  for filepath in files:
    path = os.path.join(dirpath, filepath)
    with open(path) as f:
      code = f.read()
    name = path[len(LIBC_ISYSTEM):]
    deps = re.findall(r'[.#]include "([^"]+)"', code)
    isystem[name] = '\n'.join('#include "%s"' % (d) for d in deps)

def FixQuotedPath(path, incl):
  p2 = os.path.join(os.path.dirname(path), incl)
  if os.path.exists(incl) or not os.path.exists(p2):
    return incl
  else:
    return os.path.normpath(p2)

def FixThirdParty(path):
  # if not path.endswith(EXTENSIONS):
  #   return

  print(path)
  with open(path) as f:
    code = f.read()

  start = 0
  res = []
  for m in re.finditer(r'(?:/[/*] MISSING )?#\s*include\s*"([^"]+)"(?: \*/)?', code):
    end, newstart = m.span()
    res.append(code[start:end])
    inc = FixQuotedPath(path, m.group(1))
    if os.path.exists(inc):
      res.append('#include "%s"' % (inc))
    else:
      res.append('// MISSING #include "%s"' % (inc))
    start = newstart
  res.append(code[start:])
  code = ''.join(res)

  res = []
  start = 0
  for m in re.finditer(r'(?:/[/*] MISSING )?#\s*include\s*<([^>]+)>(?: \*/)?', code):
    end, newstart = m.span()
    res.append(code[start:end])
    inc = m.group(1)
    if inc in isystem:
      res.append(isystem[inc])
    else:
      res.append('// MISSING #include <%s>' % (m.group(1)))
    start = newstart
  res.append(code[start:])

  code = ''.join(res)
  # print(code)
  with open(path, 'wb') as f:
    f.write(code.encode('utf-8'))

for arg in sys.argv[1:]:
  if os.path.isdir(arg):
    for dirpath, dirs, files in os.walk(arg):
      for filepath in files:
        FixThirdParty(os.path.join(dirpath, filepath))
  else:
    FixThirdParty(arg)
