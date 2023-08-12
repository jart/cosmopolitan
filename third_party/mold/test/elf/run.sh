// clang-format off
#!/bin/bash
. $(dirname $0)/common.inc

[ "$CC" = cc ] || skip

# ASAN doesn't work with LD_PRELOAD
nm mold-wrapper.so | grep -q '__[at]san_init' && skip

cat <<'EOF' | $CC -xc -c -o $t/a.o -
#include "libc/calls/calls.h"
#include "libc/calls/weirdtypes.h"
#include "libc/fmt/fmt.h"
#include "libc/stdio/dprintf.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/temp.h"
#include "third_party/musl/tempnam.h"

int main() {
  printf("Hello\n");
  return 0;
}
EOF

LD_PRELOAD=`pwd`/mold-wrapper.so MOLD_PATH=`pwd`/mold \
  $GCC -o $t/exe $t/a.o -B/usr/bin
readelf -p .comment $t/exe > $t/log
grep -q '[ms]old' $t/log

./mold -run env | grep -q '^MOLD_PATH=.*/mold$'

./mold -run /usr/bin/ld --version | grep -q '[ms]old'
./mold -run /usr/bin/ld.lld --version | grep -q '[ms]old'
./mold -run /usr/bin/ld.gold --version | grep -q '[ms]old'

rm -f $t/ld $t/ld.lld $t/ld.gold $t/foo.ld
touch $t/ld $t/ld.lld $t/ld.gold
echo "#!/bin/sh" >$t/foo.ld
chmod 755 $t/ld $t/ld.lld $t/ld.gold $t/foo.ld

./mold -run $t/ld --version | grep -q '[ms]old'
./mold -run $t/ld.lld --version | grep -q '[ms]old'
./mold -run $t/ld.gold --version | grep -q '[ms]old'
./mold -run $t/foo.ld --version | grep -q '[ms]old' && false

cat <<'EOF' > $t/sh
#!/bin/sh
$1 --version
EOF

chmod 755 $t/sh

./mold -run $t/sh ld --version | grep -q '[ms]old'
./mold -run $t/sh foo.ld --version >& /dev/null | grep -q '[ms]old' && false

./mold -run $t/sh $t/ld --version | grep -q '[ms]old'
./mold -run $t/sh $t/ld.lld --version | grep -q '[ms]old'
./mold -run $t/sh $t/ld.gold --version | grep -q '[ms]old'
./mold -run $t/sh $t/foo.ld --version | grep -q '[ms]old' && false
