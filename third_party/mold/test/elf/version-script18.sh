// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

# Test version script precedence.

cat <<'EOF' > $t/a.ver
{ global: extern "C++" { *libalpha::*; }; local: *libbeta*; };
EOF

cat <<EOF | $CC -fPIC -c -o $t/b.o -xc++ -
namespace libbeta {
  struct Bar;
}
namespace libalpha {
  template <typename T>
  void foo() {}
  template void foo<libbeta::Bar>();
}
EOF

$CC -B. -shared -Wl,--version-script=$t/a.ver -o $t/c.so $t/b.o
readelf --wide --dyn-syms $t/c.so | grep libalpha | grep -q Bar

