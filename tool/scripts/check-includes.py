#!/usr/bin/env python.com

import os
import sys

def CheckFile(path):
  if path.endswith(('.png', '.ico')):
    return
  sys.stderr.write('%s\n' % (path))
  with open(path) as f:
    data = f.read()
  assert '#include' not in data[65530:], "late include in %s" % (path)

for arg in sys.argv[1:]:
  if os.path.isdir(arg):
    for dirpath, dirs, files in os.walk(arg):
      for filepath in files:
        CheckFile(os.path.join(dirpath, filepath))
  else:
    CheckFile(arg)
