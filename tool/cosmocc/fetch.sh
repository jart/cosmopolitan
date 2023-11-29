#!/bin/sh
# tool/cosmocc/fetch.sh
# sets up mono repo to use latest cosmocc toolchain

URL=https://cosmo.zip/pub/cosmocc/cosmocc.zip

echo creating o/third_party/gcc... >&2
rm -rf o/third_party/gcc || exit
mkdir -p o/third_party/gcc || exit
cd o/third_party/gcc

echo downloading $URL... >&2
if command -v wget >/dev/null; then
  wget $URL || exit
else
  curl -Lo cosmocc.zip $URL || exit
fi

echo extracting cosmocc.zip... >&2
unzip cosmocc.zip || exit
rm -f cosmocc.zip

# report success and give tips
cat <<'EOF' >&2

SUCCESS

  the cosmo mono repo will now build using cosmocc

WARNING

  if you run `make clean` then you'll have to download it again

CONSIDER

  you can install the cosmocc toolchain you just downloaded as follows

    sudo chmod 01777 /opt
    rm -rf /opt/cosmocc
    mv o/third_party/gcc /opt/cosmocc
    ln -sf /opt/cosmocc o/third_party/gcc
    echo 'export PATH="/opt/cosmocc/bin:$PATH"' >>~/.profile

  then just recreate the symlink after cleaning. please note that you
  should NEVER need to use make clean with the cosmo mono repo, since
  our Makefile is flawless. if make clean actually helps, file a bug.

EOF

# give tip to linux users
if [ x"$(uname -s)" = x"Linux" ]; then
  cat <<'EOF' >&2
PROTIP

  running these commands

    rm -rf o
    sudo mount -t tmpfs -o size=10G,noatime,nodiratime /dev/shm o
    ln -sf /opt/cosmocc o/third_party/gcc

  makes the mono repo build faster

EOF
fi
