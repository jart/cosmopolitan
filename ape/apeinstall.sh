#!/bin/sh

if ! [ x"$(uname -s)" = xLinux ]; then
  echo this script is intended for linux binfmt_misc >&2
  echo freebsd/netbsd/openbsd users can use release binary >&2
  exit 1
fi

if [ -f o/depend ]; then
  # mkdeps.com build was successfully run so assume we can build
  echo >&2
  echo running: make -j8 o//ape/ape >&2
  make -j8 o//ape/ape || exit
  echo done >&2
else
  # no evidence we can build, use prebuilt one
  mkdir -p o//ape || exit
  cp -af build/bootstrap/ape o//ape/ape
fi

echo >&2
echo installing o//ape/ape to /usr/bin/ape >&2
echo sudo mv -f o//ape/ape /usr/bin/ape >&2
sudo mv -f o//ape/ape /usr/bin/ape || exit
echo done >&2

if [ -e /proc/sys/fs/binfmt_misc/APE ]; then
  echo >&2
  echo it looks like APE is already registered with binfmt_misc >&2
  echo please check that it is mapped to ape not /bin/sh >&2
  echo cat /proc/sys/fs/binfmt_misc/APE >&2
  cat /proc/sys/fs/binfmt_misc/APE >&2
  # TODO: we need better uninstall recommendations
  #       the following works fine for justine
  #       but might remove unrelated software?
  # sudo sh -c 'echo -1 >/proc/sys/fs/binfmt_misc/status'
  exit
fi

if ! [ -e /proc/sys/fs/binfmt_misc ]; then
  echo >&2
  echo loading binfmt_misc into your kernel >&2
  echo you may need to edit configs to persist across reboot >&2
  echo sudo modprobe binfmt_misc >&2
  sudo modprobe binfmt_misc || exit
  echo done >&2
fi

if ! [ -e /proc/sys/fs/binfmt_misc/register ]; then
  echo >&2
  echo mounting binfmt_misc into your kernel >&2
  echo you may need to edit configs to persist across reboot >&2
  echo sudo mount -t binfmt_misc none /proc/sys/fs/binfmt_misc >&2
  sudo mount -t binfmt_misc none /proc/sys/fs/binfmt_misc || exit
  echo done >&2
fi

echo >&2
echo registering APE with binfmt_misc >&2
echo you may need to edit configs to persist across reboot >&2
echo 'sudo sh -c "echo '"'"':APE:M::MZqFpD::/usr/bin/ape:'"'"' >/proc/sys/fs/binfmt_misc/register"' >&2
sudo sh -c "echo ':APE:M::MZqFpD::/usr/bin/ape:' >/proc/sys/fs/binfmt_misc/register" || exit
echo done >&2
