#!/bin/sh

if [ "$UID" = "0" ]; then
  SUDO=
else
  SUDO=sudo
fi

{
  echo
  echo "APE Uninstaller intends to run"
  echo
  echo "    $SUDO sh -c 'echo -1 >/proc/sys/fs/binfmt_misc/APE'"
  echo "    $SUDO rm -f /usr/bin/ape ~/.ape o/tmp/.ape /tmp/.ape"
  echo
  echo "You may then use ape/apeinstall.sh to reinstall it"
  echo
} >&2

set -ex
if [ -f /proc/sys/fs/binfmt_misc/APE ]; then
  $SUDO sh -c 'echo -1 >/proc/sys/fs/binfmt_misc/APE' || exit
fi
$SUDO rm -f /usr/bin/ape ~/.ape o/tmp/.ape o/tmp/ape /tmp/.ape /tmp/ape || exit
