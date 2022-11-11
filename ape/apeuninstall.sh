#!/bin/sh

if [ "$UID" = "0" ]; then
  SUDO=
else
  SUDO=sudo
fi

{
  echo
  echo "APE Uninstaller intends to run (in pseudo-shell)"
  echo
  echo "    sudo echo -1 into /proc/sys/fs/binfmt_misc/APE*"
  echo "    sudo rm -f /usr/bin/ape ~/.ape o/tmp/.ape /tmp/.ape"
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
$SUDO rm -f /usr/bin/ape ~/.ape o/tmp/.ape o/tmp/ape /tmp/.ape /tmp/ape || exit
