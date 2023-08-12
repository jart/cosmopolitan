// clang-format off
#!/bin/bash
#
# This test script takes a Gentoo package name and tries to build it
# using mold in a Docker environment. We chose Gentoo Linux as a test
# target, because its source-based package allows us to build programs
# locally and run their test suites without any hassle.
#
# You can get a complete list of Gentoo packages availalbe for testing
# with the following command:
#
# docker run --rm mold-gentoo emerge --color n -s '' | \
#   perl -ne 'next unless m!^\*\s+(\S+/\S+)!; print "$1\n"'

package="$1"

if [ "$package" = "" ]; then
  echo "Usage: $0 gentoo-package-name"
  exit 1
fi

set -x

# Create a Docker image
if ! docker image ls mold-gentoo | grep -q mold-gentoo; then
  set -e
  cat <<EOF | docker build -t mold-gentoo -
FROM gentoo/stage3
RUN emerge-webrsync
RUN echo 'USE="X ssl elogind -systemd corefonts truetype jpeg jpeg2k tiff zstd static-libs binary"' >> /etc/portage/make.conf && \
    echo 'ACCEPT_KEYWORDS="~amd64"' >> /etc/portage/make.conf && \
    echo 'ACCEPT_LICENSE="* -@EULA"' >> /etc/portage/make.conf && \
    echo 'FEATURES="\${FEATURE} noclean nostrip ccache -ipc-sandbox -network-sandbox -pid-sandbox -sandbox"' >> /etc/portage/make.conf && \
    echo 'CCACHE_DIR="/ccache"' >> /etc/portage/make.conf
RUN emerge gdb lld clang vim emacs strace ccache xeyes dev-util/cmake dev-vcs/git && rm -rf /var/tmp/portage
EOF
  set +e
fi

git_hash=$(./mold --version | perl -ne '/\((\w+)/; print $1;')

if [ "$package" = dev-libs/concurrencykit ]; then
  echo "Skipping known broken package: $package"
  exit 0
fi

# Build a given package in Docker
cmd1='(cd /usr/bin; ln -sf /mold/mold $(realpath ld))'
cmd2="MAKEOPTS=-'j$(nproc) --load-average=100' emerge --onlydeps $package"
cmd3="MAKEOPTS='-j$(nproc) --load-average=100' FEATURES=test emerge $package"
filename=`echo "$package" | sed 's!/!_!g'`
docker="docker run --rm --cap-add=SYS_PTRACE -v `pwd`:/mold -v /var/cache/ccache-gentoo:/ccache mold-gentoo timeout -v -k 15s 1h"
dir=gentoo/$git_hash

mkdir -p "$dir"/success "$dir"/failure

$docker nice -n 19 bash -c "$cmd1 && $cmd2 && $cmd3" >& "$dir"/"$filename".mold
if [ $? = 0 ]; then
  mv "$dir"/"$filename".mold "$dir"/success
else
  mv "$dir"/"$filename".mold "$dir"/failure
fi

$docker nice -n 19 bash -c "$cmd2 && $cmd3" >& "$dir"/"$filename".ld
if [ $? = 0 ]; then
  mv "$dir"/"$filename".ld "$dir"/success
else
  mv "$dir"/"$filename".ld "$dir"/failure
fi
