#!/bin/sh

PROG=${0##*/}
MODE=${MODE:-$m}
COSMO=${COSMO:-/opt/cosmo}
COSMOS=${COSMOS:-/opt/cosmos}

if [ ! -f ape/loader.c ]; then
  cd "$COSMO" || exit
fi

if [ "$UID" = "0" ]; then
  SUDO=
elif command -v sudo >/dev/null 2>&1; then
  SUDO=sudo
elif command -v doas >/dev/null 2>&1; then
  SUDO=doas
else
  echo "need root or sudo" >&2
  exit
fi

{
  echo
  echo "APE Uninstaller intends to run (in pseudo-shell)"
  echo
  echo "    sudo echo -1 into /proc/sys/fs/binfmt_misc/APE*"
  echo "    sudo rm -f /usr/bin/ape ~/.ape /tmp/.ape # etc."
  echo
  echo "You may then use ape/apeinstall.sh to reinstall it"
  echo
} >&2

set -ex
for f in /proc/sys/fs/binfmt_misc/APE*; do
  if [ -f $f ]; then
    $SUDO sh -c "echo -1 >$f" || exit
  fi
done

# system installation
if [ -f /usr/bin/ape ]; then
  $SUDO rm -f /usr/bin/ape
fi
if [ -f /usr/local/bin/ape ]; then
  $SUDO rm -f /usr/local/bin/ape
fi

# legacy installations
rm -f o/tmp/ape /tmp/ape "${TMPDIR:-/tmp}/ape"

# ad-hoc installations
for x in .ape \
         .ape-1.1 \
         .ape-1.3 \
         .ape-1.4 \
         .ape-1.5 \
         .ape-1.6 \
         .ape-1.7 \
         .ape-1.8 \
         .ape-1.9 \
         .ape-1.10; do
  rm -f \
     ~/$x \
     /tmp/$x \
     o/tmp/$x \
     "${TMPDIR:-/tmp}/$x"
done
