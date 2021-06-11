#!/bin/sh

if [ "$MODE" = dbg ]; then
  exit  # TODO
fi

if [ "$MODE" = opt ]; then
  exit
fi

# smoke test booting on bare metal and printing data to serial uart
CMD="o/$MODE/tool/build/blinkenlights.com.dbg -r o/$MODE/examples/hello.com"
if OUTPUT="$($CMD)"; then
  if [ x"$OUTPUT" = x"hello world" ]; then
    touch o/$MODE/test/libc/release/metal.ok
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
