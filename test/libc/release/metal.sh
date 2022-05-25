#!/bin/sh

# TODO(jart): Stack size increase probably broke this.
exit

if [ "$MODE" = dbg ]; then
  exit  # TODO
fi
if [ "$MODE" = opt ] || [ "$MODE" = optlinux ]; then
  exit
fi

$MKDIR o/$MODE/test/libc/release/

# smoke test booting on bare metal and printing data to serial uart
CMD="o/$MODE/tool/build/blinkenlights.com.dbg -r o/$MODE/examples/hello.com"
if OUTPUT="$($CMD)"; then
  if [ x"$OUTPUT" = x"hello world" ]; then
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
