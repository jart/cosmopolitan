#!/usr/bin/env python3
# Copyright (c) The mldsa-native project authors
# SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

import struct as st
import sys
import subprocess
import tempfile
import os


def err(msg, **kwargs):
    print(msg, file=sys.stderr, **kwargs)


binpath = sys.argv[1]
args = sys.argv[1:]
cmdline_offset = 0x70000

arg0_offset = cmdline_offset + 4 + len(args) * 4

arg_offsets = [sum(map(len, args[:i])) + i + arg0_offset for i in range(len(args))]

binargs = st.pack(
    f"<{1+len(args)}I" + "".join(f"{len(a)+1}s" for a in args),
    len(args),
    *arg_offsets,
    *map(lambda x: x.encode("utf-8"), args),
)

with tempfile.NamedTemporaryFile(mode="wb", delete=False, suffix=".bin") as fd:
    args_file = fd.name
    fd.write(binargs)

try:
    qemu_cmd = f"qemu-system-arm -M mps3-an547 -nographic -semihosting -kernel {binpath} -device loader,file={args_file},addr=0x{cmdline_offset:x}".split()
    result = subprocess.run(qemu_cmd, encoding="utf-8", capture_output=True)
finally:
    os.unlink(args_file)
if result.returncode != 0:
    err("FAIL!")
    err(f"{qemu_cmd} failed with error code {result.returncode}")
    err(result.stderr)
    exit(1)

for line in result.stdout.splitlines():
    print(line)
