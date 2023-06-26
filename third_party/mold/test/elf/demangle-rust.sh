// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

cat <<'EOF' | $CC -c -o $t/a.o -xc -
void _RNCINkXs25_NgCsbmNqQUJIY6D_4core5sliceINyB9_4IterhENuNgNoBb_4iter8iterator8Iterator9rpositionNCNgNpB9_6memchr7memrchrs_0E0Bb_();

int main() {
  _RNCINkXs25_NgCsbmNqQUJIY6D_4core5sliceINyB9_4IterhENuNgNoBb_4iter8iterator8Iterator9rpositionNCNgNpB9_6memchr7memrchrs_0E0Bb_();
}
EOF

! $CC -B. -o $t/exe $t/a.o 2> $t/log || false

grep -Fq '<core::slice::Iter<u8> as core::iter::iterator::Iterator>::rposition::<core::slice::memchr::memrchr::{closure#1}>::{closure#0}' $t/log
