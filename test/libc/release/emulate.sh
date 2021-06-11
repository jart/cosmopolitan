#!/bin/sh

if [ "$MODE" = dbg ]; then
  exit  # TODO
fi

if [ "$MODE" = opt ]; then
  exit
fi

# smoke test userspace binary emulation
CMD="o/$MODE/tool/build/blinkenlights.com.dbg o/$MODE/examples/hello.com"
if OUTPUT="$($CMD)"; then
  if [ x"$OUTPUT" = x"hello world" ]; then
    touch o/$MODE/test/libc/release/emulate.ok
    exit 0
  else
    printf '%s\n' "error: $CMD printed wrong output: $OUTPUT" >&2
    exit 1
  fi
else
  RC=$?
  printf '%s\n' "error: $CMD failed: $RC" >&2
  exit $RC
fi
