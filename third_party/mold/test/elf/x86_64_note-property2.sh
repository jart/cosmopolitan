// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

# Skip if target is not x86-64
[ $MACHINE = x86_64 ] || skip

# Binutils 2.32 injects their own .note.gnu.property section interfering with the tests
test_cflags -Xassembler -mx86-used-note=no && CFLAGS="-Xassembler -mx86-used-note=no" || CFLAGS=""

# This test requires the new ISA constants defined in Binutils 2.36
readelf -v | grep -q -E "GNU readelf .+ (2\.3[6-9]|2\.[4-9]|[3-9])" || skip

cat <<EOF | $CC $CFLAGS -c -o $t/a.o -xassembler-with-cpp  -
#define NT_GNU_PROPERTY_TYPE_0 5
#define GNU_PROPERTY_X86_ISA_1_USED 0xc0010002
#define GNU_PROPERTY_X86_ISA_1_NEEDED 0xc0008002
#define GNU_PROPERTY_X86_FEATURE_1_AND 0xc0000002

	.section ".note.gnu.property", "a"
	.p2align 3
	.long 4
	.long 1f - 0f
	.long NT_GNU_PROPERTY_TYPE_0
	.asciz "GNU"
0:	.long GNU_PROPERTY_X86_ISA_1_NEEDED
	.long 4
	.long 0x7
	.p2align 3
	.long GNU_PROPERTY_X86_ISA_1_USED
	.long 4
	.long 0x7
	.p2align 3
	.long GNU_PROPERTY_X86_FEATURE_1_AND
	.long 4
	.long 0x3
	.p2align 3
1:
EOF

cat <<EOF | $CC $CFLAGS -c -o $t/b.o -xassembler-with-cpp -
#define NT_GNU_PROPERTY_TYPE_0 5
#define GNU_PROPERTY_X86_ISA_1_USED 0xc0010002
#define GNU_PROPERTY_X86_ISA_1_NEEDED 0xc0008002
#define GNU_PROPERTY_X86_FEATURE_1_AND 0xc0000002

	.section ".note.gnu.property", "a"
	.p2align 3
	.long 4
	.long 1f - 0f
	.long NT_GNU_PROPERTY_TYPE_0
	.asciz "GNU"
0:	.long GNU_PROPERTY_X86_ISA_1_NEEDED
	.long 4
	.long 0x8
	.p2align 3
	.long GNU_PROPERTY_X86_ISA_1_USED
	.long 4
	.long 0x8
	.p2align 3
	.long GNU_PROPERTY_X86_FEATURE_1_AND
  .long 4
  .long 0x2
  .p2align 3
1:
EOF

cat <<EOF | $CC $CFLAGS -c -o $t/c.o -xassembler-with-cpp -
EOF

./mold -nostdlib -o $t/exe1 $t/a.o $t/b.o
readelf -n $t/exe1 | grep -q 'x86 feature: SHSTK'
readelf -n $t/exe1 | grep -q 'x86 ISA needed: x86-64-baseline, x86-64-v2, x86-64-v3, x86-64-v4'
readelf -n $t/exe1 | grep -q 'x86 ISA used: x86-64-baseline, x86-64-v2, x86-64-v3, x86-64-v4'

./mold -nostdlib -o $t/exe2 $t/a.o $t/b.o $t/c.o
! readelf -n $t/exe2 | grep -q 'x86 feature: SHSTK' || false
readelf -n $t/exe2 | grep -q 'x86 ISA needed: x86-64-baseline, x86-64-v2, x86-64-v3, x86-64-v4' || false
! readelf -n $t/exe2 | grep -q 'x86 ISA used: x86-64-baseline, x86-64-v2, x86-64-v3, x86-64-v4'

./mold --relocatable -o $t/d.o $t/a.o $t/b.o
readelf -n $t/d.o | grep -q 'x86 feature: SHSTK'
readelf -n $t/d.o | grep -q 'x86 ISA needed: x86-64-baseline, x86-64-v2, x86-64-v3, x86-64-v4'
readelf -n $t/d.o | grep -q 'x86 ISA used: x86-64-baseline, x86-64-v2, x86-64-v3, x86-64-v4'
