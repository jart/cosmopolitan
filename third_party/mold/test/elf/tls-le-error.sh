// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<EOF | $GCC -fPIC -c -o $t/a.o -xc -
__attribute__((tls_model("local-exec"))) static _Thread_local int foo = 5;
int bar() { return foo; }
EOF

! $CC -B. -shared -o $t/b.so $t/a.o >& $t/log
grep -q 'relocation .* against `foo` can not be used when making a shared object; recompile with -fPIC' $t/log
